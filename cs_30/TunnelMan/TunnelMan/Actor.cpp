#include "Actor.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction direction, double size, unsigned depth)
    : world(world),
      m_alive(std::nullopt),
      GraphObject(imageID, startX, startY, direction, size, depth) {
}

Actor::~Actor() {
}

bool Actor::getKeyPress(int& keyStore) {
    // Uses StudentWorld object's getKey function to get current key being pressed
    return this->world->getKey(keyStore);
}

bool Actor::isMovementKey(const int key) {
    // Based on GameConstants.h, valid movement keys range from 1000 to 1003 inclusive
    return key >= 1000 && key <= 1003;
}

GraphObject::Direction Actor::toDirection(const int key) {
    switch (key) {
        case KEY_PRESS_UP:
            return Direction::up;

        case KEY_PRESS_DOWN:
            return Direction::down;

        case KEY_PRESS_LEFT:
            return Direction::left;

        case KEY_PRESS_RIGHT:
            return Direction::right;
    }
    return Direction::none;
}

bool Actor::isValidMove(const Direction& direction, std::pair<int, int> moveCoords, std::unordered_set<int> invalidObjects) {
    switch (direction) {
        case up:
            ++moveCoords.second;
            break;
        case down:
            --moveCoords.second;
            break;
        case left:
            --moveCoords.first;
            break;
        case right:
            ++moveCoords.first;
            break;
    }

    for (Actor* actor : this->world->m_gameObjects) {
        if (invalidObjects.find(actor->getID()) == invalidObjects.end() || !actor->isVisible()) {
            continue;
        }

        const std::pair<int, int> coords = {actor->getX(), actor->getY()};
        // Check if upon moving, actor overlaps boulder, therefore invalid move
        if (abs(coords.first - moveCoords.first) < 4 && abs(coords.second - moveCoords.second) < 4) {
            return false;
        }
    }

    // Check if moving in direction will be out of bounds
    switch (direction) {
        case up:
            return this->getY() < MAX_COL;

        case down:
            return this->getY() > 0;

        case left:
            return this->getX() > 0;

        case right:
            return this->getX() < MAX_ROW;
    }
    return false;
}

void Actor::move(const Direction direction) {
    switch (direction) {
        case up:
            return this->moveTo(this->getX(), this->getY() + 1);

        case down:
            return this->moveTo(this->getX(), this->getY() - 1);

        case left:
            return this->moveTo(this->getX() - 1, this->getY());

        case right:
            return this->moveTo(this->getX() + 1, this->getY());
    }
}

double Actor::distance(int x1, int y1, int x2, int y2) {
    return std::abs(std::sqrt(std::pow(x2 - x1, 2.0) + std::pow(y2 - y1, 2.0)));
}

bool Actor::isAlive() {
    return this->m_alive.value_or(true);
}

void Actor::setAlive(bool alive) {
    this->m_alive = alive;
}

int Actor::getHP() {
    return this->m_hitPoints.value_or(-1);
}

void Actor::setHP(int hp) {
    this->m_hitPoints = hp;
}

int Actor::getMaxHP() {
    return this->m_maxHealth.value_or(-1);
}

int Actor::getAnnoyance() {
    return this->m_annoyance.value_or(-1);
}

void Actor::setAnnoyance(int annoyance) {
    this->m_annoyance = annoyance;
}

TunnelMan::TunnelMan(StudentWorld* world, std::vector<std::vector<Earth*>>& earthTiles)
    : m_earthTiles(earthTiles),
      m_waterUnits(5),
      m_sonarCharges(1),
      m_goldNuggets(1),  // TODO: CHANGE BACK TO 0
      Actor(world, TID_PLAYER, 30, 60, right, 1.0, 0) {
    this->m_hitPoints = 10;
    this->m_maxHealth = 10;
    this->m_alive = true;
    this->m_annoyance = 0;
    this->setVisible(true);
}

TunnelMan::~TunnelMan() {
    this->setVisible(false);
}

void TunnelMan::handleMovement(const Direction moveDirection) {
    // If tunnelman attempts to move in direction that he isn't facing,
    // change his direction to moveDirection
    if (moveDirection != this->getDirection()) {
        return this->setDirection(moveDirection);
    }
    // Else tunnelman must be facing in the direction he is facing -> move
    if (this->isValidMove(moveDirection, {this->getX(), this->getY()}, std::unordered_set<int>{TID_BOULDER})) {
        this->move(moveDirection);
        this->mineEarth();
    }
}

void TunnelMan::mineEarth() {
    const int currentX = this->getX();
    const int currentY = this->getY();
    const int maxY = currentY + 4 > EARTH_MAX_Y ? EARTH_MAX_Y : currentY + 4;
    bool overlap = false;
    for (int x = currentX; x < currentX + 4; ++x) {
        for (int y = currentY; y < maxY; ++y) {
            Earth* tile = this->m_earthTiles[x][y];
            if (x < EARTH_MAX_X && y < EARTH_MAX_Y && tile != nullptr && tile->isAlive()) {
                tile->setVisible(false);
                tile->setAlive(false);
                overlap = true;
            }
        }
    }
    if (overlap) {
        this->world->playSound(SOUND_DIG);
    }
}

void TunnelMan::doSomething() {
    if (!this->isAlive()) {
        return;
    }

    if (this->getHP() <= 0) {
        this->setAlive(false);
        this->setVisible(false);
        return;
    }

    int key = 0;  // Storage for the value of the key pressed
    // If invalid/no key is pressed, return early
    if (!this->getKeyPress(key)) {
        return;
    }

    // Abort level
    if (key == KEY_PRESS_ESCAPE) {
        return this->setAlive(false);
    }

    // Fire squirt
    if (key == KEY_PRESS_SPACE) {
        return this->handleSquirt();
    }

    // If a movement key has been pressed, handleMovement
    if (this->isMovementKey(key)) {
        return this->handleMovement(this->toDirection(key));
    }

    // Sonar charge
    if (key == 'Z' || key == 'z') {
        return this->handleSonar();
    }

    // Gold nugget
    if (key == KEY_PRESS_TAB) {
        return this->handleGoldNugget();
    }
}

void TunnelMan::handleSquirt() {
    // No water left
    if (this->getWaterUnits() <= 0) {
        return;
    }
    this->world->playSound(SOUND_PLAYER_SQUIRT);
    this->world->m_gameObjects.push_back(new Squirt(this->world, this, this->getX(), this->getY(), this->getDirection()));
    this->setWaterUnits(this->getWaterUnits() - 1);
    return;
}

void TunnelMan::handleSonar() {
    // No sonar charges
    if (this->getSonarCharges() <= 0) {
        return;
    }

    this->setSonarCharges(this->getSonarCharges() - 1);
    // Rubric doesn't reference SOUND_SONAR AFAIK so this usage is assumed
    this->world->playSound(SOUND_SONAR);
    // Illuminate all hidden alive objects within radius 12
    for (Actor* object : this->world->m_gameObjects) {
        // Check if object is dead or already illuminated/visible
        if (!object->isAlive() || object->isVisible()) {
            continue;
        }
        const double distance = this->distance(this->getX(), this->getY(), object->getX(), object->getY());
        if (distance <= 120.0) {  // TODO: CHANGE BACK TO 12
            object->setVisible(true);
        }
    }
}

void TunnelMan::handleGoldNugget() {
    // No gold nuggets
    if (this->getGoldNuggets() <= 0) {
        return;
    }

    this->setGoldNuggets(this->getGoldNuggets() - 1);
    // Create gold nugget with following properties:
    //      Same position as tunnelman, pickupable by protesters, temporary lifespan
    this->world->m_gameObjects.push_back(
        new GoldNugget(this->world, this->getX(), this->getY(), this, true, PROTESTER, TEMPORARY));
}

int TunnelMan::getWaterUnits() {
    return this->m_waterUnits;
}

void TunnelMan::setWaterUnits(int waterUnits) {
    this->m_waterUnits = waterUnits;
}

int TunnelMan::getSonarCharges() {
    return this->m_sonarCharges;
}

void TunnelMan::setSonarCharges(int sonarCharges) {
    this->m_sonarCharges = sonarCharges;
}

int TunnelMan::getGoldNuggets() {
    return this->m_goldNuggets;
}

void TunnelMan::setGoldNuggets(int goldNuggets) {
    this->m_goldNuggets = goldNuggets;
}

Earth::Earth(StudentWorld* world, int startX, int startY)
    : Actor(world, TID_EARTH, startX, startY, right, 0.25, 3) {
    this->m_alive = true;
    this->setVisible(true);
}

Earth::~Earth() {
}

void Earth::doSomething() {
    return;
}

Protester::Protester(StudentWorld* world, int hitPoints, int imageID, TunnelMan* tunnelMan)
    : tunnelMan(tunnelMan),
      m_hitPoints(hitPoints),
      m_leaveOilField(false),
      m_state(REST),
      Actor(world, imageID, 60, 60, left, 1.0, 0) {
    this->m_annoyance = 0;
    this->m_movementTick = 0;
    this->m_lastShoutTick = 0;
    this->m_lastPerpendicularTurn = 0;
    this->m_restTickAmount = std::max(0, 3 - static_cast<int>(this->world->getLevel()) / 4);

    // Random number [8, 60]
    this->m_squaresToMove = (rand() % 53) + 8;
}

ProtesterState Protester::getState() {
    return this->m_state;
}

void Protester::setState(ProtesterState state) {
    this->m_state = state;
}

int Protester::getMovementTick() {
    return this->m_movementTick;
}

void Protester::setMovementTick(int movementTick) {
    this->m_movementTick = movementTick;
}

void Protester::handleDamage() {
    // If Protester has been sufficientely annoyed
    if (this->getHP() <= 0) {
        this->m_state = LEAVE_THE_OIL_FIELD;
        this->world->playSound(SOUND_PROTESTER_GIVE_UP);
        this->m_restTickAmount = 0;
        return;
    }

    // Handle non lethal damage
    this->world->playSound(SOUND_PROTESTER_ANNOYED);
    this->m_movementTick += std::max(50, 100 - static_cast<int>(this->world->getLevel()) * 10);
}

bool Protester::canPerformMovement() {
    return this->world->m_gameTick > this->m_movementTick;
}

void Protester::setNextMovementTick() {
    this->m_movementTick = this->world->m_gameTick + this->m_restTickAmount;
}

bool Protester::isEarth(std::pair<int, int> current, Direction direction) {
    const auto& earthTiles = this->world->m_earthTiles;

    if (direction == up) {
        if (current.second + 4 == 60) {
            return false;
        }
        if (current.second + 4 > 60) {
            return true;
        }
        for (int x = current.first; x < current.first + 4; ++x) {
            Earth* tile = earthTiles[x][current.second + 4];
            if (tile != nullptr && tile->isVisible()) {
                return true;
            }
        }
        return false;
    }
    if (direction == down) {
        if (current.second - 1 < 0) {
            return true;
        }
        for (int x = current.first; x < current.first + 4; ++x) {
            Earth* tile = earthTiles[x][current.second - 1];
            if (tile != nullptr && tile->isVisible()) {
                return true;
            }
        }
        return false;
    }
    if (direction == left) {
        if (current.first - 1 < 0) {
            return true;
        }
        for (int y = current.second; y < current.second + 4; ++y) {
            if (y >= 60) {
                return false;
            }
            Earth* tile = earthTiles[current.first - 1][y];
            if (tile != nullptr && tile->isVisible()) {
                return true;
            }
        }
        return false;
    }
    if (direction == right) {
        if (current.first + 4 > 60) {
            return true;
        }
        for (int y = current.second; y < current.second + 4; ++y) {
            if (y >= 60) {
                return false;
            }
            Earth* tile = earthTiles[current.first + 4][y];
            if (tile != nullptr && tile->isVisible()) {
                return true;
            }
        }
        return false;
    }
    return true;
}

bool Protester::isEarthSwitch(std::pair<int, int> current, Direction direction) {
    const auto& earthTiles = this->world->m_earthTiles;
    if (direction == up) {
        if (current.first + 4 > 63) {
            return true;
        }
        if (current.first + 4 == 63) {
            return false;
        }
        for (int x = current.second; x < current.second + 4; ++x) {
            Earth* tile = earthTiles[x][current.first + 4];
            if (tile != nullptr && tile->isVisible()) {
                return true;
            }
        }
        return false;
    }
    if (direction == down) {
        if (current.first - 1 < 0) {
            return true;
        }
        if (current.first - 1 == 0) {
            return false;
        }
        for (int x = current.second; x < current.second + 4; ++x) {
            Earth* tile = earthTiles[x][current.first - 1];
            if (tile != nullptr && tile->isVisible()) {
                return true;
            }
        }
        return false;
    }
    if (direction == left) {
        if (current.second - 1 < 0) {
            return true;
        }
        if (current.second - 1 == 0) {
            return false;
        }
        for (int y = current.first; y < current.first + 4; ++y) {
            if (y >= 60) {
                return false;
            }
            Earth* tile = earthTiles[current.second - 1][y];
            if (tile != nullptr && tile->isVisible()) {
                return true;
            }
        }
        return false;
    }
    if (direction == right) {
        if (current.second + 4 > 63) {
            return true;
        }
        if (current.second + 4 == 63) {
            return false;
        }
        for (int y = current.first; y < current.first + 4; ++y) {
            if (y >= 60) {
                return false;
            }
            Earth* tile = earthTiles[current.second + 4][y];
            if (tile != nullptr && tile->isVisible()) {
                return true;
            }
        }
        return false;
    }
    return true;
}

bool Protester::canMove(std::pair<int, int> current, const Direction direction, const std::vector<std::vector<bool>>& visited) {
    std::unordered_set<int> invalidObjects = {TID_BOULDER};

    if (direction == up) {
        const std::pair<int, int> movePair = {current.first + 1, current.second};
        if (movePair.first > 60) {
            return false;
        }
        const bool notVisited = visited[movePair.first][movePair.second] == 0;
        return (notVisited && !this->isEarthSwitch(current, direction) && this->isValidMove(up, movePair, invalidObjects));
    }
    if (direction == down) {
        const std::pair<int, int> movePair = {current.first - 1, current.second};
        if (movePair.first < 0) {
            return false;
        }
        const bool notVisited = visited[movePair.first][movePair.second] == 0;
        return (notVisited && !this->isEarthSwitch(current, direction) && this->isValidMove(down, movePair, invalidObjects));
    }
    if (direction == left) {
        const std::pair<int, int> movePair = {current.first, current.second - 1};
        if (movePair.second < 0) {
            return false;
        }
        const bool notVisited = visited[movePair.first][movePair.second] == 0;
        return (notVisited && !this->isEarthSwitch(current, direction) && this->isValidMove(left, movePair, invalidObjects));
    }
    if (direction == right) {
        const std::pair<int, int> movePair = {current.first, current.second + 1};
        if (movePair.second > 60) {
            return false;
        }
        const bool notVisited = visited[movePair.first][movePair.second] == 0;
        return (notVisited && !this->isEarthSwitch(current, direction) && this->isValidMove(right, movePair, invalidObjects));
    }
    return false;
}

void Protester::findExitPath() {
    std::vector<std::vector<bool>> visited(MAX_ROW + 1, std::vector<bool>(MAX_COL + 1, 0));
    std::deque<Direction> movements;
    const std::pair<int, int> target = {60, 60};

    this->findNextMovement(visited, movements, {this->getY(), this->getX()}, target);
    this->m_exitPath = movements;
}

bool Protester::findNextMovement(std::vector<std::vector<bool>>& visited, std::deque<Direction>& movements, std::pair<int, int> current, const std::pair<int, int>& target) {
    // Condition if end is found
    if (current.first == target.first && current.second == target.second) {
        return true;
    }

    // Cache position
    visited[current.first][current.second] = 1;

    // Right check
    if (this->canMove(current, right, visited)) {
        movements.push_back(right);
        if (this->findNextMovement(visited, movements, {current.first, current.second + 1}, target)) {
            return true;
        }
    }

    // Up check
    if (this->canMove(current, up, visited)) {
        movements.push_back(up);
        if (this->findNextMovement(visited, movements, {current.first + 1, current.second}, target)) {
            return true;
        }
    }

    // Down check
    if (this->canMove(current, down, visited)) {
        movements.push_back(down);
        if (this->findNextMovement(visited, movements, {current.first - 1, current.second}, target)) {
            return true;
        }
    }

    // Left check
    if (this->canMove(current, left, visited)) {
        movements.push_back(left);
        if (this->findNextMovement(visited, movements, {current.first, current.second - 1}, target)) {
            return true;
        }
    }

    // Dead end
    movements.pop_back();
    return false;
}

bool Protester::facingTunnelMan() {
    const std::pair<int, int> tunnelManCoords = {this->tunnelMan->getX(), this->tunnelMan->getY()};
    const std::pair<int, int> protesterCoords = {this->getX(), this->getY()};
    const Direction direction = this->getDirection();

    const int xDifference = protesterCoords.first - tunnelManCoords.first;
    const int yDifference = protesterCoords.second - tunnelManCoords.second;

    // If the protester's y is within four pixels of the tunnelman's y they share at least one column
    if (std::abs(yDifference) <= 4) {
        // If the difference in x is >= 0, the protester must be to the right of tunnelman and vice versa
        return (xDifference >= 0 && direction == left) || (xDifference <= 0 && direction == right);
    }
    // If the protester's x is within four pixels of the tunnelman's y they share at least one row
    if (std::abs(xDifference) <= 4) {
        // If the difference in y is >= 0, the protester must be above tunnelman and vice versa
        return (yDifference >= 0 && direction == down) || (yDifference <= 0 && direction == up);
    }

    return false;
}

bool Protester::tunnelManLOS() {
    const std::pair<int, int> tunnelManCoords = {this->tunnelMan->getX(), this->tunnelMan->getY()};
    std::pair<int, int> protesterCoords = {this->getX(), this->getY()};
    const Direction direction = this->getDirection();

    const int xDifference = protesterCoords.first - tunnelManCoords.first;
    const int yDifference = protesterCoords.second - tunnelManCoords.second;

    // If protester and tunnelman share the same column
    if (std::abs(yDifference) <= 4) {
        // Logic if protester is right
        if (protesterCoords.first >= tunnelManCoords.first) {
            while (protesterCoords.first > tunnelManCoords.first) {
                if (this->isEarth(protesterCoords, left) || !this->isValidMove(left, protesterCoords, std::unordered_set<int>{TID_BOULDER})) {
                    return false;
                }
                --protesterCoords.first;
            }
            this->setDirection(left);
            return true;
        }

        // Logic if protester is left
        if (protesterCoords.first <= tunnelManCoords.first) {
            while (protesterCoords.first < tunnelManCoords.first) {
                if (this->isEarth(protesterCoords, right) || !this->isValidMove(right, protesterCoords, std::unordered_set<int>{TID_BOULDER})) {
                    return false;
                }
                ++protesterCoords.first;
            }
            this->setDirection(right);
            return true;
        }
    }

    // If protester and tunnelman share the same row
    if (std::abs(xDifference) <= 4) {
        // Logic if protester is above
        if (protesterCoords.second >= tunnelManCoords.second) {
            while (protesterCoords.second > tunnelManCoords.second) {
                if (this->isEarth(protesterCoords, down) || !this->isValidMove(down, protesterCoords, std::unordered_set<int>{TID_BOULDER})) {
                    return false;
                }
                --protesterCoords.second;
            }
            this->setDirection(down);
            return true;
        }

        // Logic if protester is below
        if (protesterCoords.second <= tunnelManCoords.second) {
            while (protesterCoords.second < tunnelManCoords.second) {
                if (this->isEarth(protesterCoords, up) || !this->isValidMove(up, protesterCoords, std::unordered_set<int>{TID_BOULDER})) {
                    return false;
                }
                ++protesterCoords.second;
            }
            this->setDirection(up);
            return true;
        }
    }

    return false;
}

bool Protester::movePerpendicular() {
    const std::pair<int, int> currentPosition = {this->getX(), this->getY()};
    const Direction currentDirection = this->getDirection();
    // If the protester is currently facing up or down, check for left and right
    if (currentDirection == up || currentDirection == down) {
        const bool canMoveLeft = this->isValidMove(left, currentPosition, std::unordered_set<int>{TID_BOULDER}) && !this->isEarth(currentPosition, left);
        const bool canMoveRight = this->isValidMove(right, currentPosition, std::unordered_set<int>{TID_BOULDER}) && !this->isEarth(currentPosition, right);

        // Cannot go perpendicular
        if (!canMoveLeft && !canMoveRight) {
            return false;
        }
        // If both are available then choose random direction
        // Else it has to be one of either
        if (canMoveLeft && canMoveRight) {
            const Direction newDirection = (rand() % 2 == 0) ? left : right;
            this->setDirection(newDirection);
        } else {
            this->setDirection((canMoveLeft) ? left : right);
        }
    } else if (currentDirection == left || currentDirection == right) {
        const bool canMoveUp = this->isValidMove(up, currentPosition, std::unordered_set<int>{TID_BOULDER}) && !this->isEarth(currentPosition, up);
        const bool canMoveDown = this->isValidMove(down, currentPosition, std::unordered_set<int>{TID_BOULDER}) && !this->isEarth(currentPosition, down);

        // Cannot go perpendicular
        if (!canMoveUp && !canMoveDown) {
            return false;
        }

        // If both are available then choose random direction
        // Else it has to be one of either
        if (canMoveUp && canMoveDown) {
            const Direction newDirection = (rand() % 2 == 0) ? up : down;
            this->setDirection(newDirection);
        } else {
            this->setDirection((canMoveUp) ? up : down);
        }
    }
    return true;
}

RegularProtester::RegularProtester(StudentWorld* world, TunnelMan* tunnelMan)
    : Protester(world, 5, TID_PROTESTER, tunnelMan) {
    this->m_hitPoints = 5;
    this->m_maxHealth = 5;
    this->m_alive = true;
    this->setVisible(true);
}

void RegularProtester::doSomething() {
    // If the protester is either dead or in a REST state + is on cooldown, exit early
    if (!this->isAlive() || (this->m_state == REST && !this->canPerformMovement())) {
        return;
    }

    // Set next movement cooldown tick
    this->setNextMovementTick();

    if (this->m_state == LEAVE_THE_OIL_FIELD) {
        const std::pair<int, int> position = {this->getX(), this->getY()};

        // Reached exit of oil field
        if (position.first == 60 && position.second == 60) {
            this->setAlive(false);
            this->setVisible(false);
            return;
        }

        // Check if an exit path hasn't been found
        if (!this->m_exitPath.has_value()) {
            this->findExitPath();
        }
        // Move to exit
        const Direction moveDirection = this->m_exitPath->front();
        this->setDirection(moveDirection);
        this->move(moveDirection);
        this->m_exitPath->pop_front();
        return;
    }

    // Check if tunnelman is within distance, facing tunnelman, and shout is not on cooldown
    const double tunnelManDistance = this->distance(this->getX(), this->getY(), this->tunnelMan->getX(), this->tunnelMan->getY());
    if (tunnelManDistance <= 4.0 && this->facingTunnelMan()) {
        if (this->world->m_gameTick <= this->m_lastShoutTick) {
            return;
        }
        this->world->playSound(SOUND_PROTESTER_YELL);
        this->tunnelMan->setHP(this->tunnelMan->getHP() - 2);
        this->m_lastShoutTick = this->world->m_gameTick + 15;
        return;
    }

    // If tunnelman is in line of sight and the distance is greater than 4.0 units, move towards tunnelman
    if (this->tunnelManLOS() && !this->isEarth({this->getX(), this->getY()}, this->getDirection()) && tunnelManDistance > 4.0) {
        this->move(this->getDirection());
        this->m_squaresToMove = 0;
        return;
    }

    // Check if perpendicular turn cooldown is up
    if (this->world->m_gameTick > this->m_lastPerpendicularTurn && this->movePerpendicular()) {
        // Reset squares to move
        this->m_squaresToMove = (rand() % 53) + 8;
        // Reset cooldown
        this->m_lastPerpendicularTurn = this->world->m_gameTick + 200;
    }

    // Set protester to a random *valid* direction if there are no more squares for it to move
    if (this->m_squaresToMove <= 0) {
        std::vector<Direction> directions{up, down, left, right};
        std::shuffle(directions.begin(), directions.end(), std::random_device());
        for (Direction& direction : directions) {
            if (isValidMove(direction, {this->getX(), this->getY()}, std::unordered_set<int>{TID_BOULDER}) && !isEarth({this->getX(), this->getY()}, direction)) {
                this->m_squaresToMove = (rand() % 53) + 8;
                this->setDirection(direction);
                return this->move(direction);
            }
        }
        return;
    }

    // Attempt to move
    const Direction& direction = this->getDirection();
    if (this->isValidMove(direction, {this->getX(), this->getY()}, std::unordered_set<int>{TID_BOULDER}) && !this->isEarth({this->getX(), this->getY()}, direction)) {
        return this->move(direction);
    }
    // If not valid move, find new direction in next tick
    this->m_squaresToMove = 0;
}

HardcoreProtester::HardcoreProtester(StudentWorld* world, TunnelMan* tunnelMan)
    : Protester(world, 20, TID_HARD_CORE_PROTESTER, tunnelMan) {
    this->m_hitPoints = 20;
    this->m_maxHealth = 20;
    this->m_alive = true;
    this->setVisible(true);
}

void HardcoreProtester::doSomething() {
    return;
}

Squirt::Squirt(StudentWorld* world, TunnelMan* tunnelMan, int startX, int startY, Direction direction)
    : m_tunnelMan(tunnelMan),
      Actor(world, TID_WATER_SPURT, startX, startY, direction, 1.0, 1) {
    const int x = this->getX();
    const int y = this->getY();
    switch (direction) {
        case up:
            this->m_maxDistance = {x, (y + 4 > MAX_ROW) ? MAX_ROW : y + 4};
            break;

        case down:
            this->m_maxDistance = {x, (y - 4 < 0) ? 0 : y - 4};
            break;

        case left:
            this->m_maxDistance = {(x - 4 < 0) ? 0 : x - 4, y};
            break;

        case right:
            this->m_maxDistance = {(x + 4 > MAX_COL) ? MAX_COL : x + 4, y};
            break;
    }

    this->m_alive = true;
    this->setVisible(true);
}

void Squirt::doSomething() {
    if (!this->isAlive()) {
        return;
    }

    // If squirt hits at leaster one protester, then return early
    bool hitProtester = false;
    for (Protester* protester : this->world->m_protesters) {
        const double distance = this->distance(this->getX(), this->getY(), protester->getX(), protester->getY());
        // If squirt touches protester and they are not already max annoyed
        if (distance <= 3.0 && protester->getState() != LEAVE_THE_OIL_FIELD) {
            protester->setHP(protester->getHP() - 2);
            protester->handleDamage();
            this->world->playSound(SOUND_PROTESTER_ANNOYED);
            if (protester->getID() == TID_PROTESTER) {
                this->world->increaseScore(100);
            } else if (protester->getID() == TID_HARD_CORE_PROTESTER) {
                this->world->increaseScore(250);
            }
            hitProtester = true;
        }
    }
    if (hitProtester) {
        this->setAlive(false);
        this->setVisible(false);
        return;
    }

    // Check if squirt traveled full distance or encounters earth/boulder
    if ((this->getX() == this->m_maxDistance.first && this->getY() == this->m_maxDistance.second) ||
        !this->isValidMove(this->getDirection(), {this->getX(), this->getY()}, std::unordered_set<int>{TID_BOULDER}) || !this->canMove()) {
        this->setAlive(false);
        this->setVisible(false);
        return;
    }

    this->move(this->getDirection());
}

bool Squirt::canMove() {
    std::pair<int, int> moveCoords = {this->getX(), this->getY()};
    switch (this->getDirection()) {
        case up:
            ++moveCoords.second;
            break;

        case down:
            --moveCoords.second;
            break;

        case left:
            --moveCoords.first;
            break;

        case right:
            ++moveCoords.first;
            break;
    }

    int maxY = moveCoords.second + 4 > MAX_ROW ? MAX_ROW : moveCoords.second + 4;
    for (int earthX = moveCoords.first; earthX < moveCoords.first + 4; ++earthX) {
        for (int earthY = moveCoords.second; earthY < maxY; ++earthY) {
            Earth* tile = this->world->m_earthTiles[earthX][earthY];
            if (tile != nullptr && tile->isVisible()) {
                return false;
            }
        }
    }
    return true;
}

Barrel::Barrel(StudentWorld* world, int startX, int startY, TunnelMan* tunnelMan)
    : tunnelMan(tunnelMan),
      Actor(world, TID_BARREL, startX, startY, right, 1.0, 2) {
    this->m_alive = true;
}

Barrel::~Barrel() {
}

void Barrel::doSomething() {
    if (!this->isAlive()) {
        return;
    }

    const std::pair<int, int> tunnelManCoords = {this->tunnelMan->getX(), this->tunnelMan->getY()};
    const double distance = this->distance(this->getX(), this->getY(), tunnelManCoords.first, tunnelManCoords.second);
    // If barrel is not visible and the distance between the barrel and TunnelMan is <= 4, become visible
    if (!this->isVisible() && distance <= 4.0) {
        this->setVisible(true);
        return;
    }

    // Barrel has been located
    if (distance <= 3.0) {
        --this->world->m_barrelsLeft;
        this->setAlive(false);
        this->world->playSound(SOUND_FOUND_OIL);
        this->world->increaseScore(1000);
    }
}

GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, TunnelMan* tunnelMan, bool visible, Pickupable pickupable, Lifespan lifespan)
    : tunnelMan(tunnelMan),
      m_visible(visible),
      m_pickupable(pickupable),
      m_lifespan(lifespan),
      Actor(world, TID_GOLD, startX, startY, right, 1.0, 2) {
    this->m_alive = true;
    if (lifespan == TEMPORARY) {
        this->m_expirationTick = this->world->m_gameTick + 100;
    }
    this->setVisible(visible);
}

void GoldNugget::doSomething() {
    if (!this->isAlive()) {
        return;
    }
    const std::pair<int, int> tunnelManCoords = {this->tunnelMan->getX(), this->tunnelMan->getY()};
    const double distance = this->distance(this->getX(), this->getY(), tunnelManCoords.first, tunnelManCoords.second);
    if (!this->isVisible() && distance <= 4.0) {
        this->m_visible = true;
        this->setVisible(this->m_visible);
        return;
    }

    if (this->m_pickupable == TUNNELMAN && distance <= 3.0) {
        // Give tunnelman a gold nugget
        this->tunnelMan->setGoldNuggets(this->tunnelMan->getGoldNuggets() + 1);
        this->setAlive(false);
        this->world->playSound(SOUND_GOT_GOODIE);
        this->world->increaseScore(10);
        return;
    }

    // Protester interaction with gold nugget
    if (this->m_pickupable == PROTESTER) {
        for (Protester* protester : this->world->m_protesters) {
            const double protesterDistance = this->distance(this->getX(), this->getY(), protester->getX(), protester->getY());
            // If protester isn't close enough to gold nugget, continue to next protester
            if (protesterDistance > 3) {
                continue;
            }

            this->setAlive(false);
            this->world->playSound(SOUND_PROTESTER_FOUND_GOLD);
            if (protester->getID() == TID_PROTESTER) {
                protester->setState(LEAVE_THE_OIL_FIELD);
                this->world->increaseScore(25);
            }
            if (protester->getID() == TID_HARD_CORE_PROTESTER) {
                // Hardcore protester is fixated for short duration
                protester->setMovementTick(this->world->m_gameTick + std::max(50, 100 - static_cast<int>(this->world->getLevel()) * 10));
                this->world->increaseScore(50);
            }
            break;  // Can only bribe one protester per gold at a time
        }
    }

    // Check if expired
    if (this->m_lifespan == TEMPORARY && this->world->m_gameTick > this->m_expirationTick) {
        this->setAlive(false);
        this->setVisible(false);
        return;
    }
}

Boulder::Boulder(StudentWorld* world, int startX, int startY, BoulderState state, TunnelMan* tunnelMan)
    : m_state(state),
      m_fallTick(std::nullopt),
      tunnelMan(tunnelMan),
      Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1) {
    this->m_alive = true;
    this->setVisible(true);
}

void Boulder::doSomething() {
    if (!this->isAlive()) {
        return;
    }

    if (this->m_state == STABLE) {
        // If the boulder can fall, set state to waiting for thirty ticks
        if (this->canFall()) {
            this->m_state = WAITING;
            this->m_fallTick = this->world->m_gameTick + 30;
        }
        return;
    }

    if (this->m_state == WAITING) {
        // If thirty game ticks have passed, start falling
        if (this->world->m_gameTick > this->m_fallTick) {
            this->world->playSound(SOUND_FALLING_ROCK);
            this->m_state = FALLING;
        }
        return;
    }

    if (this->m_state == FALLING) {
        // If the boulder can fall, fall
        if (this->canFall()) {
            return this->handleFall();
        }
        // Else make the boulder dead and not visible
        this->setAlive(false);
        this->setVisible(false);
    }
}

bool Boulder::canFall() {
    const std::pair<int, int> boulderCoords = {this->getX(), this->getY()};

    // Handle edgecase if boudler is at bottom of screen
    if (boulderCoords.second == 0) {
        return false;
    }

    // Check if the bottom four earth objects are missing
    const auto& earthTiles = this->world->m_earthTiles;
    const int earthY = boulderCoords.second - 1;
    for (int earthX = boulderCoords.first;
         earthX < boulderCoords.first + 4;
         ++earthX) {
        const Earth* tile = earthTiles[earthX][earthY];
        // If no (visible) earth tile below continue
        if (tile == nullptr || !tile->isVisible()) {
            continue;
        }
        // If visible earth tile below, cannot fall
        return false;
    }

    // Check if boulder can collide with another boulder
    for (Actor* actor : this->world->m_gameObjects) {
        if (actor->getID() == TID_BOULDER && earthY == actor->getY() && abs(actor->getX() - boulderCoords.first) < 4) {
            return false;
        }
    }

    // Boulder can fall
    return true;
}

void Boulder::handleFall() {
    this->move(down);

    const std::pair<int, int> boulderCoords = {this->getX(), this->getY()};

    // If the boulder is within distance of the tunnelman, die
    const auto& tunnelMan = this->tunnelMan;
    const double tunnelManDistance = this->distance(boulderCoords.first, boulderCoords.second, tunnelMan->getX(), tunnelMan->getY());
    if (tunnelManDistance <= 3) {
        tunnelMan->setHP(tunnelMan->getHP() - 100);
        tunnelMan->setAlive(false);
        return;
    }

    // If the boulder is within distance of a protester, annoy them
    const auto& protesters = this->world->m_protesters;
    for (Protester* protester : protesters) {
        const double protesterDistance = this->distance(boulderCoords.first, boulderCoords.second, protester->getX(), protester->getY());
        if (protesterDistance <= 3 && protester->getState() != LEAVE_THE_OIL_FIELD) {
            protester->setHP(protester->getHP() - 100);
            protester->handleDamage();
            this->world->increaseScore(500);
        }
    }
}

SonarKit::SonarKit(StudentWorld* world, int startX, int startY, TunnelMan* tunnelMan)
    : tunnelMan(tunnelMan),
      Actor(world, TID_SONAR, startX, startY, right, 1.0, 2) {
    this->m_experationTick = std::max(100, 300 - 10 * static_cast<int>(this->world->getLevel()));
    this->m_alive = true;
    this->setVisible(true);
}

void SonarKit::doSomething() {
    if (!this->isAlive()) {
        return;
    }

    // Check if sonar kit has expired
    if (this->world->m_gameTick > this->m_experationTick) {
        this->setAlive(false);
        this->setVisible(false);
        return;
    }

    // Check if tunnelman can pick up sonar kit
    const double distance = this->distance(this->getX(), this->getY(), this->tunnelMan->getX(), this->tunnelMan->getY());
    if (distance <= 3) {
        this->setAlive(false);
        this->world->playSound(SOUND_GOT_GOODIE);
        this->tunnelMan->setSonarCharges(this->tunnelMan->getSonarCharges() + 1);
        this->world->increaseScore(75);
    }
}

WaterPool::WaterPool(StudentWorld* world, int startX, int startY, TunnelMan* tunnelMan)
    : m_lifespan(TEMPORARY),
      tunnelMan(tunnelMan),
      Actor(world, TID_WATER_POOL, startX, startY, right, 1.0, 2) {
    this->m_experationTick = std::max(100, 300 - 10 * static_cast<int>(this->world->getLevel()));
    this->m_alive = true;
    this->setVisible(true);
}

void WaterPool::doSomething() {
    if (!this->isAlive()) {
        return;
    }

    // Check if tunnelman is on water pool
    const double tunnelManDistance = this->distance(this->getX(), this->getY(), this->tunnelMan->getX(), this->tunnelMan->getY());
    if (tunnelManDistance <= 3) {
        this->setAlive(false);
        this->setVisible(false);
        this->world->playSound(SOUND_GOT_GOODIE);
        this->tunnelMan->setWaterUnits(this->tunnelMan->getWaterUnits() + 5);
        this->world->increaseScore(100);
    }

    // Check if expired
    if (this->world->m_gameTick > this->m_experationTick) {
        this->setAlive(false);
        this->setVisible(false);
    }
}