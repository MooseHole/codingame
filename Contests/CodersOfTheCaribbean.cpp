/*
0074224023042017: 176/ 354:Go Try going just a little bit ahead of evading enemy ships.
0073220023042017: 184/ 353:Go Try going ahead of evading enemy ships.
0072210023042017: 111/ 345:Go Allow some default actions while evading or pathing.
0071135023042017: 135/ 337:Go Evade when not using path.
0070133523042017: 144/ 332:Go Use a mobile heading when not using path.
0069131523042017: 188/ 331:Go Use neighbor speed for path transitions.
0068005023042017: 233/ 318:Go Choose omega path based on blood honey and whales.
0067220522042017: 249/ 310:Go Better check of path transition due to current speed.
0066220522042017: 202/ 305:Go Check neighbor for fscore instead of current.
0065192522042017: 215/ 302:Go Attempt to shoot if no change on path.
0064191522042017: 222/ 301:Go Corrected spelling of swooty.
0063191022042017: 222/ 300:Go Revert to revision 0061.
0062172022042017: 236/ 300:Go Don't shoot the same place twice.
0061152522042017: 188/ 286:Go If on path runs out, use shootmove.
0060150022042017: 184/ 286:Go Copy path for simulation.
0059144022042017: 242/ 286:Go Switch the "mine" move to "wait".
0058140522042017: 238/ 285:Go Incorporate pathfinding into simulations.
0057002422042017: 265/ 275:Go Try out some pathfinding.
0056094521042017: 240/ 255:Go My ships with less rum (non-honeyships) have rum worth more.
0055233520042017: 237/ 257:Go Copy for Gold level.
0054230520042017:   1/ 715:Si Speed up distance finder using cubic coordinates.
0053213520042017:  51/ 718:Si Chase blood every turn instead of just initial.
0052200020042017:  83/ 717:Si Aggressively hunt for blood.
0051181520042017:  80/ 716:Si Score rum collected instead of barrel distance.  Lower deadline from 0.035 to 0.030
0050175020042017:  87/ 717:Si Assume enemy will FIRE or WAIT instead of move forward and drop mines.
0049164520042017:  97/ 707:Si Smell blood.
0048163020042017:  97/ 707:Si Fix cannon aiming.
0047122020042017:  90/ 703:Si Don't simulate past the last round.
0046120020042017: 210/ 701:Si Check all positions for collisions.
0045112520042017:  90/ 700:Si Fix speed simulation bug.
0044110020042017: 690/ 701:Si Simulate collisions better.  Decay future scores better.  Increase own rum score.
0043005520042017: 113/ 648:Si Simulate speed to 0 when hit edge.
0042003520042017:  84/ 648:Si Actually revert to revision 0039.
0041170019042017: 386/ 649:Si Revert to revision 0039.  Woops still have random?
0040165519042017: 659/ 674:Si Try random operations.
0039155519042017: 143/ 671:Si Use distance to nearest my ship to score formation.
0038153019042017: 101/ 675:Si Remove formation heading.  Increase barrel distance and formation1 penalties.
0037123519042017: 173/ 676:Si Increase formation search radius from 6 to 15.
0036121519042017: 142/ 676:Si Simulate mine blast ring.
0035113819042017: 220/ 677:Si Only destroy mines when at speed 1.
0034113819042017: 291/ 677:Si Allow destruction of mines.  Limit iterations to 25 and decay evenly.
0033103519042017: 191/ 676:Si Detect when able to place mines.  Simulate new barrel when ship dies.
0032101519042017: 185/ 790:Si Aim for the closest ship.
0031013019042017: 330/ 734:Si Aim cannons at bow and stern as well.
0030010019042017: 340/ 734:Si Decay scoring with each iteration.
0029003519042017: 403/ 732:Si Restore initial move scoring.
0028001519042017: 726/ 732:Si Refactor trials so max can be run.
0027000517042017: 317/ 576:Si Add high scores for win/loss.
0026205517042017: 335/ 561:Si Improve ship collision detection.
0025162017042017: 373/ 540:Si Keep better track of refiring during simulations.
0024155517042017: 315/ 538:Si Add output comments.
0023151017042017: 430/ 533:Si Search farther for barrels.
0022141517042017: 280/ 514:Si Copy for Silver level.
0021141517042017:   2/ 481:Br Do not penalize barrel distance if a barrel has been collected.
0020133017042017:   4/ 483:Br Corrected simulation timeout.  Score higher for closer to barrels.  Score much lower for edging.
0019113017042017:  11/ 478:Br Use FASTER instead of MINE in trials.  Mess with formation scores.  Stop simulation on timeout.
0018113017042017:   8/ 478:Br Fix a bunch of bugs related to non-updating.
0017010017042017: 840/ 872:Br Reduce score for on edge and begin formation scoring.
0016003617042017: 855/ 872:Br Improve distance and cannon calculations.
0015001017042017: 822/ 872:Br Simulate each of faster/edgeturn/fire, port, starboard, mine.
0014222316042017: 869/ 871:Br Simulation with random moves and score by rum.
0013185015042017: 260/ 609:Br Begin Grid implementation.
0012162515042017: 141/ 550:Br Better cannon aiming.
0011161015042017: 382/ 549:Br Keep track of cannon and mine cooldown.
0009155515042017:   1/ 247:W1 Find closest barrel only.  Setup for blockers but too slow.
0008120715042017: 205/ 244:W1 Limit find distance to 10 to speed up processing.
0006014015042017:   1/1073:W2 Shoot ships when they are stopped.
0005013015042017: 109/1073:W2 Shoot ships incidentally.
0004005015042017: 410/1069:W2 Find distance by BFS.  Cannonball mines in my way.
0003132514042017: 268/ 576:W2 Fix bad locations.  Generalize and derive entities.
0001121114042017:   2/ 106:W3 Initial submission
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>

using namespace std;

#define GRID_WIDTH  23
#define GRID_HEIGHT 21
#define LAST_ROUND 200

#define MOVE_BITS	2

#define SIMULATION_DEADLINE 0.030
#define MAX_ITERATIONS      25
#define FORMATION_SEARCH    15 
#define FORMATION_MAX       5

#define SCORE_WIN               1000000
#define SCORE_LOSE              -1000000
#define SCORE_SHIP				1000
#define SCORE_RUM				100
#define SCORE_HONEY_RUM			1000
#define SCORE_EDGE				0//-1
#define SCORE_HARD_EDGE			0//-3
#define SCORE_ENEMY_SHIP		0//-1
#define SCORE_ENEMY_RUM			-10
#define SCORE_FORMATION_1		-100
#define SCORE_FORMATION_2		-5
#define SCORE_FORMATION_FAR		-6
#define SCORE_FORMATION_HEADING	0
#define SCORE_BARREL_DISTANCE	-10
#define SCORE_STOPPED			-1
#define SCORE_REMAINING_BARREL	0
#define SCORE_FAR_FROM_BLOOD	-10
#define SCORE_RUM_COLLECTED		10

static minstd_rand randomEngine(clock());
static uniform_int_distribution<int> randomOperation{0, 5};
static uniform_int_distribution<int> randomBinary{0, 1};

class Location
{
	private:
	int id;

	public:
	int x;
	int y;

	Location()
	{
		x = 0;
		y = 0;
		id = 10000;
	}

	Location(int _x, int _y)
	{
		x = _x;
		y = _y;
		id = 10000 + x * 100 + y;
	}

	Location& operator=(Location other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

    bool operator==(const Location& other) const
    {
        return x == other.x && y == other.y;
    }
	
    bool operator!=(const Location& other) const
    {
        return !(*this == other);
    }

    bool operator<(const Location& other) const
    {
        return id < other.id;
    }

	Location neighborHeading(int heading) const
	{
		Location neighbor = Location(x, y);
		bool even = y % 2 == 0;
		switch(heading)
		{
			case 0:
				if (         x < GRID_WIDTH - 1                       ) neighbor = Location(x+1, y  );
				break;
			case 1:
				if ( even &&                       y > 0              ) neighbor = Location(x  , y-1);
				if (!even && x < GRID_WIDTH - 1 && y > 0              ) neighbor = Location(x+1, y-1);
				break;
			case 2:
				if ( even && x > 0 &&              y > 0              ) neighbor = Location(x-1, y-1);
				if (!even &&                       y > 0              ) neighbor = Location(x  , y-1);
				break;
			case 3:
				if (         x > 0                                    ) neighbor = Location(x-1, y  );
				break;
			case 4:
				if ( even && x > 0 &&              y < GRID_HEIGHT - 1) neighbor = Location(x-1, y+1);
				if (!even &&                       y < GRID_HEIGHT - 1) neighbor = Location(x  , y+1);
				break;
			case 5:
				if ( even &&                       y < GRID_HEIGHT - 1) neighbor = Location(x  , y+1);
				if (!even && x < GRID_WIDTH - 1 && y < GRID_HEIGHT - 1) neighbor = Location(x+1, y+1);
				break;
			default:
				break;
		}
		
		return neighbor;
	}

	vector<Location> neighbors() const
	{
		vector<Location> result;

		// If even
		bool even = y % 2 == 0;
		if ( even && x > 0 &&              y > 0              ) result.push_back(Location(x-1, y-1));
		if ( even && x > 0 &&              y < GRID_HEIGHT - 1) result.push_back(Location(x-1, y+1));
		if (!even && x < GRID_WIDTH - 1 && y < 0              ) result.push_back(Location(x+1, y-1));
		if (!even && x < GRID_WIDTH - 1 && y < GRID_HEIGHT - 1) result.push_back(Location(x+1, y+1));
		if (                               y > 0              ) result.push_back(Location(x  , y-1));
		if (         x < GRID_WIDTH - 1                       ) result.push_back(Location(x+1, y  ));
		if (                               y < GRID_HEIGHT - 1) result.push_back(Location(x  , y+1));
		if (         x > 0                                    ) result.push_back(Location(x-1, y  ));
		
		return result;
	}

	string print() const
	{
		return "(" + to_string(x) + "," + to_string(y) + ")";
	}
	
	friend ostream &operator<<(ostream &os, Location const &m)
	{
		return os << m.print();
	}
};

class Move
{
	public:
	int entityId;
	string action;
	Location target;
	string comment;
	
	Move()
	{
		entityId = -1;
		action = "WAIT";
		target = Location();
		comment = "";
	}
	
	Move(int _entityId, string _action, string _comment)
	{
		entityId = _entityId;
		action = _action;
		target = Location();
		comment = _comment;
	}
	
	Move(int _entityId, string _action, Location _target = Location(), string _comment = "")
	{
		entityId = _entityId;
		action = _action;
		target = _target;
		comment = _comment;
	}

	Move& operator=(const Move other)
	{
		entityId = other.entityId;
		action = other.action;
		target = other.target;
		comment = other.comment;
	}

	string print() const
	{
		return "{" + to_string(entityId) + " " + action + " " + target.print() + " " + comment + "}";
	}
	
	friend ostream &operator<<(ostream &os, Move const &m)
	{
		return os << m.print();
	}
};

class Entity
{
	public:
	int id;
	Location location;
	bool updated;
	string type;
	int arg1, arg2, arg3, arg4;

	Entity()
	{
		id = -1;
		type = "ENTITY";
	}

	Entity(int _id, int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		id = _id;
		type = "ENTITY";
		update(_x, _y, _arg1, _arg2, _arg3, _arg4);
	}
	
	Entity& operator=(const Entity other)
	{
		id = other.id;
		location = other.location;
		updated = other.updated;
		type = other.type;
		return *this;
	}

	virtual void update(int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		location.x = _x;
		location.y = _y;
		arg1 = _arg1;
		arg2 = _arg2;
		arg3 = _arg3;
		arg4 = _arg4;
		updated = true;
	}
	
	virtual bool blocking(Location target, int atTurn = 0) const
	{
		return false;
	}

	virtual string print() const
	{
		return type + "[" + to_string(id) + "]" + (updated ? "U" : "u") + location.print();
	}

	friend ostream &operator<<(ostream &os, Entity const &m)
	{
		return os << m.print();
	}
};

class Mine : public Entity
{
	public:
	int timeToDestruction;
	bool blocker;
	
	Mine(int _id, int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		id = _id;
		type = "MINE";
		timeToDestruction = 100000000;
		update(_x, _y, _arg1, _arg2, _arg3, _arg4);
	}
	
	Mine& operator=(const Mine other)
	{
		id = other.id;
		location = other.location;
		updated = other.updated;
		type = other.type;
		timeToDestruction = other.timeToDestruction;
		return *this;
	}
	
	void update(int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		Entity::update(_x, _y, _arg1, _arg2, _arg3, _arg4);
	}

	bool beingShot() const
	{
		return timeToDestruction < 10;
	}
	
	virtual bool blocking(Location target, int atTurn = 0) const
	{
		return target == location && timeToDestruction > atTurn;
	}
};

class Cannonball : public Entity
{
	public:
	int firedFrom;
	int turnsBeforeImpact;

	Cannonball(int _id, int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		id = _id;
		type = "CANNONBALL";
		update(_x, _y, _arg1, _arg2, _arg3, _arg4);
	}
	
	Cannonball& operator=(const Cannonball other)
	{
		id = other.id;
		location = other.location;
		updated = other.updated;
		type = other.type;
		firedFrom = other.firedFrom;
		turnsBeforeImpact = other.turnsBeforeImpact;
		return *this;
	}

	void update(int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
//cerr << "Cannonball[" << id << "]::update(" << _x << "," << _y << "," << arg1 << "," << arg2 << "," << arg3 << "," << arg4 << ")" << endl;
		firedFrom = _arg1;
		turnsBeforeImpact = _arg2;
		Entity::update(_x, _y, _arg1, _arg2, _arg3, _arg4);
	}

	virtual bool blocking(Location target, int atTurn = 0) const
	{
		return target == location && atTurn == turnsBeforeImpact;
	}
};

class Barrel : public Entity
{
	public:
	int rum;

	Barrel(int _id, int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		id = _id;
		type = "BARREL";
		update(_x, _y, _arg1, _arg2, _arg3, _arg4);
	}
	
	Barrel& operator=(const Barrel other)
	{
		id = other.id;
		location = other.location;
		updated = other.updated;
		type = other.type;
		rum = other.rum;
		return *this;
	}

	void update(int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		rum = _arg1;
		Entity::update(_x, _y, _arg1, _arg2, _arg3, _arg4);
	}
};

int findDistance(Location from, Location to, int limit = 10)
{
	int xFrom = from.x - (from.y - (from.y & 1)) / 2;
	int zFrom = from.y;
    int yFrom = -(xFrom + zFrom);

	int xTo = to.x - (to.y - (to.y & 1)) / 2;
	int zTo = to.y;
    int yTo = -(xTo + zTo);
		
	return min(limit, (abs(xFrom - xTo) + abs(yFrom - yTo) + abs(zFrom - zTo)) / 2);
}

int findClosestLocation(Location from, vector<Location> targets, int limit = 100000)
{
	int distance = limit;
	for(vector<Location>::const_iterator loc = targets.begin(); loc != targets.end(); ++loc)
	{
		int thisDistance = findDistance(from, *loc, distance);
		if (distance != 0 && distance > thisDistance)
		{
			distance = thisDistance;
		}
	}
	
	return distance;
}

Location getClosestLocation(Location from, vector<Location> targets)
{
	int distance = 1000000;
	Location returnLocation(-1, -1);
	for(vector<Location>::const_iterator loc = targets.begin(); loc != targets.end(); ++loc)
	{
		int thisDistance = findDistance(from, *loc, distance);
		if (distance != 0 && distance > thisDistance)
		{
			distance = thisDistance;
			returnLocation = *loc;
		}
	}
	
	return returnLocation;
}

Location getFurthestLocation(Location from, vector<Location> targets)
{
	int distance = 1000000;
	Location returnLocation(-1, -1);
	for(vector<Location>::const_iterator loc = targets.begin(); loc != targets.end(); ++loc)
	{
		int thisDistance = findDistance(from, *loc, distance);
		if (distance != 0 && distance < thisDistance)
		{
			distance = thisDistance;
			returnLocation = *loc;
		}
	}
	
	return returnLocation;
}

class Ship : public Entity
{
	public:
	int turnsUntilCannon;
	int turnsUntilMine;
	int order;
	int barrelsCollected;
	int rumCollected;
	Location tempLocation;
	Location tempBow;
	Location tempStern;
	int tempHeading;
	bool collisionDetected;
	map<int, string> path;
	bool usingPath;
	
	Ship(int _id, int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		id = _id;
		type = "SHIP";
		update(_x, _y, _arg1, _arg2, _arg3, _arg4);
		order = -1;
	}

	Ship& operator=(Ship other)
	{
		id = other.id;
		location = other.location;
		updated = other.updated;
		type = other.type;
		arg1 = other.arg1;
		arg2 = other.arg2;
		arg3 = other.arg3;
		arg4 = other.arg4;
		turnsUntilCannon = other.turnsUntilCannon;
		turnsUntilMine = other.turnsUntilMine;
		return *this;
	}

	void update(int _x, int _y, int _arg1, int _arg2, int _arg3, int _arg4)
	{
		turnsUntilCannon = max(0, turnsUntilCannon - 1);
		turnsUntilMine = max(0, turnsUntilMine - 1);
		barrelsCollected = 0;
		rumCollected = 0;
		Entity::update(_x, _y, _arg1, _arg2, _arg3, _arg4);
	}
	
	void setHeading(int heading)
	{
		arg1 = heading;
	}
	
	int heading() const
	{
		return arg1;
	}

	void setSpeed(int speed)
	{
		arg2 = speed;
	}
	
	int speed() const
	{
		return arg2;
	}

	void setRum(int rum)
	{
		arg3 = rum;
	}
	
	int rum() const
	{
		return arg3;
	}

	void setOwner(int owner)
	{
		arg4 = owner;
	}
	
	bool owned() const
	{
		return arg4 == 1;
	}

	virtual bool blocking(Location target, int atTurn = 0) const
	{		
		return target == location || target == bow() || target == stern();
	}
	
	int moveMask(int moveNum) const
	{
		return moveNum << (order * MOVE_BITS);
	}

	Location mineLocation()
	{
		Location sternLocation = stern();
		Location minePlacement = sternLocation.neighborHeading((heading() + 3) % 6);
		if (minePlacement != sternLocation && minePlacement != location)
		{
			return minePlacement;
		}
		
		return Location(-1, -1);
	}
	
	Location bow() const
	{
		return location.neighborHeading(heading());
	}

	Location stern() const
	{
		return location.neighborHeading((heading() + 3) % 6);
	}
	
	Location nextLocation(Location input) const
	{
		return input.neighborHeading(heading());
	}

	Location locationIn(int turns) const
	{
		Location projectedLocation = location;
		for (int i = 0; i < turns; i++)
		{
			for (int j = 0; j < speed(); j++)
			{
				projectedLocation = nextLocation(projectedLocation);
			}
		}
		
		return projectedLocation;
	}
	
	void clearPath()
	{
		path.clear();
	}
	
	void addPath(int turnNumber, string moveString)
	{
		path[turnNumber] = moveString;
	}
	
	Move pathMove(int turnNumber, map<int, Entity*> entities)
	{
		if (path[turnNumber] != "")
		{
			if (path[turnNumber] == "WAIT")
			{
				usingPath = true;
				return defaultMove(entities, false);
			}

			Move outputMove = Move(id, path[turnNumber], "Swiggity Swooty");
			usingPath = true;
			return outputMove;
		}

		usingPath = false;
		return evadeMove(entities);
	}

	Move defaultMove(map<int, Entity*> entities, bool maintainSpeedOne = true)
	{
		Ship* closestShip = 0;
		Location cannon = bow();
		Location placeMine = mineLocation();
		Mine* closestMine = 0;
		bool canPlaceMine = true;
		for (map<int, Entity*>::iterator entityIt = entities.begin(); entityIt != entities.end(); ++entityIt)
		{
			if (entityIt->second->type == "SHIP")
			{
				Ship* ship = (Ship*)entityIt->second;
				if (ship->id != id)
				{
					if (owned() != ship->owned())
					{
						// If there is no closest ship yet
						if (closestShip == 0)
						{
							closestShip = ship;
						}
						else
						{
							if (findDistance(cannon, ship->location, 11) < findDistance(cannon, closestShip->location, 11))
							{
								closestShip = ship;
							}
						}
					}
					
					if (ship->location == placeMine || ship->bow() == placeMine || ship->stern() == placeMine)
					{
						canPlaceMine = false;
					}
				}
			}
			else if (entityIt->second->type == "MINE")
			{
				Mine* mine = (Mine*)entityIt->second;
				if (!mine->beingShot())
				{
					if (closestMine == 0)
					{
						closestMine = mine;
					}
					else
					{
						if (findDistance(cannon, mine->location, 11) < findDistance(cannon, closestMine->location, 11))
						{
							closestMine = mine;
						}
					}
				}

				if (mine->location == placeMine)
				{
					canPlaceMine = false;
				}
			}
			else if (entityIt->second->type == "BARREL")
			{
				if (entityIt->second->location == placeMine)
				{
					canPlaceMine = false;
				}
			}
		}

		if (closestShip != 0)
		{
			if (canPlaceMine && turnsUntilMine <= 0)
			{
				if (placeMine == closestShip->location.neighborHeading(closestShip->heading()).neighborHeading(closestShip->heading()))
				{
					return Move(id, "MINE", "Scurry off!");
				}
			}
				
			if (turnsUntilCannon <= 0)
			{
				vector<Location> shots;
				for (int i = 1; i <= 4; i++)
				{
					Location proj = closestShip->locationIn(i);
					Location projBow = proj.neighborHeading(closestShip->heading());
					Location projStern = proj.neighborHeading((closestShip->heading() + 3) % 6);
					int shotDistance = findDistance(cannon, proj, 11);
					int shotDistanceBow = findDistance(cannon, projBow, 11);
					int shotDistanceStern = findDistance(cannon, projStern, 11);
					if (shotDistance <= 10 && ((i == 1 + round((float)shotDistance / 3.0))))
					{
						shots.insert(shots.begin(), proj);
					}
					else if (shotDistanceBow <= 10 && (i == 1 + round((float)shotDistanceBow / 3.0)))
					{
						shots.push_back(projBow);
					}
					else if (shotDistanceStern <= 10 && (i == 1 + round((float)shotDistanceStern / 3.0)))
					{
						shots.push_back(projStern);
					}
					
					if (!shots.empty())
					{
						string targetFlag;
						switch(closestShip->id)
						{
							case 0:
							case 1:
								targetFlag = "Codingame";
								break;
							case 2:
							case 3:
								targetFlag = "Cutlasses";
								break;
							case 4:
							case 5:
								targetFlag = "Skull";
								break;
							default:
								targetFlag = "that ship";
								break;
						}

						return Move(id, "FIRE", shots[0], "Fire on " + targetFlag + "!");
					}
				}
			}
		}
		
		if (maintainSpeedOne)
		{
			switch (speed())
			{
				case 0:
					return Move(id, "FASTER", "Make speed!");
				case 2:
					return Move(id, "SLOWER", "Hold steady.");
				case 1:
				default:
					if (closestMine != 0 && turnsUntilCannon <= 0)
					{
						int shotDistanceMine = findDistance(cannon, closestMine->location, 11);
						if (shotDistanceMine <= 10)
						{
							return Move(id, "FIRE", closestMine->location, "Scuttle the mine.");
						}
					}

					return Move(id, "WAIT", "Keep her so.");
			}
		}

		return Move(id, "WAIT", "Keep her so.");
	}

	Move evadeMove(map<int, Entity*> entities)
	{
		if (speed() < 2)
		{
			if (location.x < GRID_WIDTH / 2)
			{
				switch(heading())
				{
					case 0:
						if (speed() < 2)
							return Move(id, "FASTER", "Weigh anchor!");
						else
							return defaultMove(entities, false);
					case 1:
					case 2:
					case 3:
						return Move(id, "STARBOARD", "Turn about!");
					case 4:
					case 5:
						return Move(id, "PORT", "Turn about!");	
				}
			}
			else
			{
				switch(heading())
				{
					case 0:
					case 1:
					case 2:
						return Move(id, "PORT", "Turn about!");	
					case 3:
						if (speed() < 2)
							return Move(id, "FASTER", "Weigh anchor!");
						else
							return defaultMove(entities, false);
					case 4:
					case 5:
						return Move(id, "STARBOARD", "Turn about!");
				}
			}
		}
		else
		{
			return randomTurn(entities);
		}
	}

	Move shootMove(map<int, Entity*> entities)
	{
		Ship* closestShip = 0;
		Location cannon = bow();
		for (map<int, Entity*>::iterator entityIt = entities.begin(); entityIt != entities.end(); ++entityIt)
		{
			if (entityIt->second->type == "SHIP")
			{
				Ship* ship = (Ship*)entityIt->second;
				if (ship->id != id)
				{
					if (owned() != ship->owned())
					{
						// If there is no closest ship yet
						if (closestShip == 0)
						{
							closestShip = ship;
						}
						else
						{
							if (findDistance(cannon, ship->location, 11) < findDistance(cannon, closestShip->location, 11))
							{
								closestShip = ship;
							}
						}
					}
				}
			}
		}

		if (closestShip != 0)
		{				
			if (turnsUntilCannon <= 0)
			{
				vector<Location> shots;
				for (int i = 1; i <= 4; i++)
				{
					Location proj = closestShip->locationIn(i);
					Location projBow = proj.neighborHeading(closestShip->heading());
					Location projStern = proj.neighborHeading((closestShip->heading() + 3) % 6);
					int shotDistance = findDistance(cannon, proj, 11);
					int shotDistanceBow = findDistance(cannon, projBow, 11);
					int shotDistanceStern = findDistance(cannon, projStern, 11);
					if (shotDistance <= 10 && ((i == 1 + round((float)shotDistance / 3.0))))
					{
						shots.insert(shots.begin(), proj);
					}
					else if (shotDistanceBow <= 10 && (i == 1 + round((float)shotDistanceBow / 3.0)))
					{
						shots.push_back(projBow);
					}
					else if (shotDistanceStern <= 10 && (i == 1 + round((float)shotDistanceStern / 3.0)))
					{
						shots.push_back(projStern);
					}
					
					if (!shots.empty())
					{
						string targetFlag;
						switch(closestShip->id)
						{
							case 0:
							case 1:
								targetFlag = "Codingame";
								break;
							case 2:
							case 3:
								targetFlag = "Cutlasses";
								break;
							case 4:
							case 5:
								targetFlag = "Skull";
								break;
							default:
								targetFlag = "that ship";
								break;
						}
						return Move(id, "FIRE", shots[0], "Fire on " + targetFlag + "!");
					}
				}
			}
		}
		
		if (location.x < GRID_WIDTH / 2)
		{
			switch(heading())
			{
				case 0:
					return Move(id, "WAIT", "Keep her so.");
				case 1:
				case 2:
				case 3:
					return Move(id, "STARBOARD", "Turn about!");
				case 4:
				case 5:
					return Move(id, "PORT", "Turn about!");	
			}
		}
		else
		{
			switch(heading())
			{
				case 0:
				case 1:
				case 2:
					return Move(id, "PORT", "Turn about!");	
				case 3:
					return Move(id, "WAIT", "Keep her so.");
				case 4:
				case 5:
					return Move(id, "STARBOARD", "Turn about!");
			}
		}
	}

	Move randomMove(map<int, Entity*> entities)
	{
		switch(randomOperation(randomEngine))
		{
			case 0:
				return Move(id, "FASTER");
			case 1:
				return Move(id, "SLOWER");
			case 2:
				return Move(id, "STARBOARD");
			case 3:
				return Move(id, "PORT");
			case 4:
				return Move(id, "MINE");
			case 5:
			default:
				return defaultMove(entities);
		}
	}
	
	Move randomTurn(map<int, Entity*> entities)
	{
		switch(randomBinary(randomEngine))
		{
			case 0:
				return Move(id, "STARBOARD", "Starboard");
			case 1:
			default:
				return Move(id, "PORT", "Port");
		}
	}
	
	string print() const
	{
		string baseString = Entity::print();
		return baseString + "{" + to_string(rum()) + "}<" + to_string(heading()) + "," + to_string(speed()) + ">" + (owned() ? "O" : "o");
	}
	
	friend ostream &operator<<(ostream &os, Ship const &m)
	{
		return os << m.print();
	}
};

class ShipPather
{
	int id;

	public:
	int heading;
	int speed;
	Location location;
	string previousMove;
	int turnNumber;
	
	ShipPather()
	{
	}

	ShipPather(Ship* ship)
	{
		heading = ship->heading();
		speed = ship->speed();
		location = ship->location;
		previousMove = "WAIT";
		updateId();
		turnNumber = 0;
	}

	ShipPather& operator=(ShipPather other)
	{
		heading = other.heading;
		speed = other.speed;
		id = other.id;
		location = other.location;
		previousMove = other.previousMove;
		turnNumber = other.turnNumber;
		return *this;
	}

	void updateId()
	{
		id = 1000000 + location.x * 100000 + location.y * 1000 + heading * 10 + speed;
	}
	
    bool operator==(const ShipPather& other) const
    {
        return id == other.id;
    }
	
    bool operator!=(const ShipPather& other) const
    {
        return !(*this == other);
    }

    bool operator<(const ShipPather& other) const
    {
        return id < other.id;
    }

	vector<ShipPather> neighbors()
	{
		vector<ShipPather> myNeighbors;
		ShipPather next = *this;
		next.turnNumber = turnNumber + 1;

		ShipPather faster = next;
		if (speed == 2)
		{
			faster.previousMove = "WAIT";
		}
		else
		{
			faster.speed = speed + 1;
			faster.previousMove = "FASTER";
		}
		for (int i = 0; i < faster.speed; i++)
		{
			faster.location = faster.location.neighborHeading(faster.heading);
		}
		faster.updateId();
		myNeighbors.push_back(faster);

		ShipPather slower = next;
		if (speed == 0)
		{
			slower.previousMove = "WAIT";
		}
		else
		{
			slower.speed = speed - 1;
			slower.previousMove = "SLOWER";
		}
		for (int i = 0; i < slower.speed; i++)
		{
			slower.location = slower.location.neighborHeading(slower.heading);
		}
		slower.updateId();
		myNeighbors.push_back(slower);

		for (int i = 0; i < next.speed; i++)
		{
			next.location = next.location.neighborHeading(next.heading);
		}

		ShipPather port = next;
		port.heading = (heading + 6 + 1) % 6;
		port.previousMove = "PORT";
		port.updateId();
		myNeighbors.push_back(port);

		ShipPather starboard = next;
		starboard.heading = (heading + 6 - 1) % 6;
		starboard.previousMove = "STARBOARD";
		starboard.updateId();
		myNeighbors.push_back(starboard);
		
		return myNeighbors;
	}
};

class Grid
{
	private:
	int newId;

	public:
	map<int, Entity*> entities;
	int totalScore;
	vector<Move> trialMoves;
	
	Grid()
	{
	}
	
	~Grid()
	{
		for (map<int, Entity*>::iterator it = entities.begin(); it != entities.end(); ++it)
		{
			delete it->second;
		}
		
		entities.clear();
	}
	
	Grid* getGrid(vector<Move> _trialMoves)
	{
		Grid* newGrid = new Grid();

		for (vector<Move>::iterator move = _trialMoves.begin(); move != _trialMoves.end(); ++move)
		{
			newGrid->trialMoves.push_back(*move);
		}

		for(map<int, Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity)
		{
			string entityType = entity->second->type;

			if (entityType == "SHIP")
			{
				newGrid->entities[entity->first] = new Ship(entity->second->id, entity->second->location.x, entity->second->location.y, entity->second->arg1, entity->second->arg2, entity->second->arg3, entity->second->arg4);
				Ship* newShip = (Ship*)newGrid->entities[entity->first];
				Ship* oldShip = (Ship*)entity->second;
				newShip->turnsUntilCannon = oldShip->turnsUntilCannon;
				newShip->turnsUntilMine = oldShip->turnsUntilMine;
				newShip->barrelsCollected = oldShip->barrelsCollected;
				newShip->rumCollected = oldShip->rumCollected;
				newShip->usingPath = oldShip->usingPath;
				if (newShip->usingPath)
				{
					for (map<int, string>::iterator it = oldShip->path.begin(); it != oldShip->path.end(); ++it)
					{
						newShip->path[it->first] = it->second;
					}
				}
			}
			else if (entityType == "BARREL")
			{
				newGrid->entities[entity->first] = new Barrel(entity->second->id, entity->second->location.x, entity->second->location.y, entity->second->arg1, entity->second->arg2, entity->second->arg3, entity->second->arg4);
			}
			else if (entityType == "MINE")
			{
				newGrid->entities[entity->first] = new Mine(entity->second->id, entity->second->location.x, entity->second->location.y, entity->second->arg1, entity->second->arg2, entity->second->arg3, entity->second->arg4);
			}
			else if (entityType == "CANNONBALL")
			{
				newGrid->entities[entity->first] = new Cannonball(entity->second->id, entity->second->location.x, entity->second->location.y, entity->second->arg1, entity->second->arg2, entity->second->arg3, entity->second->arg4);
			}
		}
		
		newGrid->totalScore = 0;
		newGrid->newId = 1000;
		return newGrid;
	}

	void gridWith(vector<Move> moves, int iteration)
	{
		// This iterates through all ships a lot, so setup a temp vector to hold them
		vector<Ship*> ships;
		vector<Cannonball*> cannonballs;
		for(map<int, Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity)
		{				
			if (entity->second->type == "CANNONBALL")
			{
				Cannonball* cannonball = (Cannonball*)entity->second;
				cannonballs.push_back(cannonball);
				cannonball->turnsBeforeImpact--;
			}
			else if (entity->second->type == "SHIP")
			{
				Ship* ship = (Ship*)entity->second;
				ships.push_back(ship);
				ship->setRum(ship->rum() - 1);
				ship->turnsUntilCannon--;
				ship->turnsUntilMine--;
				ship->tempHeading = ship->heading();
				for(vector<Move>::iterator move = moves.begin(); move != moves.end(); ++move)
				{
					if (move->entityId == ship->id)
					{
						if (move->action == "MINE")
						{
							Location placeMine = ship->mineLocation();
							if (placeMine.x != -1)
							{
								entities[newId] = new Mine(newId, placeMine.x, placeMine.y, 0, 0, 0, 0);
								newId++;
							}

							ship->turnsUntilMine = 5;
						}
						else if (move->action == "FIRE")
						{
							ship->turnsUntilCannon = 2;
							entities[newId] = new Cannonball(newId, move->target.x, move->target.y, ship->id, 1 + round((float)findDistance(ship->bow(), move->target) / 3.0), 0, 0);
							cannonballs.push_back((Cannonball*)entities[newId]);
							newId++;
						}
						else if (move->action == "FASTER")
						{
							ship->setSpeed(min(ship->speed() + 1, 2));
						}
						else if (move->action == "SLOWER")
						{
							ship->setSpeed(max(ship->speed() - 1, 0));
						}
						else if (move->action == "STARBOARD")
						{
							ship->tempHeading = (ship->heading() + 6 - 1) % 6;
						}
						else if (move->action == "PORT")
						{
							ship->tempHeading = (ship->heading() + 6 + 1) % 6;
						}
					}
				}
			}
		}
		
		// Move forward, checking for collisions
		for (int i = 1; i <= 2; i++)
		{
			// Fill temporary positions
			for(vector<Ship*>::iterator shipIt = ships.begin(); shipIt != ships.end(); ++shipIt)
			{
				Ship* ship = *shipIt;

				ship->collisionDetected = false;
				
				if (ship->speed() >= i)
				{
					ship->tempLocation = ship->bow();
					ship->tempBow = ship->tempLocation.neighborHeading(ship->heading());
					ship->tempStern = ship->tempLocation.neighborHeading((ship->heading() + 3) % 6);
				}
				else
				{
					ship->tempLocation = ship->location;
					ship->tempBow = ship->bow();
					ship->tempStern = ship->stern();
				}
			}

			// Check for collisions with other ships
			checkCollide(ships);

			// Set new posisions or stop
			for(vector<Ship*>::iterator shipIt = ships.begin(); shipIt != ships.end(); ++shipIt)
			{
				Ship* ship = *shipIt;
				if (!ship->collisionDetected)
				{
					ship->location = ship->tempLocation;
					checkHit(this, ship);
					
					// If fell off edge
					if (ship->bow() == ship->location)
					{
						ship->setSpeed(0);
					}
				}
				else
				{
					ship->setSpeed(0);
				}
			}
		}
		
		// Rotate, Fill temporary positions
		for(vector<Ship*>::iterator shipIt = ships.begin(); shipIt != ships.end(); ++shipIt)
		{
			Ship* ship = *shipIt;
			ship->collisionDetected = false;
				
			if (ship->tempHeading != ship->heading())
			{
				ship->tempLocation = ship->location;
				ship->tempBow = ship->location.neighborHeading(ship->tempHeading);
				ship->tempStern = ship->location.neighborHeading((ship->tempHeading + 3) % 6);
			}
			else
			{
				ship->tempLocation = ship->location;
				ship->tempBow = ship->bow();
				ship->tempStern = ship->stern();
			}
		}
		
		// Check for collisions with other ships
		checkCollide(ships);
		
		// Set new posisions or stop
		for(vector<Ship*>::iterator shipIt = ships.begin(); shipIt != ships.end(); ++shipIt)
		{
			Ship* ship = *shipIt;
			if (!ship->collisionDetected)
			{
				ship->setHeading(ship->tempHeading);
				checkHit(this, ship);
			}
			else
			{
				ship->setSpeed(0);
			}
		}

		// Check cannonballs
		for(vector<Cannonball*>::iterator cannonballIt = cannonballs.begin(); cannonballIt != cannonballs.end(); ++cannonballIt)
		{
			Cannonball* cannonball = *cannonballIt;
			if (cannonball->turnsBeforeImpact == 0)
			{
				for(map<int, Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity)
				{
					if (entity->second->type == "SHIP")
					{
						Ship* ship = (Ship*)entity->second;
						if (ship->location == cannonball->location)
						{
							ship->setRum(ship->rum() - 50);
						}
						else if (ship->bow() == cannonball->location || ship->stern() == cannonball->location)
						{
							ship->setRum(ship->rum() - 25);
						}
					}
					else if (entity->second->type == "MINE")
					{
						if (cannonball->location == entity->second->location)
						{
							vector<Location> blastRing = entity->second->location.neighbors();
							for(map<int, Entity*>::iterator blastedEntity = entities.begin(); blastedEntity != entities.end(); ++blastedEntity)
							{
								if (blastedEntity->second->type == "SHIP")
								{
									Ship* blastedShip = (Ship*)blastedEntity->second;
									for (vector<Location>::iterator blast = blastRing.begin(); blast != blastRing.end(); ++blast)
									{
										if (blastedShip->location == *blast || blastedShip->bow() == *blast || blastedShip->stern() == *blast)
										{
											blastedShip->setRum(blastedShip->rum() - 10);
										}
									}
								}
							}
							entities.erase(entity->first);
						}
					}
					else if (entity->second->type == "BARREL")
					{
						if (cannonball->location == entity->second->location)
						{
							entities.erase(entity->first);
						}
					}
				}
			}
		}

		for(map<int, Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity)
		{
			if (entity->second->type == "CANNONBALL")
			{
				Cannonball* ball = (Cannonball*)entity->second;
				if (ball->turnsBeforeImpact == 0)
				{
					for(map<int, Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity)
					{
						if (entity->second->type == "SHIP")
						{
							Ship* ship = (Ship*)entity->second;
							if (ship->location == ball->location)
							{
								ship->setRum(ship->rum() - 50);
							}
							else if (ship->bow() == ball->location || ship->stern() == ball->location)
							{
								ship->setRum(ship->rum() - 25);
							}
						}
						else if (entity->second->type == "MINE")
						{
							vector<Location> blastRing = entity->second->location.neighbors();
							for(map<int, Entity*>::iterator blastedEntity = entities.begin(); blastedEntity != entities.end(); ++blastedEntity)
							{
								if (blastedEntity->second->type == "SHIP")
								{
									Ship* blastedShip = (Ship*)blastedEntity->second;
									for (vector<Location>::iterator blast = blastRing.begin(); blast != blastRing.end(); ++blast)
									{
										if (blastedShip->location == *blast || blastedShip->bow() == *blast || blastedShip->stern() == *blast)
										{
											blastedShip->setRum(blastedShip->rum() - 10);
										}
									}
								}
							}
							entities.erase(entity->first);
						}
						else if (entity->second->type == "BARREL")
						{
							entities.erase(entity->first);
						}
					}
				}
			}
		}
		
		// Remove dead ships
		for(vector<Ship*>::iterator shipIt = ships.begin(); shipIt != ships.end(); ++shipIt)
		{
			Ship* ship = *shipIt;

			if (ship->rum() <= 0)
			{
				// Drop barrel where ship was (just set to 30 for now)
				entities[newId] = new Barrel(newId, ship->location.x, ship->location.y, 30, 0, 0, 0);
				newId++;

				// Remove ship
				entities.erase(ship->id);
			}
		}
		
		totalScore += (float)score(iteration==1) * ((float)MAX_ITERATIONS / (float)iteration);
	}

	void checkCollide(vector<Ship*> ships)
	{
		for(vector<Ship*>::iterator shipIt = ships.begin(); shipIt != ships.end(); ++shipIt)
		{
			Ship* ship = *shipIt;
				
			for(vector<Ship*>::iterator shipOtherIt = ships.begin(); shipOtherIt != ships.end(); ++shipOtherIt)
			{
				Ship* shipOther = *shipOtherIt;
				if (shipOther->id != ship->id)
				{
					if ((ship->tempBow == shipOther->tempLocation || ship->tempBow == shipOther->tempBow || ship->tempBow == shipOther->tempStern)
					 || (ship->tempLocation == shipOther->tempLocation || ship->tempLocation == shipOther->tempBow || ship->tempLocation == shipOther->tempStern)
					 || (ship->tempStern == shipOther->tempLocation || ship->tempStern == shipOther->tempBow || ship->tempStern == shipOther->tempStern))
					{
						ship->collisionDetected = true;
					}
				}
			}
		}
	}
	
	bool checkHit(Grid* thisGrid, Ship* thisShip)
	{
		int hitEntityId = -1;
		
		vector<int> hitEntityIds;
		for(map<int, Entity*>::iterator entity = thisGrid->entities.begin(); entity != thisGrid->entities.end(); ++entity)
		{
			if (entity->first != thisShip->id)
			{
				if (entity->second->location == thisShip->location || entity->second->location == thisShip->bow() || entity->second->location == thisShip->stern())
				{
					hitEntityIds.push_back(entity->first);
				}
			}
		}
		
		for (vector<int>::iterator hit = hitEntityIds.begin(); hit != hitEntityIds.end(); ++hit)
		{
			if (thisGrid->entities[*hit]->type == "MINE")
			{
				thisShip->setRum(thisShip->rum() - 25);
				vector<Location> blastRing = thisGrid->entities[*hit]->location.neighbors();
				for(map<int, Entity*>::iterator entity = thisGrid->entities.begin(); entity != thisGrid->entities.end(); ++entity)
				{
					if (entity->first != thisShip->id)
					{
						if (entity->second->type == "SHIP")
						{
							Ship* otherShip = (Ship*)entity->second;
							for (vector<Location>::iterator blast = blastRing.begin(); blast != blastRing.end(); ++blast)
							{
								if (otherShip->location == *blast || otherShip->bow() == *blast || otherShip->stern() == *blast)
								{
									otherShip->setRum(otherShip->rum() - 10);
								}
							}
						}
					}
				}
				
				thisGrid->entities.erase(*hit);
			}
			else if (thisGrid->entities[*hit]->type == "BARREL")
			{
				Barrel* barrel = (Barrel*)thisGrid->entities[*hit];
				thisShip->barrelsCollected++;
				thisShip->rumCollected += barrel->rum;
				thisShip->setRum(min(100, thisShip->rum() + barrel->rum));
				thisGrid->entities.erase(*hit);
			}
		}
		
		return false;
	}
	
	void findPath(Ship* ship, Location goal)
	{
		ship->clearPath();
		ShipPather start(ship);
		vector<ShipPather> closedSet;
		vector<ShipPather> openSet;
		openSet.push_back(start);
		map<ShipPather, ShipPather> cameFrom;

		map<ShipPather, int> gScore;
		gScore[start] = 0;
		map<ShipPather, int> fScore;
		fScore[start] = findDistance(ship->bow(), goal, 50);

//cerr << "Find path " << ship << " to " << goal << endl;
		while(!openSet.empty())
		{
			ShipPather current = start; // Initialize to start, will change later
			int lowestFScore = 10000000;
			for(vector<ShipPather>::iterator it = openSet.begin(); it != openSet.end(); ++it)
			{
				if (fScore[*it] < lowestFScore)
				{
					lowestFScore = fScore[*it];
					current = *it;
//cerr << "Current with fscore " << lowestFScore << endl;
				}
			}

//cerr << current.location << " " << goal << endl;
			if (current.location == goal || current.location.neighborHeading(current.heading % 6) == goal || current.location.neighborHeading((current.heading + 3) % 6) == goal)
			{
				while (current != ship)
				{
					ship->addPath(current.turnNumber, current.previousMove);
					current = cameFrom[current];					
				}

				return;
			}

			vector<ShipPather>::iterator openIt = find(openSet.begin(), openSet.end(), current);
			if (openIt != openSet.end())
			{
				openSet.erase(openIt);
			}
			closedSet.push_back(current);

			vector<ShipPather> neighbors = current.neighbors();
			for (vector<ShipPather>::iterator neighbor = neighbors.begin(); neighbor != neighbors.end(); ++neighbor)
			{
				// If already in closed set
				if (find(closedSet.begin(), closedSet.end(), *neighbor) != closedSet.end())
				{
//cerr << "neighbor in closed set, continue. " << (*neighbor).heading << (*neighbor).location << " " << (*neighbor).previousMove << endl;
					continue;
				}

//cerr << "Check neighbor " << (*neighbor).heading << (*neighbor).location << " " << (*neighbor).previousMove << endl;
				vector<int> hitEntityIds;
				int hitCost = 0;
				for(map<int, Entity*>::iterator entity = entities.begin(); entity != entities.end(); ++entity)
				{
					if (entity->first != ship->id)
					{
						if (((*neighbor).speed >= 1 && entity->second->location == current.location.neighborHeading(current.heading % 6).neighborHeading(current.heading % 6))
						 || ((*neighbor).speed == 2 && entity->second->location == current.location.neighborHeading(current.heading % 6).neighborHeading(current.heading % 6).neighborHeading(current.heading % 6))
						 || (entity->second->location == (*neighbor).location)
						 || (entity->second->location == (*neighbor).location.neighborHeading((*neighbor).heading % 6))
						 || (entity->second->location == (*neighbor).location.neighborHeading(((*neighbor).heading + 3) % 6)))
						{
							if (entity->second->type == "MINE")
							{
								hitCost += 25;
							}
							else if (entity->second->type == "CANNONBALL")
							{
								Cannonball* cannonball = (Cannonball*)entity->second;
								if (cannonball->turnsBeforeImpact == (*neighbor).turnNumber)
								{
									if (entity->second->location == (*neighbor).location)
									{
										hitCost += 50;
									}
									else
									{
										hitCost += 25;
									}
								}
							}
							else if (entity->second->type == "BARREL")
							{
								Barrel* barrel = (Barrel*)entity->second;
								hitCost -= barrel->rum;
							}
						}
					}
				}
//cerr << "hitCost " << hitCost << endl;
				int tentativeGScore = gScore[current] + 1 + hitCost;
				if (find(openSet.begin(), openSet.end(), *neighbor) == openSet.end())
				{
					openSet.push_back(*neighbor);
				}
				else if (tentativeGScore >= gScore[*neighbor])
				{
					continue;
				}
				
				cameFrom[*neighbor] = current;
				gScore[*neighbor] = tentativeGScore;
				fScore[*neighbor] = tentativeGScore + findDistance((*neighbor).location.neighborHeading((*neighbor).heading % 6), goal, 50);
			}
		}

		return;
	}

	int score(bool initialMove = false) const
	{
		int scoreValue = 0;
		vector<Ship*> currentShips;
		vector<Ship*> initialShips;
		vector<Ship*> initialEnemyShips;
		vector<Location> barrelLocations;
		vector<Location> myShipLocations;
        int myShips = 0;
        int theirShips = 0;
		for (map<int, Entity*>::const_iterator entity = entities.begin(); entity != entities.end(); ++entity)
		{
			if (entity->second->type == "SHIP")
			{
				Ship* ship = (Ship*)entity->second;
				if (ship->owned())
				{
				    myShips++;
					scoreValue += SCORE_SHIP;
					scoreValue += ship->rum() * SCORE_RUM;
					if (initialMove)
					{
//						int edgeScore = scoreValue;
						if (ship->location.x == 0             && (ship->heading() == 2 || ship->heading() == 4)) scoreValue += SCORE_EDGE;
						if (ship->location.x == 0             && (ship->heading() == 3)                        ) scoreValue += SCORE_HARD_EDGE;
						if (ship->location.y == 0             && (ship->heading() == 1 || ship->heading() == 2)) scoreValue += SCORE_EDGE;
						if (ship->location.x == GRID_WIDTH-1  && (ship->heading() == 1 || ship->heading() == 5)) scoreValue += SCORE_EDGE;
						if (ship->location.x == GRID_WIDTH-1  && (ship->heading() == 0)                        ) scoreValue += SCORE_HARD_EDGE;
						if (ship->location.y == GRID_HEIGHT-1 && (ship->heading() == 4 || ship->heading() == 5)) scoreValue += SCORE_EDGE;
						if (ship->location   == Location(0,            0)             && (ship->heading() == 2)) scoreValue += SCORE_HARD_EDGE;
						if (ship->location   == Location(0,            GRID_HEIGHT-1) && (ship->heading() == 4)) scoreValue += SCORE_HARD_EDGE;
						if (ship->location   == Location(GRID_WIDTH-1, 0)             && (ship->heading() == 1)) scoreValue += SCORE_HARD_EDGE;
						if (ship->location   == Location(GRID_WIDTH-1, GRID_HEIGHT-1) && (ship->heading() == 5)) scoreValue += SCORE_HARD_EDGE;
//						edgeScore -= scoreValue;
//						cerr << ship << " edge score " << edgeScore << endl;

						initialShips.push_back(ship);
					}

					currentShips.push_back(ship);
					myShipLocations.push_back(ship->location);
					if (ship->speed() == 0 || ship->bow() == ship->location) scoreValue += SCORE_STOPPED;
					scoreValue += ship->rumCollected * SCORE_RUM_COLLECTED;
				}
				else
				{
				    theirShips++;
					scoreValue += SCORE_ENEMY_SHIP;
					scoreValue += ship->rum() * SCORE_ENEMY_RUM;
					initialEnemyShips.push_back(ship);
				}
			}
			else if (entity->second->type == "BARREL")
			{
				barrelLocations.push_back(entity->second->location);
				scoreValue += SCORE_REMAINING_BARREL;
			}
		}
		
		if (theirShips == 0)
		{
		    scoreValue += SCORE_WIN;
		}
		if (myShips == 0)
		{
		    scoreValue += SCORE_LOSE;
		}

		
		int referenceRum = 1000;
		Ship* bloodShip = 0;
		for (vector<Ship*>::const_iterator blood = initialEnemyShips.begin(); blood != initialEnemyShips.end(); ++blood)
		{
			if ((*blood)->rum() < referenceRum)
			{
				referenceRum = (*blood)->rum();
				bloodShip = (*blood);
			}
		}
		
		referenceRum = -1;
		Ship* honeyShip = 0;
		for (vector<Ship*>::const_iterator honey = initialEnemyShips.begin(); honey != initialEnemyShips.end(); ++honey)
		{
			if ((*honey)->rum() > referenceRum)
			{
				referenceRum = (*honey)->rum();
				honeyShip = (*honey);
			}
		}

		for (vector<Ship*>::const_iterator form1 = currentShips.begin(); form1 != currentShips.end(); ++form1)
		{
			int formDistance = findClosestLocation((*form1)->location, myShipLocations, FORMATION_SEARCH);
			if (formDistance == 1)
			{
				scoreValue += SCORE_FORMATION_1;
			}
			if (formDistance == 2)
			{
				scoreValue += SCORE_FORMATION_2;
			}
			else if (formDistance >= FORMATION_MAX)
			{
				scoreValue += formDistance * SCORE_FORMATION_FAR;
			}

			if (bloodShip != 0)
			{
				int bloodDistance = findDistance((*form1)->location, bloodShip->location, 20);
				scoreValue += bloodDistance * SCORE_FAR_FROM_BLOOD;
			}

			if (honeyShip != 0)
			{
				if ((*form1) != honeyShip)
				{
					scoreValue += (*form1)->rum() * SCORE_HONEY_RUM;
				}
			}
		}
				
		for (vector<Ship*>::const_iterator form1 = initialShips.begin(); form1 != initialShips.end(); ++form1)
		{
			for (vector<Ship*>::const_iterator form2 = initialShips.begin(); form2 != initialShips.end(); ++form2)
			{
				if (findDistance((*form1)->location, (*form2)->location, FORMATION_SEARCH) <= FORMATION_MAX)
				{
					scoreValue += abs((*form1)->heading() - (*form2)->heading()) * SCORE_FORMATION_HEADING;
				}
			}
		}

		return scoreValue;
	}
};


/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	Grid grid;
	clock_t turnStartTime;
	int round = 1;
    // game loop
    while (1)
	{
		int shipOrder = 0;
        int myShipCount; // the number of remaining ships
        cin >> myShipCount; cin.ignore();
		turnStartTime = clock();
        int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
        cin >> entityCount; cin.ignore();
		// Setup entities for possible removal
		for (map<int, Entity*>::iterator entityIt = grid.entities.begin(); entityIt != grid.entities.end(); ++entityIt)
		{
			entityIt->second->updated = false;
		}

		// Update entities
        for (int i = 0; i < entityCount; i++)
		{
            int entityId;
            string entityType;
            int x;
            int y;
            int arg1;
            int arg2;
            int arg3;
            int arg4;
            cin >> entityId >> entityType >> x >> y >> arg1 >> arg2 >> arg3 >> arg4; cin.ignore();

			map<int, Entity*>::iterator entity = grid.entities.find(entityId);
			if (entity != grid.entities.end())
			{
				grid.entities[entityId]->update(x, y, arg1, arg2, arg3, arg4);
				if (entityType == "SHIP")
				{
					Ship* ship = (Ship*)grid.entities[entityId];
					ship->order = shipOrder++;
				}
			}
			else
			{
				if (entityType == "SHIP")
				{
					Ship* ship = new Ship(entityId, x, y, arg1, arg2, arg3, arg4);
					ship->order = shipOrder++;
					
					grid.entities[entityId] = ship;
				}
				else if (entityType == "BARREL")
				{
					grid.entities[entityId] = new Barrel(entityId, x, y, arg1, arg2, arg3, arg4);
				}
				else if (entityType == "MINE")
				{
					grid.entities[entityId] = new Mine(entityId, x, y, arg1, arg2, arg3, arg4);
				}
				else if (entityType == "CANNONBALL")
				{
					grid.entities[entityId] = new Cannonball(entityId, x, y, arg1, arg2, arg3, arg4);
				}
			}
        }
		
		// Project cannonballed mines
		for (map<int, Entity*>::iterator cannonballIt = grid.entities.begin(); cannonballIt != grid.entities.end(); ++cannonballIt)
		{
			if (cannonballIt->second->type == "CANNONBALL")
			{
				Cannonball* cannonball = (Cannonball*)cannonballIt->second;
				for (map<int, Entity*>::iterator mineIt = grid.entities.begin(); mineIt != grid.entities.end(); ++mineIt)
				{
					if (mineIt->second->type == "MINE")
					{
						Mine* mine = (Mine*)mineIt->second;
						if (cannonball->location == mine->location)
						{
							mine->timeToDestruction = min(mine->timeToDestruction, cannonball->turnsBeforeImpact);
						}
					}
				}
			}
		}
	
		// Remove deleted entities
		for (map<int, Entity*>::iterator entityIt = grid.entities.begin(); entityIt != grid.entities.end(); )
		{
			if (!entityIt->second->updated)
			{
				grid.entities.erase(entityIt++);
			}
			else
			{
				++entityIt;
			}
		}
		
		// Find targets
		vector<Location> barrelLocations;
		Location lowestEnemyRumTarget;
		Location highestEnemyRumTarget;
		int lowestEnemyRum = 1000;
		int lowestRum = 1000;
		int highestEnemyRum = 0;
		int highestRum = 0;
		for (map<int, Entity*>::iterator entityIt = grid.entities.begin(); entityIt != grid.entities.end(); ++entityIt)
		{
			if (entityIt->second->type == "BARREL")
			{
				barrelLocations.push_back(entityIt->second->location);
			}
			else if (entityIt->second->type == "SHIP")
			{
				Ship* ship = (Ship*)entityIt->second;
				int rumAmount = ship->rum();
				if (!ship->owned())
				{
					if (lowestEnemyRum > rumAmount)
					{
						lowestEnemyRum = rumAmount;
						lowestEnemyRumTarget = ship->locationIn(1);
					}
					
					if (highestEnemyRum < ship->rum())
					{
						highestEnemyRum = rumAmount;
						highestEnemyRumTarget = ship->locationIn(1);
					}
				}
				else
				{
					if (lowestRum > rumAmount)
					{
						lowestRum = rumAmount;
					}
					
					if (highestRum < rumAmount)
					{
						highestRum = rumAmount;
					}
				}
			}
		}

		// Set path targets
		for (map<int, Entity*>::iterator entityIt = grid.entities.begin(); entityIt != grid.entities.end(); ++entityIt)
		{
			if (entityIt->second->type == "SHIP")
			{
				Ship* ship = (Ship*)entityIt->second;
				if (ship->owned())
				{
					Location target = getClosestLocation(ship->location, barrelLocations);
					// If no more barrels
					if (target.x == -1)
					{
						// If honey is bigger than whale
						if (highestRum > highestEnemyRum)
						{
							if (ship->rum() == highestRum)
							{
								// Do nothing for pathing
								target = ship->location;
							}
							else
							{
								// Protect the honey ship by destroying enemies as fast as possible
								target = lowestEnemyRumTarget;
							}
						}
						else
						{
							// Bridge the gap
							target = highestEnemyRumTarget;
						}
					}
					else
					{
						// Have honey hunt whale to make sure we end up with a bigger honey
//						if ((ship->rum() == highestRum) && (ship->rum() != lowestRum))
						{
//							target = highestEnemyRumTarget;
						}
					}
					
					vector<Location>::iterator barrelIt = find(barrelLocations.begin(), barrelLocations.end(), target);
					if (barrelIt != barrelLocations.end())
					{
						barrelLocations.erase(barrelIt);
					}
					
					grid.findPath(ship, target);
				}
			}
		}

		// Setup permutations of moves
		int bestScore = -1000000000;
		vector<Move> moves;
		int totalTrials = pow(2, myShipCount * MOVE_BITS);
		vector<Grid*> trials;
		int iteration = 1;
		for (int trial = 0; trial < totalTrials; trial++)
		{
			vector<Move> trialMoves;
			for (map<int, Entity*>::iterator entityIt = grid.entities.begin(); entityIt != grid.entities.end(); ++entityIt)
			{
				if (entityIt->second->type == "SHIP")
				{
					Ship* ship = (Ship*)entityIt->second;
					Move move;
					if (ship->owned())
					{
						ship->usingPath = false;

						if ((ship->moveMask(3) & trial) == ship->moveMask(3))
						{
							move = ship->defaultMove(grid.entities);
						}
						else if  ((ship->moveMask(2) & trial) == ship->moveMask(2))
						{
							move = ship->shootMove(grid.entities);
						}
						else if  ((ship->moveMask(1) & trial) == ship->moveMask(1))
						{
							move = Move(entityIt->first, "WAIT", "Hold steady.");
						}
						else
						{
							move = ship->pathMove(1, grid.entities);
						}
					}
					else
					{
						move = ship->shootMove(grid.entities);
					}
					trialMoves.push_back(move);
//					cerr << trial << ": " << *ship << move << endl;
				}
			}

			Grid* next = grid.getGrid(trialMoves);
			next->gridWith(trialMoves, 1);
			trials.push_back(next);
		}
		
		// Simulate the trials
		for (int iteration = 2; (iteration < MAX_ITERATIONS) && (round + iteration < LAST_ROUND); iteration++)
		{
			for(vector<Grid*>::iterator next = trials.begin(); next != trials.end(); ++next)
			{
				vector<Move> defaultMoves;
				for (map<int, Entity*>::iterator entityIt = (*next)->entities.begin(); entityIt != (*next)->entities.end(); ++entityIt)
				{
					if (entityIt->second->type == "SHIP")
					{
						Ship* ship = (Ship*)entityIt->second;
						if (ship->owned())
						{
							if (ship->usingPath)
							{
								defaultMoves.push_back(ship->pathMove(iteration, grid.entities));
							}
							else
							{
								defaultMoves.push_back(ship->defaultMove((*next)->entities));
							}
						}
						else
						{
							defaultMoves.push_back(ship->shootMove((*next)->entities));
						}
					}
				}
				(*next)->gridWith(defaultMoves, iteration);
			}

			if (clock() > turnStartTime + (double)CLOCKS_PER_SEC * SIMULATION_DEADLINE)
			{
cerr << "Timeout at iteration " << iteration << endl;
				break;
			}
		}

		// Get best trial
		Grid* bestGrid;
		for(vector<Grid*>::iterator next = trials.begin(); next != trials.end(); ++next)
		{
			if (bestScore < (*next)->totalScore)
			{
				bestScore = (*next)->totalScore;
				bestGrid = *next;
cerr << "===NEW BEST: " << bestScore << "=== ";
for(vector<Move>::iterator move = bestGrid->trialMoves.begin(); move != bestGrid->trialMoves.end(); ++move)
{
	cerr << move->action << " ";
}
cerr << endl;
			}
		}

		// Output the moves
		for(vector<Move>::iterator move = bestGrid->trialMoves.begin(); move != bestGrid->trialMoves.end(); ++move)
		{
			Ship* ship = (Ship*)grid.entities[move->entityId];
			if (ship != 0 && ship->owned())
			{
				cout << move->action;
				if (move->action == "FIRE")
				{
					ship->turnsUntilCannon = 2;
					cout << " " << move->target.x << " " << move->target.y;
				}
				else if (move->action == "MINE")
				{
					ship->turnsUntilMine = 5;
				}

				if (move->comment == "")
				{
					cout << " " << move->action;
				}
				else
				{
					cout << " " << move->comment;
				}
				cout << endl;
			}
		}
    }
}
