#include "StudentWorld.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

GameWorld* createStudentWorld(string assetDir) {
    return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir) {
    this->m_earthTiles.resize(60, std::vector<Earth*>(64));
    this->m_tunnelMan = nullptr;
}

StudentWorld::~StudentWorld() {
}

int StudentWorld::init() {
    this->m_gameTick = 0;
    this->m_lastProtesterTick = 0;

    this->createEarth();

    this->m_tunnelMan = new TunnelMan(this, this->m_earthTiles);
    this->m_gameObjects.push_back(this->m_tunnelMan);

    this->createObjects();

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    ++this->m_gameTick;

    this->setGameStatText(this->getGameStatText());

    this->createGoodie();
    this->spawnProtester();

    for (int i = 0; i < this->m_gameObjects.size(); ++i) {
        Actor* actor = this->m_gameObjects[i];

        if (actor->isAlive()) {
            actor->doSomething();
        }

        // Lose condition
        if (!this->m_tunnelMan->isAlive()) {
            this->playSound(SOUND_PLAYER_GIVE_UP);
            this->decLives();
            return GWSTATUS_PLAYER_DIED;
        }

        // Win condition
        if (this->m_barrelsLeft == 0) {
            this->playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    for (Actor* actor : this->m_gameObjects) {
        if (!actor->isAlive()) {
            actor->setVisible(false);
        }
    }

    if (!this->m_tunnelMan->isAlive()) {
        this->playSound(SOUND_PLAYER_GIVE_UP);
        this->decLives();
        return GWSTATUS_PLAYER_DIED;
    }

    // Win condition
    if (this->m_barrelsLeft == 0) {
        this->playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    // Delete all dynamically allocated Earth objects
    for (int x = 0; x < EARTH_MAX_X; ++x) {
        for (int y = 0; y < EARTH_MAX_Y; y++) {
            if (this->m_earthTiles[x][y] != nullptr) {
                delete this->m_earthTiles[x][y];
            }
        }
    }

    // Delete all game objects
    for (Actor* actor : this->m_gameObjects) {
        if (actor != nullptr) {
            delete actor;
        }
    }

    // Clear game object and protester vector storage
    this->m_gameObjects.clear();
    this->m_protesters.clear();

    return;
}

void StudentWorld::createEarth() {
    for (int x = 0; x < 60; x++) {
        for (int y = 0; y < 64; y++) {
            if (x < START_COL || x > END_COL || y < 4) {
                this->m_earthTiles[x][y] = new Earth(this, x, y);
            }
        }
    }
}

void StudentWorld::createObjects() {
    // get number of items according to level
    int boulderAmount = min(static_cast<int>(this->getLevel()) / 2 + 2, 9);
    int goldAmount = max(5 - static_cast<int>(this->getLevel()) / 2, 2);
    int barrelAmount = min(2 + static_cast<int>(this->getLevel()), 21);

    this->m_barrelsLeft = barrelAmount;

    createObject(boulderAmount, TID_BOULDER);
    createObject(goldAmount, TID_GOLD);
    createObject(barrelAmount, TID_BARREL);
}

void StudentWorld::createObject(int numObjects, int objectID) {
    auto distance = [](int x1, int y1, int x2, int y2) -> double {
        return sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
    };

    int created = 0;
    while (created < numObjects) {
        int x = rand() % 60;
        // If the objectID is a boulder, 19 <= y <= 55, else, 0 <= y <= 55
        int y = (objectID == TID_BOULDER)
                    ? (19 + rand() % 37)
                    : (rand() % 56);

        bool okDistance = true;
        for (const auto& object : this->m_gameObjects) {
            if (distance(x, y, object->getX(), object->getY()) <= 6) {
                okDistance = false;
                break;
            }
        }

        if (okDistance) {
            switch (objectID) {
                case TID_BOULDER:
                    this->m_gameObjects.push_back(new Boulder(this, x, y, STABLE, this->m_tunnelMan));
                    break;

                case TID_GOLD:
                    this->m_gameObjects.push_back(new GoldNugget(this, x, y, this->m_tunnelMan, false, TUNNELMAN, PERMANENT));
                    break;

                case TID_BARREL:
                    this->m_gameObjects.push_back(new Barrel(this, x, y, this->m_tunnelMan));
                    break;
            }
            ++created;
        }
    }
}

void StudentWorld::createGoodie() {
    // Check if random number from 0 to goodieChance (exclusive) equals 0
    int goodieChance = this->getLevel() * 25 + 300;
    if (rand() % goodieChance != 0) {
        return;
    }

    // Get random number from 0 to 5 (exclusive) to determine goodie to spawn
    int randomGoodie = rand() % 5;

    // Sonar (1/5)
    if (randomGoodie == 0) {
        return this->m_gameObjects.push_back(new SonarKit(this, 0, 60, this->m_tunnelMan));
    }
    // Water Pool (4/5)
    // Find appropriate place for water pool
findWaterPoolCoords:
    int x = rand() % 60;
    int y = rand() % 60;
    const int maxY = y + 4 > EARTH_MAX_Y ? EARTH_MAX_Y : y + 4;

    for (int earthX = x; earthX < x + 4; ++earthX) {
        for (int earthY = y; earthY < maxY; ++earthY) {
            const Earth* earthTile = this->m_earthTiles[earthX][earthY];
            // If a coordinate overlaps a visible earth tile, look for new random coords
            if (earthTile != nullptr && earthTile->isVisible()) {
                goto findWaterPoolCoords;
            }
        }
    }
    return this->m_gameObjects.push_back(new WaterPool(this, x, y, this->m_tunnelMan));
}

void StudentWorld::spawnProtester() {
    // Protester cannot be spawned if game tick is too early, or max amount of protesters have already been spawned
    const int currentLevel = static_cast<int>(this->getLevel());
    if (this->m_gameTick < this->m_lastProtesterTick || this->m_protesters.size() >= std::min(15, static_cast<int>(2 + currentLevel * 1.5))) {
        return;
    }

    // Set cooldown for new protester spawn
    this->m_lastProtesterTick = this->m_gameTick + std::max(25, 200 - currentLevel);

    Protester* protester = new RegularProtester(this, this->m_tunnelMan);
    // Check if hardcore protester should spawn
    // if ((rand() % 100) < std::min(90, currentLevel * 10 + 30)) {
    //     protester = new HardcoreProtester(this, this->m_tunnelMan);
    // } else {
    // protester = new RegularProtester(this, this->m_tunnelMan);
    // }

    this->m_gameObjects.push_back(protester);
    this->m_protesters.push_back(protester);
}

std::string StudentWorld::getGameStatText() {
    stringstream text;
    const int percentHealth = static_cast<double>(this->m_tunnelMan->getHP()) / static_cast<double>(this->m_tunnelMan->getMaxHP()) * 100;
    text << "Lvl: " << setw(2) << getLevel()
         << " Lives: " << getLives()
         << " Hlth: " << setw(3) << percentHealth << "%"
         << " Wtr: " << setw(2) << this->m_tunnelMan->getWaterUnits()
         << " Gld: " << setw(2) << this->m_tunnelMan->getGoldNuggets()
         << " Oil Left: " << setw(2) << this->m_barrelsLeft
         << " Sonar: " << setw(2) << this->m_tunnelMan->getSonarCharges()
         << " Scr: " << setw(6) << setfill('0') << getScore();
    return text.str();
}
