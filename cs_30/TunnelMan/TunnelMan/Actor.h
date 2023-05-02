#ifndef ACTOR_H_
#define ACTOR_H_

#include <deque>
#include <optional>
#include <unordered_set>

#include "GameConstants.h"
#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;

class Actor;
class TunnelMan;
class Earth;
class Protester;
class RegularProtester;
class HardcoreProtester;
class Squirt;
class Barrel;
class GoldNugget;
class Boulder;
class SonarKit;
class WaterPool;

enum Pickupable {
    TUNNELMAN,
    PROTESTER
};

enum Lifespan {
    PERMANENT,
    TEMPORARY
};

enum BoulderState {
    STABLE,
    WAITING,
    FALLING
};

enum ProtesterState {
    REST,
    LEAVE_THE_OIL_FIELD
};

// 64x64 for earth given in GameConstant.h
// general object size constant, object specific given through ID

// "all of the Earth that comprises the 64 -square wide by 60-square high oil field"
const int EARTH_MAX_X = 64;
const int EARTH_MAX_Y = 60;

const int OBJECT_SIZE = 4;
const double DEFUALT_SIZE = 1.0;
const int GOLD_TIME = 100;

// tunnelMan consts and contents of oil field
const int START_COL = 30;
const int END_COL = 33;
const int START_ROW = 4;
const int END_ROW = 59;
const int TUNNELMAN_COL = 30;
const int TUNNELMAN_ROW = 60;
const int TUNNELMAN_HEALTH = 100;

// "The oil field, which is comprised of 1x1 Earth objects, occupies the entire width of the
// game window, but only the lower 60 squares of the screen."
const int MAX_COL = 60;
const int MAX_ROW = 60;

// counter/scores for different objects
const int OIL_SCORE = 1000;
const int WATER_SCORE = 100;
const int GOLD_ACQ_SCORE = 10;

/* Actors include the Tunnelman, Regular Protesters,
 * Hardcore Protesters, Boulders, Gold Nuggets, Barrels of oil, Water, Squirts from the
 * Tunnelman’s squirt gun, and Sonar KitsActor */
class Actor : public GraphObject {
   protected:
    StudentWorld* world;
    std::optional<bool> m_alive;
    std::optional<int> m_hitPoints;
    std::optional<int> m_maxHealth;
    std::optional<int> m_annoyance;

   public:
    /* The reason we pass a StudentWorld argument in Actor is because Actor children classes
     * require the knowledge of what key is being pressed, and the logic for that exists
     * only on StudentWorld and GameWorld objects */
    Actor(StudentWorld* world, int imageID, int startX, int startY, Direction direction, double size, unsigned depth);

    ~Actor();

    virtual void doSomething() = 0;

    // Finds whether or not a valid key was pressed; where if so, sets keyStore to the respective key's value
    bool getKeyPress(int& keyStore);

    // Finds if the argument key is a movement key
    bool isMovementKey(const int key);

    // Converts an integer key to a Direction enum (usage mostly revolves around easier readability/understanding)
    GraphObject::Direction toDirection(const int key);

    // Finds if moving in the given direction is valid (will not result in out of bounds position)
    bool isValidMove(const Direction& direction, std::pair<int, int> moveCoords, std::unordered_set<int> invalidObjects);

    // Helper function to move Actor in direction (doesn't check for out of bounds/invalid movement!)
    void move(const Direction direction);

    // Helper function to find distance between coordinates
    double distance(int x1, int y1, int x2, int y2);

    // Alive Status getter / setter
    bool isAlive();
    void setAlive(bool alive);

    // Annoyance getter / setter
    int getAnnoyance();
    void setAnnoyance(int annoyance);

    // Health Points getter / setter
    int getHP();
    void setHP(int hp);

    // Max HP getter
    int getMaxHP();
};

class TunnelMan : public Actor {
   private:
    std::vector<std::vector<Earth*>>& m_earthTiles;

    int m_waterUnits;
    int m_sonarCharges;
    int m_goldNuggets;

    // Helper function to handle movement of TunnelMan in the supplied direction (with bounds checking!)
    void handleMovement(const Direction moveDirection);

   public:
    TunnelMan(StudentWorld* world, std::vector<std::vector<Earth*>>& earthTiles);

    ~TunnelMan();

    void mineEarth();

    virtual void doSomething();

    // Water getter / setter
    int getWaterUnits();
    void setWaterUnits(int waterUnits);

    // Sonar getter / setter
    int getSonarCharges();
    void setSonarCharges(int sonarCharges);

    // Gold Nugget getter / setter
    int getGoldNuggets();
    void setGoldNuggets(int goldNuggets);

    // Squirt handler
    void handleSquirt();

    // Sonar handler
    void handleSonar();

    // Gold nugget handler
    void handleGoldNugget();
};

class Earth : public Actor {
   public:
    // Position of Earth object must be supplied
    Earth(StudentWorld* world, int startX, int startY);

    ~Earth();

    // Earth doesn't do anything pog
    virtual void doSomething();
};

class Protester : public Actor {
   protected:
    TunnelMan* tunnelMan;

    int m_squaresToMove;

    int m_hitPoints;
    bool m_leaveOilField;

    int m_restTickAmount;
    int m_movementTick;

    int m_lastShoutTick;

    int m_lastPerpendicularTurn;

    ProtesterState m_state;

    std::optional<std::deque<GraphObject::Direction>> m_exitPath;

   public:
    Protester(StudentWorld* world, int hitPoints, int imageID, TunnelMan* tunnelMan);

    virtual void doSomething() = 0;

    // State getter / setter
    ProtesterState getState();
    void setState(ProtesterState state);

    // Movement tick getter / setter
    int getMovementTick();
    void setMovementTick(int movementTick);

    // Helper function to handle when the protester is damaged
    void handleDamage();

   protected:
    // Helper function that returns whether or not the protester is capable of performing a movement command
    bool canPerformMovement();

    // Helper function to calculate when the protester can perform a movement command again
    void setNextMovementTick();

    // Helper function to see if earth exists in the movement direction
    bool isEarth(std::pair<int, int> current, GraphObject::Direction direction);

    // Helper function to see if earth exists in the movement direction (modified for switched coordinates)
    bool isEarthSwitch(std::pair<int, int> current, GraphObject::Direction direction);

    // Helper function to see if protester can move in direction (utilized in shortest path algorithm)
    bool canMove(std::pair<int, int> current, const GraphObject::Direction direction, const std::vector<std::vector<bool>>& visited);

    // Helper function to find shortest path to exit
    void findExitPath();

    // Recursive helper function to help find exit path
    bool findNextMovement(std::vector<std::vector<bool>>& visited, std::deque<Direction>& movements, std::pair<int, int> current, const std::pair<int, int>& target);

    // Helper function to check if protester is looking at TunnelMan
    bool facingTunnelMan();

    // Helper function to check if tunnelman is in the protester's line of sight without object obstruction
    bool tunnelManLOS();

    // Helper function to determine perpendicular movement
    bool movePerpendicular();
};

class RegularProtester : public Protester {
   public:
    RegularProtester(StudentWorld* world, TunnelMan* tunnelMan);

    ~RegularProtester() = default;

    virtual void doSomething();
};

class HardcoreProtester : public Protester {
   public:
    HardcoreProtester(StudentWorld* world, TunnelMan* tunnelMan);

    ~HardcoreProtester() = default;

    virtual void doSomething();
};

class Squirt : public Actor {
   private:
    TunnelMan* m_tunnelMan;
    std::pair<int, int> m_maxDistance;

   public:
    Squirt(StudentWorld* world, TunnelMan* tunnelMan, int startX, int startY, Direction direction);

    virtual void doSomething();

    // Checks to see if squirt can move in direction
    bool canMove();
};

class Barrel : public Actor {
   private:
    TunnelMan* tunnelMan;

   public:
    Barrel(StudentWorld* world, int startX, int startY, TunnelMan* tunnelMan);

    ~Barrel();

    virtual void doSomething();
};

class GoldNugget : public Actor {
   private:
    TunnelMan* tunnelMan;
    bool m_visible;
    Pickupable m_pickupable;
    Lifespan m_lifespan;
    std::optional<int> m_expirationTick;

   public:
    GoldNugget(StudentWorld* world, int startX, int startY, TunnelMan* tunnelMan, bool visible, Pickupable pickupable, Lifespan lifespan);

    virtual void doSomething();
};

class Boulder : public Actor {
   private:
    BoulderState m_state;
    std::optional<int> m_fallTick;
    TunnelMan* tunnelMan;

   public:
    Boulder(StudentWorld* world, int startX, int startY, BoulderState state, TunnelMan* tunnelMan);

    virtual void doSomething();

    // Check if earth is below
    bool canFall();

    // Handle boulder falling logic
    void handleFall();
};

class SonarKit : public Actor {
   private:
    TunnelMan* tunnelMan;
    int m_experationTick;

   public:
    SonarKit(StudentWorld* world, int startX, int startY, TunnelMan* tunnelMan);

    virtual void doSomething();
};

class WaterPool : public Actor {
   private:
    TunnelMan* tunnelMan;
    Lifespan m_lifespan;
    int m_experationTick;

   public:
    WaterPool(StudentWorld* world, int startX, int startY, TunnelMan* tunnelMan);

    virtual void doSomething();
};

#endif
