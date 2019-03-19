#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>
#include <sstream>

class Actor;
class Penelope;
class Level;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath) :
		GameWorld(assetPath) {};

	virtual ~StudentWorld();

	void updateDisplay();
	std::string formatDisplay(int score, int level, int lives, int vaccines, int flames, int mines, int infected);

	virtual int init();
	virtual int move();
	void removeDeadGameObjects();
	virtual void cleanUp();

	std::vector<Actor*> getActors() { return m_actors; };
	Penelope* getPenelope() { return m_penelope; };
	void completed() { m_completed = true; };
	void addActor(Actor* actor);
	double euclideanDistance(double x1, double y1, double x2, double y2) const;
	bool isZombieVomitTriggerAt(double x, double y) const;
	bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance);
	bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const;
	bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const;
	bool noCitizens();

private:
	std::vector<Actor*> m_actors;
	Penelope* m_penelope;
	bool m_completed;
};

#endif // STUDENTWORLD_H_
