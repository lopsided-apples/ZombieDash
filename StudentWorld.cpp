#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>


using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

void StudentWorld::updateDisplay()
{
	int score = getScore();
	int level = getLevel();
	int lives = getLives();
	int vaccines = getPenelope()->getVaccines();
	int flames = getPenelope()->getFlames();
	int mines = getPenelope()->getMines();
	int infected = getPenelope()->getInfectionCount();

	string display = formatDisplay(score, level, lives, vaccines, flames, mines, infected);
	setGameStatText(display);
}

string StudentWorld::formatDisplay(int score, int level, int lives, int vaccines, int flames, int mines, int infected)
{
	ostringstream oss;

	oss << "Score: ";
	if (score >= 0)
	{
		oss.fill('0');
		oss << setw(6) << score << "  ";
	}
	else
	{
		oss << "-";
		oss.fill('0');
		oss << setw(5) << -score << "  ";
	}
	oss << "Level: " << level << "  " 
	<< "Lives: " << lives << "  " 
	<< "Vaccines: " << vaccines << "  " 
	<< "Flames: " << flames << "  " 
	<< "Mines: " << mines << "  " 
	<< "Infected: " << infected;

	string display = oss.str();
	return display;
}

int StudentWorld::init()
{
	m_completed = false;
	ostringstream oss;
	oss << "level";
	oss.fill('0');
	oss << setw(2) << getLevel();
	oss << ".txt";
	string curLevel = oss.str();

	Level lev(assetPath());
	Level::LoadResult result = lev.loadLevel(curLevel);

	if (result == Level::load_fail_file_not_found || getLevel() > 100)
	{
		m_penelope = nullptr;
		return GWSTATUS_PLAYER_WON;
	}
	if (result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;

	for (int x = 0; x < VIEW_WIDTH; x++) 
	{
		for (int y = 0; y < VIEW_HEIGHT; y++) 
		{

			Level::MazeEntry item = lev.getContentsOf(x, y);
			double fixedX = x * SPRITE_WIDTH;
			double fixedY = y * SPRITE_HEIGHT;
			switch (item) 
			{
				case Level::empty:
					break;
				case Level::player:
					m_penelope = new Penelope(this, fixedX, fixedY);
					break;
				case Level::wall:
					m_actors.push_back(new Wall(this, fixedX, fixedY));
					break;
				case Level::exit:
					m_actors.push_back(new Exit(this, fixedX, fixedY));
					break;
				case Level::pit:
					m_actors.push_back(new Pit(this, fixedX, fixedY));
					break;
				case Level::vaccine_goodie:
					m_actors.push_back(new VaccineGoodie(this, fixedX, fixedY));
					break;
				case Level::gas_can_goodie:
					m_actors.push_back(new GasCanGoodie(this, fixedX, fixedY));
					break;
				case Level::landmine_goodie:
					m_actors.push_back(new LandmineGoodie(this, fixedX, fixedY));
					break;
				case Level::citizen:
					m_actors.push_back(new Citizen(this, fixedX, fixedY));
					break;
				case Level::dumb_zombie:
					m_actors.push_back(new DumbZombie(this, fixedX, fixedY));
					break;
				case Level::smart_zombie:
					m_actors.push_back(new SmartZombie(this, fixedX, fixedY));
					break;
			}
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	if (m_penelope->isAlive())
	{
		m_penelope->doSomething();
	}

	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->isAlive())
		{
			m_actors[i]->doSomething();
		}
	}

	if (!m_penelope->isAlive())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	if (m_completed) 
	{
		playSound(SOUND_LEVEL_FINISHED);
		return GWSTATUS_FINISHED_LEVEL;
	}

	removeDeadGameObjects();

	updateDisplay();

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::removeDeadGameObjects() 
{
	for (vector<Actor*>::iterator iter = m_actors.begin(); iter != m_actors.end(); )
	{
		if (!(*iter)->isAlive())
		{
			delete (*iter);
			iter = m_actors.erase(iter);
		}
		else { iter++; }
	}
}
void StudentWorld::cleanUp()
{
	delete m_penelope;

	for (vector<Actor*>::iterator iter = m_actors.begin(); iter != m_actors.end(); )
	{
		delete *iter;
		iter = m_actors.erase(iter);
	}
}

void StudentWorld::addActor(Actor* actor)
{
	m_actors.push_back(actor);
}

double StudentWorld::euclideanDistance(double x1, double y1, double x2, double y2) const
{
	return (pow(x1 - x2, 2) + pow(y2 - y1, 2));
}

bool StudentWorld::isZombieVomitTriggerAt(double x, double y) const
{
	for (int i = 0; i < m_actors.size(); i++)
	{
		if ((euclideanDistance(m_actors[i]->getX(), m_actors[i]->getY(), x, y) <= pow(10,2) && m_actors[i]->canBeInfected() && m_actors[i]->isAlive()) ||
			(euclideanDistance(m_penelope->getX(), m_penelope->getY(), x, y) <= pow(10, 2))) { return true; }
	}
	return false;
}

bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance)
{
	double temp = pow(VIEW_HEIGHT, 2);

	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->canBeInfected() && m_actors[i]->isAlive())
		{
			double cdistance = euclideanDistance(x, y, m_actors[i]->getX(), m_actors[i]->getY());
			if (cdistance < temp)
			{
				temp = cdistance;
				otherX = m_actors[i]->getX();
				otherY = m_actors[i]->getY();
			}
		}
	}
	double pdistance = euclideanDistance(x, y, m_penelope->getX(), m_penelope->getY());
	{
		if (pdistance < temp)
		{
			temp = pdistance;
			otherX = m_penelope->getX();
			otherY = m_penelope->getY();
		}
	}
	distance = temp;
	return true;
}

bool StudentWorld::locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const
{
	double temp = pow(VIEW_HEIGHT, 2);

	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->triggersCitizens() && m_actors[i]->isAlive())
		{
			double zdistance = euclideanDistance(x, y, m_actors[i]->getX(), m_actors[i]->getY());
			if (zdistance < temp)
			{
				temp = zdistance;
				otherX = m_actors[i]->getX();
				otherY = m_actors[i]->getY();
				isThreat = true;
			}
		}
	}
	double pdistance = euclideanDistance(x, y, m_penelope->getX(), m_penelope->getY());
	{
		if (pdistance < temp)
		{
			temp = pdistance;
			otherX = m_penelope->getX();
			otherY = m_penelope->getY();
			isThreat = false;
		}
	}
	distance = temp;
	return true;
}

bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const
{
	bool zombies = false;
	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->threatensCitizens() && m_actors[i]->isAlive()) { zombies = true; break; }
	}
	if (!zombies) { return false; }

	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->threatensCitizens() && m_actors[i]->isAlive())
		{
			double zdistance = euclideanDistance(x, y, m_actors[i]->getX(), m_actors[i]->getY());
			if (zdistance < distance)
			{
				distance = zdistance;
				otherX = m_actors[i]->getX();
				otherY = m_actors[i]->getY();
			}
		}
	}
	return true;
}

bool StudentWorld::noCitizens()
{
	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->canUseExit()) { return false; }
	}
	return true;
}