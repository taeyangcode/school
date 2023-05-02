#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "Actor.h"
#include "GameConstants.h"
#include "GameWorld.h"
#include "GraphObject.h"
#include "SoundFX.h"

// forward declarations
class Actor;
class TunnelMan;
class Earth;
class Protester;
class RegularProtester;
class HardcoreProtester;
class Squirt;
class Barrel;

class StudentWorld : public GameWorld {
   public:
    StudentWorld(std::string assetDir);

    virtual int init();

    virtual int move();

    virtual void cleanUp();

    // based off of init()
    // create the "Earth"
    void createEarth();

    // create oil, boulders, and gold
    void createObjects();

    // create specific amount of a specific object
    void createObject(int numObjects, int objectID);

    // create goodie
    void createGoodie();

    // logic related to creating protesters
    void spawnProtester();

    // specify what text is displayed at the top
    std::string getGameStatText();

    // destructor
    ~StudentWorld();

   private:
    TunnelMan* m_tunnelMan;

    int m_lastProtesterTick;

   public:
    int m_gameTick;

    std::vector<std::vector<Earth*> > m_earthTiles;

    std::vector<Actor*> m_gameObjects;

    std::vector<Protester*> m_protesters;

    int m_barrelsLeft;  // Keep track of how many oil barrels are left

    int m_gameScore;
};

#endif  // STUDENTWORLD_H_
