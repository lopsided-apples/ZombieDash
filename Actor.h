#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;
class Penelope;
class Goodie;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int imageID, double x, double y, int dir, int depth) :
		GraphObject(imageID, x, y, dir, depth)
	{
		m_isAlive = true;
		m_world = world;
	}

	virtual void doSomething() = 0;

	virtual bool isAlive() const { return m_isAlive; };
	virtual void setDead() { m_isAlive = false; };
	virtual StudentWorld* getWorld() const { return m_world; };

	virtual ~Actor() {};
	
	virtual bool canUseExit() const { return false; };
	virtual void useExitIfAppropriate() {};
	virtual bool canDieByBurn() const { return false; };
	virtual bool canDieByFallorBurn() const { return false; };
	virtual void dieByFallOrBurnIfAppropriate() {};
	virtual bool canBeInfected() const { return false; };
	virtual void beVomitedOnIfAppropriate() {};
	virtual void pickUpGoodieIfAppropriate(Goodie* g) {};
	virtual bool blocksMovement() const { return false; };
	virtual bool blocksFlame() const { return false; };
	virtual bool triggersOnlyActiveLandmines() const { return false; };
	virtual bool triggersZombieVomit() const { return false; };
	virtual bool threatensCitizens() const { return false; };
	virtual bool triggersCitizens() const { return false; };

private:
	bool m_isAlive;
	StudentWorld* m_world;
};

class Wall : public Actor
{
public:
	Wall(StudentWorld* world, double x, double y) :
		Actor(world, IID_WALL, x, y, right, 0) {};

	virtual void doSomething() {};
	virtual bool blocksMovement() const { return true; };
	virtual bool blocksFlame() const { return true; };
};

class ActivatingObject : public Actor
{
public:
	ActivatingObject(StudentWorld* world, int imageID, double x, double y, int dir, int depth) :
		Actor(world, imageID, x, y, dir, depth) {};
};

class Exit : public ActivatingObject
{
public:
	Exit(StudentWorld* world, double x, double y) :
		ActivatingObject(world, IID_EXIT, x, y, right, 1) {};

	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
	virtual bool blocksFlame() const { return true; };
};

class Pit : public ActivatingObject
{
public:
	Pit(StudentWorld* world, double x, double y) :
		ActivatingObject(world, IID_PIT, x, y, right, 0) {};

	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
};

class Flame : public ActivatingObject
{
public:
	Flame(StudentWorld* world, double x, double y, int dir) :
		ActivatingObject(world, IID_FLAME, x, y, dir, 0), m_ticks(0) {};

	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);

private:
	int m_ticks;
};

class Vomit : public ActivatingObject
{
public:
	Vomit(StudentWorld* world, double x, double y, int dir) :
		ActivatingObject(world, IID_VOMIT, x, y, dir, 0), m_ticks(0) {};

	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
	
private:
	int m_ticks;
};

class Landmine : public ActivatingObject
{
public:
	Landmine(StudentWorld* world, double x, double y) :
		ActivatingObject(world, IID_LANDMINE, x, y, right, 1), m_active(false), m_ticks(30), m_triggered(false) {};

	virtual void doSomething();
	virtual void activateIfAppropiate();
	virtual bool canDieByBurn() const { return true; };
	virtual void dieByFallOrBurnIfAppropriate() { m_triggered = true; };
	virtual bool isActive() { return m_active; };
private:
	bool m_active;
	int m_ticks;
	bool m_triggered;
};

class Goodie : public ActivatingObject
{
public:
	Goodie(StudentWorld* world, int imageID, double x, double y) :
		ActivatingObject(world, imageID, x, y, right, 1) {};

	virtual void doSomething();
	virtual void activateIfAppropriate(Penelope* p);
	virtual bool canDieByBurn() const { return true; };
	virtual void dieByFallOrBurnIfAppropriate() { setDead(); };
	virtual void pickUp(Penelope* p) = 0;
};

class VaccineGoodie : public Goodie
{
public:
	VaccineGoodie(StudentWorld* world, double x, double y) :
		Goodie(world, IID_VACCINE_GOODIE, x, y) {};

	virtual void pickUp(Penelope* p);
};

class GasCanGoodie : public Goodie
{
public:
	GasCanGoodie(StudentWorld* world, double x, double y) :
		Goodie(world, IID_GAS_CAN_GOODIE, x, y) {};

	virtual void pickUp(Penelope* p);
};

class LandmineGoodie : public Goodie
{
public:
	LandmineGoodie(StudentWorld* world, double x, double y) :
		Goodie(world, IID_LANDMINE_GOODIE, x, y) {};

	virtual void pickUp(Penelope* p);
};

class Agent : public Actor
{
public:
	Agent(StudentWorld* world, int imageID, double x, double y, int dir) :
		Actor(world, imageID, x, y, dir, 0) {};
	
	virtual bool canMove(double x, double y) const;
	virtual bool blocksMovement() const { return true; };
	virtual bool triggersOnlyActiveLandmines() const { return true; };
	virtual bool canDieByFallorBurn() const { return true; };
};

class Infectable : public Agent
{
public:
	Infectable(StudentWorld* world, int imageID, double x, double y) :
		Agent(world, imageID, x, y, right), m_infectionCount(0), m_infected(false) {};

	virtual bool canUseExit() const { return true; };
	virtual bool canBeInfected() const { return true; };
	virtual void beVomitedOnIfAppropriate() = 0;
	virtual bool triggersZombieVomit() const { return true; };

	virtual void clearInfection() { m_infected = false; m_infectionCount = 0; };
	virtual int getInfectionCount() { return m_infectionCount; };
	virtual void changeInfectionCount(int count) { m_infectionCount += count; };
	virtual void infect() { m_infected = true; };
	virtual bool getInfectionStatus() { return m_infected; };
private:
	int m_infectionCount;
	bool m_infected;
};

class Penelope : public Infectable
{
public:
	Penelope(StudentWorld* world, double x, double y) :
		Infectable(world, IID_PLAYER, x, y),
		m_vaccines(0), m_flamethrower(0), m_mines(0) {};

	virtual void doSomething();
	virtual bool canMove(double x, double y) const;
	virtual void useExitIfAppropriate();
	virtual void dieByFallOrBurnIfAppropriate();
	virtual void pickUpGoodieIfAppropriate(Goodie* g);
	virtual void beVomitedOnIfAppropriate() { infect(); }

	int getVaccines() { return m_vaccines; };
	int getFlames() { return m_flamethrower; };
	int getMines() { return m_mines; };
	void changeVaccines(int count) { m_vaccines = m_vaccines + count; };
	void changeFlames(int count) { m_flamethrower = m_flamethrower + count; };
	void changeMines(int count) { m_mines = m_mines + count; };
private:
	int m_vaccines;
	int m_flamethrower;
	int m_mines;
};

class Citizen : public Infectable
{
public:
	Citizen(StudentWorld* world, double x, double y) :
		Infectable(world, IID_CITIZEN, x, y), m_paralyzed(true) {};

	virtual void doSomething();
	virtual void useExitIfAppropriate();
	virtual void dieByFallOrBurnIfAppropriate();
	virtual void beVomitedOnIfAppropriate();

	virtual void paralyze() { m_paralyzed = !m_paralyzed; };
	virtual bool getParalysis() { return m_paralyzed; };
private:
	bool m_paralyzed;
};

class Zombie : public Agent
{
public:
	Zombie(StudentWorld* world, double x, double y) :
		Agent(world, IID_ZOMBIE, x, y, right), m_movementplan(0), m_paralyzed(true) {};

	virtual bool threatensCitizens() const { return true; };
	virtual bool triggersCitizens() const { return true; };
	virtual int getMovementPlan() { return m_movementplan; };
	virtual void setMovementPlan(int plan) { m_movementplan = plan; }
	virtual void decrementMovementPlan() { m_movementplan--; };
	virtual void paralyze() { m_paralyzed = !m_paralyzed; };
	virtual bool getParalysis() { return m_paralyzed; };
private:
	int m_movementplan;
	bool m_paralyzed;

};

class DumbZombie : public Zombie
{
public:
	DumbZombie(StudentWorld* world, double x, double y) :
		Zombie(world, x, y) {};

	virtual void doSomething();
	virtual void dieByFallOrBurnIfAppropriate();
};


class SmartZombie : public Zombie
{
public:
	SmartZombie(StudentWorld* world, double x, double y) :
		Zombie(world, x, y) {};

	virtual void doSomething();
	virtual void dieByFallOrBurnIfAppropriate();
};

#endif // ACTOR_H_
