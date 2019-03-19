#include "Actor.h"
#include "StudentWorld.h"
#include <vector>

void Exit::doSomething()
{
	std::vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if ((getWorld()->euclideanDistance(actors[i]->getX(), actors[i]->getY(), getX(), getY()) <= pow(10, 2)) && actors[i]->canUseExit())
		{
			activateIfAppropriate(actors[i]);
		}
	}

	if (getWorld()->euclideanDistance(getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY(), getX(), getY()) <= pow(10, 2) && getWorld()->noCitizens())
	{
		activateIfAppropriate(getWorld()->getPenelope());
	}
	return;
}

void Exit::activateIfAppropriate(Actor* a)
{
	a->useExitIfAppropriate();
}

void Pit::doSomething()
{
	std::vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if (getWorld()->euclideanDistance(actors[i]->getX(), actors[i]->getY(), getX(), getY()) <= pow(10, 2) && actors[i]->canDieByFallorBurn())
		{
			activateIfAppropriate(actors[i]);
		}
	}

	if (getWorld()->euclideanDistance(getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY(), getX(), getY()) <= pow(10, 2))
	{
		activateIfAppropriate(getWorld()->getPenelope());
	}
}

void Pit::activateIfAppropriate(Actor* a)
{
	if (a->isAlive())
	a->dieByFallOrBurnIfAppropriate();
}

void Flame::doSomething()
{
	if (!isAlive()) return;

	m_ticks++;

	if (m_ticks == 2) { setDead(); return; }

	std::vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if (getWorld()->euclideanDistance(actors[i]->getX(), actors[i]->getY(), getX(), getY()) <= pow(10, 2) && (actors[i]->canDieByBurn() || actors[i]->canDieByFallorBurn()))
		{
			activateIfAppropriate(actors[i]);
		}
	}

	if (getWorld()->euclideanDistance(getX(), getY(), getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY()) <= pow(10, 2))
	{
		activateIfAppropriate(getWorld()->getPenelope());
	}

}

void Flame::activateIfAppropriate(Actor* a)
{
	if (a->isAlive())
	a->dieByFallOrBurnIfAppropriate();
}

void Vomit::doSomething()
{
	if (!isAlive()) return;

	m_ticks++;

	if (m_ticks == 2) { setDead(); return; }

	std::vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if (getWorld()->euclideanDistance(actors[i]->getX(), actors[i]->getY(), getX(), getY()) <= pow(10, 2) && actors[i]->canBeInfected())
		{
			activateIfAppropriate(actors[i]);
		}
	}

	if (getWorld()->euclideanDistance(getX(), getY(), getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY()) <= pow(10, 2))
	{
		activateIfAppropriate(getWorld()->getPenelope());
	}

}

void Vomit::activateIfAppropriate(Actor* a)
{
	a->beVomitedOnIfAppropriate();
}

void Landmine::doSomething()
{
	if (!isAlive()) { return; }

	if (!m_active)
	{
		m_ticks--;
		if (m_ticks == 0)
		{
			m_active = true;
			return;
		}
	}

	std::vector<Actor*> actors = getWorld()->getActors();

	for (int i = 0; i < actors.size(); i++)
	{
		if ((getWorld()->euclideanDistance(actors[i]->getX(), actors[i]->getY(), getX(), getY()) <= pow(10, 2) && actors[i]->triggersOnlyActiveLandmines() && m_active) ||
			(getWorld()->euclideanDistance(getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY(), getX(), getY()) <= pow(10, 2) && m_active))
		{
			m_triggered = true;
		}
	}

	if (m_triggered) { activateIfAppropiate(); }
}

void Landmine::activateIfAppropiate()
{
	setDead();
	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
	Actor* flame0 = new Flame(getWorld(), getX(), getY(), up);
	getWorld()->addActor(flame0);
	Actor* flame1 = new Flame(getWorld(), getX(), getY() + SPRITE_HEIGHT, up);
	getWorld()->addActor(flame1);
	Actor* flame2 = new Flame(getWorld(), getX() + SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up);
	getWorld()->addActor(flame2);
	Actor* flame3 = new Flame(getWorld(), getX() + SPRITE_WIDTH, getY(), up);
	getWorld()->addActor(flame3);
	Actor* flame4 = new Flame(getWorld(), getX() + SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up);
	getWorld()->addActor(flame4);
	Actor* flame5 = new Flame(getWorld(), getX(), getY() - SPRITE_HEIGHT, up);
	getWorld()->addActor(flame5);
	Actor* flame6 = new Flame(getWorld(), getX() - SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up);
	getWorld()->addActor(flame6);
	Actor* flame7 = new Flame(getWorld(), getX() - SPRITE_WIDTH, getY(), up);
	getWorld()->addActor(flame7);
	Actor* flame8 = new Flame(getWorld(), getX() - SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up);
	getWorld()->addActor(flame8);
	Actor* pit = new Pit(getWorld(), getX(), getY());
	getWorld()->addActor(pit);
}

void Goodie::activateIfAppropriate(Penelope* p)
{
	p->pickUpGoodieIfAppropriate(this);
	pickUp(p);
}

void Goodie::doSomething()
{
	if (!isAlive()) return;

	if (getWorld()->euclideanDistance(getWorld()->getPenelope()->getX(), getWorld()->getPenelope()->getY(), getX(), getY()) <= pow(10, 2))
		activateIfAppropriate(getWorld()->getPenelope());
};

void VaccineGoodie::pickUp(Penelope* p)
{
	p->changeVaccines(1);
}

void GasCanGoodie::pickUp(Penelope* p)
{
	p->changeFlames(5);
}

void LandmineGoodie::pickUp(Penelope* p)
{
	p->changeMines(2);
}

void Penelope::doSomething()
{
	if (!isAlive())
	{
		return;
	}
	if (getInfectionStatus())
	{
		changeInfectionCount(1);
		if (getInfectionCount() == 500)
		{
			setDead();
			getWorld()->playSound(SOUND_PLAYER_DIE);
			return;
		}
	}
	int key;
	if (getWorld()->getKey(key))
	{
		double x = getX();
		double y = getY();

		switch (key)
		{
		case KEY_PRESS_SPACE:
			if (m_flamethrower > 0)
			{
				m_flamethrower--;
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				double px;
				double py;
				bool flameBlocked;
				std::vector<Actor*> actors = getWorld()->getActors();
				for (int i = 1; i <= 3; i++)
				{
					flameBlocked = false;
					if (getDirection() == up)
					{
						px = getWorld()->getPenelope()->getX();
						py = getWorld()->getPenelope()->getY() + i * SPRITE_HEIGHT;
					}
					else if (getDirection() == down)
					{
						px = getWorld()->getPenelope()->getX();
						py = getWorld()->getPenelope()->getY() - i * SPRITE_HEIGHT;
					}
					else if (getDirection() == left)
					{
						px = getWorld()->getPenelope()->getX() - i * SPRITE_WIDTH;
						py = getWorld()->getPenelope()->getY();
					}
					else if (getDirection() == right)
					{
						px = getWorld()->getPenelope()->getX() + i * SPRITE_WIDTH;
						py = getWorld()->getPenelope()->getY();
					}
					for (int j = 0; j < actors.size(); j++)
					{
						if (getWorld()->euclideanDistance(actors[j]->getX(), actors[j]->getY(), px, py) <= pow(10, 2) && actors[j]->blocksFlame())
						{
							flameBlocked = true;
						}
					}
					if (flameBlocked) { break; }
					else
					{
						Actor* flame = new Flame(getWorld(), px, py, getDirection());
						getWorld()->addActor(flame);
					}
				}
			}
			break;
		case KEY_PRESS_TAB:
			if (m_mines > 0)
			{
				m_mines--;
				Actor* landmine = new Landmine(getWorld(), getX(), getY());
				getWorld()->addActor(landmine);
			}
			break;
		case KEY_PRESS_ENTER:
			if (m_vaccines > 0)
			{
				m_vaccines--;
				clearInfection();
			}
			break;
		case KEY_PRESS_LEFT:
			setDirection(left);
			if (canMove(x - 4, y)) {
				moveTo(x - 4, y);
			}
			break;
		case KEY_PRESS_RIGHT:
			setDirection(right);
			if (canMove(x + 4, y)) {
				moveTo(x + 4, y);
			}
			break;
		case KEY_PRESS_UP:
			setDirection(up);
			if (canMove(x, y + 4)) {
				moveTo(x, y + 4);
			}
			break;
		case KEY_PRESS_DOWN:
			setDirection(down);
			if (canMove(x, y - 4)) {
				moveTo(x, y - 4);
			}
			break;
		}

	}
}

bool Penelope::canMove(double x, double y) const
{
	if (x > VIEW_WIDTH || x < 0 || y > VIEW_HEIGHT || y < 0)
	{
		return false;
	}

	double rb = x + 7;
	double lb = x - 7;
	double ub = y + 7;
	double db = y - 7;

	std::vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		double ax = actors[i]->getX();
		double ay = actors[i]->getY();
		double arb = ax + 7;
		double alb = ax - 7;
		double aub = ay + 7;
		double adb = ay - 7;

		if ((rb >= alb && lb <= arb && ub >= adb && db <= aub) && this != actors[i] && actors[i]->blocksMovement())
		{
			return false;
		}
	}
	return true;
}

void Penelope::useExitIfAppropriate()
{
	getWorld()->completed();
}

void Penelope::dieByFallOrBurnIfAppropriate()
{
	setDead();
	getWorld()->playSound(SOUND_PLAYER_DIE);
}

void Penelope::pickUpGoodieIfAppropriate(Goodie* g)
{
	getWorld()->increaseScore(50);
	g->setDead();
	getWorld()->playSound(SOUND_GOT_GOODIE);
}

bool Agent::canMove(double x, double y) const
{
	if (x > VIEW_WIDTH || x < 0 || y > VIEW_HEIGHT || y < 0) { return false; }

	double rb = x + 7;
	double lb = x - 7;
	double ub = y + 7;
	double db = y - 7;

	double px = getWorld()->getPenelope()->getX();
	double py = getWorld()->getPenelope()->getY();
	double prb = px + 7;
	double plb = px - 7;
	double pub = py + 7;
	double pdb = py - 7;

	if (rb >= plb && lb <= prb && ub >= pdb && db <= pub)
	{
		return false;
	}

	std::vector<Actor*> actors = getWorld()->getActors();
	for (int i = 0; i < actors.size(); i++)
	{
		
		double ax = actors[i]->getX();
		double ay = actors[i]->getY();
		double arb = ax + 7;
		double alb = ax - 7;
		double aub = ay + 7;
		double adb = ay - 7;

		if ((rb >= alb && lb <= arb && ub >= adb && db <= aub) && this != actors[i] && actors[i]->blocksMovement())
		{
			return false;
		}
	}
	return true;
}

void Citizen::doSomething()
{
	if (!isAlive()) { return; }

	if (getInfectionStatus()) 
	{ 
		changeInfectionCount(1); 
		if (getInfectionCount() == 500)
		{
			setDead();
			getWorld()->playSound(SOUND_ZOMBIE_BORN);
			getWorld()->increaseScore(-1000);
			int random = randInt(0, 9);
			if (random < 7)
			{
				Actor* zombie = new DumbZombie(getWorld(), getX(), getY());
				getWorld()->addActor(zombie);
				return;
			}
			else
			{
				Actor* zombie = new SmartZombie(getWorld(), getX(), getY());
				getWorld()->addActor(zombie);
				return;
			}
			return;
		}
	}
	paralyze();
	if (getParalysis()) { return; }
	
	double x = getX();
	double y = getY();

	double tx;
	double ty;
	double dist_t = pow(VIEW_HEIGHT,2);
	bool threat;
	getWorld()->locateNearestCitizenTrigger(x, y, tx, ty, dist_t, threat);

	if (dist_t <= pow(80, 2) && !threat)
	{
		if (x == tx && y < ty && canMove(x, y + 2))
		{
			setDirection(up);
			moveTo(x, y + 2);
			return;
		}
				
		else if (x == tx && y > ty && canMove(x, y - 2))
		{
			setDirection(down);
			moveTo(x, y - 2);
			return;
		}
		else if (y == ty && x < tx && canMove(x + 2, y))
		{
			setDirection(right);
			moveTo(x + 2, y);
			return;
		}
		else if (y == ty && x > tx && canMove(x - 2, y))
			{
			setDirection(left);
			moveTo(x - 2, y);
			return;
		}

		if (x != tx && y != ty)
		{
			if (x < tx && y < ty)
			{
				int random = randInt(0, 1);
				if ((random == 0 && canMove(x, y + 2)) || (random == 1 && !canMove(x + 2, y) && canMove(x, y + 2)))
				{
					setDirection(up);
					moveTo(x, y + 2);
					return;
				}
				else if ((random == 1 && canMove(x + 2, y)) || (random == 0 && !canMove(x, y + 2) && canMove(x + 2, y)))
				{
					setDirection(right);
					moveTo(x + 2, y);
					return;
				}
			}
			else if (x < tx && y > ty)
			{
				int random = randInt(0, 1);
				if ((random == 0 && canMove(x, y - 2)) || (random == 1 && !canMove(x + 2, y) && canMove(x, y - 2)))
				{
					setDirection(down);
					moveTo(x, y - 2);
					return;
				}
				else if ((random == 1 && canMove(x + 2, y)) || (random == 0 && !canMove(x, y - 2) && canMove(x + 2, y)))
				{
					setDirection(right);
					moveTo(x + 2, y);
					return;
				}
			}
			if (x > tx && y > ty)
			{
				int random = randInt(0, 1);
				if ((random == 0 && canMove(x, y - 2)) || (random == 1 && !canMove(x - 2, y) && canMove(x, y - 2)))
				{
					setDirection(down);
					moveTo(x, y - 2);
					return;
				}
				else if ((random == 1 && canMove(x - 2, y)) || (random == 0 && !canMove(x, y - 2) && canMove(x - 2, y)))
				{
					setDirection(left);
					moveTo(x - 2, y);
					return;
				}
			}
			if (x > tx && y < ty)
			{
				int random = randInt(0, 1);
				if ((random == 0 && canMove(x, y + 2)) || (random == 1 && !canMove(x - 2, y) && canMove(x, y + 2)))
				{
					setDirection(up);
					moveTo(x, y + 2);
					return;
				}
				else if ((random == 1 && canMove(x - 2, y)) || (random == 0 && !canMove(x, y + 2) && canMove(x - 2, y)))
				{
					setDirection(left);
					moveTo(x - 2, y);
					return;
				}
			}
		}
	}

	if (dist_t <= pow(80, 2) && threat)
	{
		double nearestZombieUp = pow(VIEW_HEIGHT, 2);
		double nearestZombieDown = pow(VIEW_HEIGHT, 2);
		double nearestZombieRight = pow(VIEW_HEIGHT, 2);
		double nearestZombieLeft = pow(VIEW_HEIGHT, 2);

		if (canMove(x, y + 2)) { getWorld()->locateNearestCitizenThreat(x, y + 2, tx, ty, nearestZombieUp);	}
		else nearestZombieUp = 0;

		if (canMove(x, y - 2)) { getWorld()->locateNearestCitizenThreat(x, y - 2, tx, ty, nearestZombieDown); }
		else nearestZombieDown = 0;

		if (canMove(x + 2, y)) { getWorld()->locateNearestCitizenThreat(x + 2, y, tx, ty, nearestZombieRight); }
		else nearestZombieRight = 0;

		if (canMove(x - 2, y)) { getWorld()->locateNearestCitizenThreat(x - 2, y, tx, ty, nearestZombieLeft); }
		else nearestZombieLeft = 0;


		if (nearestZombieUp >= nearestZombieDown &&
			nearestZombieUp >= nearestZombieRight &&
			nearestZombieUp >= nearestZombieLeft &&
			nearestZombieUp >= dist_t)
		{
			setDirection(up);
			moveTo(x, y + 2);
			return;
		}
		if (nearestZombieDown >= nearestZombieUp &&
			nearestZombieDown >= nearestZombieRight &&
			nearestZombieDown >= nearestZombieLeft &&
			nearestZombieDown >= dist_t)
		{
			setDirection(down);
			moveTo(x, y - 2);
			return;
		}
		if (nearestZombieRight >= nearestZombieUp &&
			nearestZombieRight >= nearestZombieDown &&
			nearestZombieRight >= nearestZombieLeft &&
			nearestZombieRight >= dist_t)
		{
			setDirection(right);
			moveTo(x + 2, y);
			return;
		}
		if (nearestZombieLeft >= nearestZombieUp &&
			nearestZombieLeft >= nearestZombieDown &&
			nearestZombieLeft >= nearestZombieRight &&
			nearestZombieLeft >= dist_t)
		{
			setDirection(left);
			moveTo(x - 2, y);
			return;
		}
		else { return; }
	}
	return;
}

void Citizen::useExitIfAppropriate()
{
	getWorld()->increaseScore(500);
	setDead();
	getWorld()->playSound(SOUND_CITIZEN_SAVED);
}

void Citizen::dieByFallOrBurnIfAppropriate()
{
	if (isAlive())
	{
		setDead();
		getWorld()->playSound(SOUND_CITIZEN_DIE);
		getWorld()->increaseScore(-1000);
	}
}

void Citizen::beVomitedOnIfAppropriate()
{
	if (getInfectionStatus()) { return; }
	getWorld()->playSound(SOUND_CITIZEN_INFECTED);
	infect();
}

void DumbZombie::doSomething()
{
	if (!isAlive()) { return; }

	paralyze();
	if (getParalysis()) { return; }

	double x = getX();
	double y = getY();
	double px = getWorld()->getPenelope()->getX();
	double py = getWorld()->getPenelope()->getY();

	double vx = x;
	double vy = y;

	if (getMovementPlan() == 0)
	{
		int randomplan = randInt(3, 10);
		setMovementPlan(randomplan);

		int randomdir = randInt(0, 3);
		if (randomdir == 0) { setDirection(up); }
		else if (randomdir == 1) { setDirection(down); }
		else if (randomdir == 2) { setDirection(right); }
		else { setDirection(left); }
	}

	switch (getDirection())
	{
	case right:
		vx = x + SPRITE_WIDTH;
		vy = y;
		break;
	case left:
		vx = x - SPRITE_WIDTH;
		vy = y;
		break;
	case up:
		vx = x;
		vy = y + SPRITE_HEIGHT;
		break;
	case down:
		vx = x;
		vy = y - SPRITE_HEIGHT;
		break;
	}

	if (getWorld()->isZombieVomitTriggerAt(vx, vy))
	{
		std::vector<Actor*> actors = getWorld()->getActors();
		for (int i = 0; i < actors.size(); i++)
		{
			if (getWorld()->isZombieVomitTriggerAt(vx, vy))
			{
				if (getWorld()->euclideanDistance(vx, vy, actors[i]->getX(), actors[i]->getY()) <= pow(10, 2) && (actors[i]->canBeInfected()) ||
					getWorld()->euclideanDistance(vx, vy, px, py) <= pow(10, 2))
				{
					int random = randInt(0, 2);
					if (random == 1)
					{
						Actor* vomit = new Vomit(getWorld(), vx, vy, getDirection());
						getWorld()->addActor(vomit);
						getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
						setMovementPlan(0);
						return;
					}
				}
			}
		}
	}

	double dest_x = x;
	double dest_y = y;

	switch (getDirection())
	{
	case up:
		dest_x = x;
		dest_y = y + 1;
		break;
	case down:
		dest_x = x;
		dest_y = y - 1;
		break;
	case right:
		dest_x = x + 1;
		dest_y = y;
		break;
	case left:
		dest_x = x - 1;
		dest_y = y;
		break;
	}

	if (canMove(dest_x, dest_y))
	{
		moveTo(dest_x, dest_y);
		decrementMovementPlan();
	}
	else
	{
		setMovementPlan(0);
	}
	return;
}
void DumbZombie::dieByFallOrBurnIfAppropriate()
{
	if (isAlive())
	{
		setDead();
		getWorld()->playSound(SOUND_ZOMBIE_DIE);
		getWorld()->increaseScore(1000);
		int random = randInt(0, 9);
		if (random < 1)
		{
			double px;
			double py;
			int randomDirection = randInt(0, 3);
			switch (randomDirection)
			{
			case 0:	//right
				px = getX() + SPRITE_WIDTH;
				py = getY();
				break;
			case 1:	//left
				px = getX() - SPRITE_WIDTH;
				py = getY();
				break;
			case 2:	//up
				px = getX();
				py = getY() + SPRITE_HEIGHT;
				break;
			case 3:	//down
				px = getX();
				py = getY() - SPRITE_HEIGHT;
				break;
			}

			std::vector<Actor*> actors = getWorld()->getActors();

			for (int i = 0; i < actors.size(); i++)
			{
				if (getWorld()->euclideanDistance(actors[i]->getX(), actors[i]->getY(), px, py) <= pow(10, 2)) { return; }
			}
			Actor* vaccine = new VaccineGoodie(getWorld(), px, py);
			getWorld()->addActor(vaccine);
		}
	}
}

void SmartZombie::doSomething()
{
	if (!isAlive()) { return; }

	paralyze();
	if (getParalysis()) { return; }

	double x = getX();
	double y = getY();
	double px = getWorld()->getPenelope()->getX();
	double py = getWorld()->getPenelope()->getY();

	double vx = x;
	double vy = y;

	if (getMovementPlan() == 0)
	{
		int randomplan = randInt(3, 10);
		setMovementPlan(randomplan);

		double distance;
		double cx;
		double cy;
		if (getWorld()->locateNearestVomitTrigger(x, y, cx, cy, distance))
		{
			if (distance > pow(80, 2))
			{
				int randomdir = randInt(0, 3);
				if (randomdir == 0) { setDirection(up); }
				else if (randomdir == 1) { setDirection(down); }
				else if (randomdir == 2) { setDirection(right); }
				else { setDirection(left); }
			}

			else if (distance <= pow(80, 2))
			{
				if (x == cx && y < cy) { setDirection(up); }
				else if (x == cx && y > cy) { setDirection(down); }
				else if (x < cx && y == cy) { setDirection(right); }
				else if (x > cx && y == cy) { setDirection(left); }

				int random = randInt(0, 1);
				if (x < cx && y < cy)
				{
					if (random == 0) { setDirection(right); }
					else { setDirection(up); }
				}
				if (x > cx && y < cy)
				{
					if (random == 0) { setDirection(left); }
					else { setDirection(up); }
				}
				if (x > cx && y > cy)
				{
					if (random == 0) { setDirection(left); }
					else { setDirection(down); }
				}
				if (x < cx && y > cy)
				{
					if (random == 0) { setDirection(right); }
					else { setDirection(down); }
				}
			}
		}
	}

	switch (getDirection())
	{
	case right:
		vx = x + SPRITE_WIDTH;
		vy = y;
		break;
	case left:
		vx = x - SPRITE_WIDTH;
		vy = y;
		break;
	case up:
		vx = x;
		vy = y + SPRITE_HEIGHT;
		break;
	case down:
		vx = x;
		vy = y - SPRITE_HEIGHT;
		break;
	}

	if (getWorld()->isZombieVomitTriggerAt(vx, vy))
	{
		std::vector<Actor*> actors = getWorld()->getActors();
		for (int i = 0; i < actors.size(); i++)
		{
			if (getWorld()->isZombieVomitTriggerAt(vx, vy))
			{
				if (getWorld()->euclideanDistance(vx, vy, actors[i]->getX(), actors[i]->getY()) <= pow(10, 2) && (actors[i]->canBeInfected()) ||
					getWorld()->euclideanDistance(vx, vy, px, py) <= pow(10, 2))
				{
					int random = randInt(0, 2);
					if (random == 1)
					{
						Actor* vomit = new Vomit(getWorld(), vx, vy, getDirection());
						getWorld()->addActor(vomit);
						getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
						setMovementPlan(0);
						return;
					}
				}
			}
		}
	}

	double dest_x = x;
	double dest_y = y;

	switch (getDirection())
	{
	case up:
		dest_x = x;
		dest_y = y + 1;
		break;
	case down:
		dest_x = x;
		dest_y = y - 1;
		break;
	case right:
		dest_x = x + 1;
		dest_y = y;
		break;
	case left:
		dest_x = x - 1;
		dest_y = y;
		break;
	}

	if (canMove(dest_x, dest_y))
	{
		moveTo(dest_x, dest_y);
		decrementMovementPlan();
	}
	else
	{
		setMovementPlan(0);
	}
}

void SmartZombie::dieByFallOrBurnIfAppropriate()
{
	if (isAlive())
	{
		setDead();
		getWorld()->playSound(SOUND_ZOMBIE_DIE);
		getWorld()->increaseScore(2000);
	}
}


