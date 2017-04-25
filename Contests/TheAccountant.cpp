#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#define PLAYER_MOVE_RATE	1000
#define ENEMY_MOVE_RATE		500
#define DAMAGE_NUMERATOR	125000
#define DAMAGE_EXPONENT		1.2
#define DATA_SAVED_POINTS	100
#define ENEMY_KILLED_POINTS	10
#define DEAD_ZONE			2000
#define SIMULATION_DEADLINE	0.0095

static minstd_rand randomEngine(clock());
static uniform_real_distribution<> randomRotation{0, 2*M_PI};
static uniform_real_distribution<> randomDistance{0, PLAYER_MOVE_RATE};

class Coordinate
{
	public:
	int x;
	int y;

	Coordinate()
	{
		x = 0;
		y = 0;
	}
	
	Coordinate(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	
	Coordinate(float x, float y)
	{
		this->x = floor(x);
		this->y = floor(y);
	}
	
	Coordinate& operator=(Coordinate other)
	{
		this->x = other.x;
		this->y = other.y;
	}

	Coordinate operator+(const Coordinate& other) const
	{
		return Coordinate(x + other.x, y + other.y);
	}

	Coordinate operator-(const Coordinate& other) const
	{
		return Coordinate(x - other.x, y - other.y);
	}

    bool operator==(const Coordinate& other) const
    {
        return x == other.x && y == other.y;
    }
	
    bool operator!=(const Coordinate& other) const
    {
		return !(*this == other);
    }
	
	float distance(const Coordinate& other) const
	{
		return sqrt(pow((this->x - other.x), 2) + pow((this->y - other.y), 2));
	}
	
	// Returns the difference angle
	float angleTo(Coordinate other) const
	{
		Coordinate diff = *this - other;
		return diff.angle();
	}
	
	// Returns the angle of a vector in radians
	float angle() const
	{
//		return ((int)((atan2(y, x)) * RAD_TO_DEG) + CIRCLE_DEGREES) % CIRCLE_DEGREES;
		return atan2(y, x) + M_PI;
	}

	friend ostream &operator<<(ostream &os, Coordinate const &m)
	{
		return os << "(" << m.x << "," << m.y << ")";
	}
};

class Datum
{
    public:
	int id;
	Coordinate location;

	Datum()
	{
	}
	
	Datum(int it, Coordinate location)
	{
		this->id = id;
		this->location = location;
	}
	
	Datum& operator=(Datum other)
	{
		this->id = other.id;
		this->location = other.location;
	}
};

class Enemy
{
	private:
	float killDistance;
	int killDistanceCalculatedAt;

    public:
	int id;
	Coordinate location;
	int life;
	int movement;
	Datum targetDatum;

	Enemy()
	{
		movement = ENEMY_MOVE_RATE;
	}
	
	Enemy(int id, Coordinate location, int life)
	{
		movement = ENEMY_MOVE_RATE;
		this->id = id;
		this->location = location;
		this->life = life;
		this->killDistance = numeric_limits<int>::max();
		this->killDistanceCalculatedAt = numeric_limits<int>::max();
	}
	
	Enemy& operator=(Enemy other)
	{
		this->id = other.id;
		this->location = other.location;
		this->life = other.life;
		this->movement = other.movement;
		this->targetDatum = other.targetDatum;
		this->killDistance = other.killDistance;
		this->killDistanceCalculatedAt = other.killDistanceCalculatedAt;
	}
	
	void hit(int damage)
	{
		life -= damage;
	}
	
	bool dead()
	{
		return life <= 0;
	}
	
	bool onDatum()
	{
		return location == targetDatum.location;
	}
	
	void move()
	{
		location = moveLocation(location);
	}
	
	Coordinate moveLocation(Coordinate current)
	{
		int distance = current.distance(targetDatum.location);
//		cerr << "Enemy moveLocation. Current=" << current << " distance=" << distance << endl;
		if (distance <= movement)
		{
//			cerr << "Enemy moveLocation. Grabbed." << endl;
			return targetDatum.location;
		}
		else
		{
			float angle = current.angleTo(targetDatum.location);
//			cerr << "Enemy moveLocation. Current=" << current << " angle " << angle << " moveLocation " << Coordinate((int)round(movement * cos(angle) + current.x), (int)round(movement * sin(angle) + current.y)) << endl;
			return Coordinate((int)round(movement * cos(angle) + current.x), (int)round(movement * sin(angle) + current.y));
		}
	}
	
	float killShotDistance()
	{
		if (killDistanceCalculatedAt != life)
		{
			killDistance = pow(10, log10(DAMAGE_NUMERATOR/life)/DAMAGE_EXPONENT);
			killDistanceCalculatedAt = life;
		}

		return killDistance;
	}
	
	bool inShootToKillZone(Coordinate currentLocation)
	{
		cerr << "inShootToKillZone distance:" << moveLocation(location).distance(currentLocation) << "  killShotDistance():" << killShotDistance() << endl;
		return (moveLocation(location).distance(currentLocation) < killShotDistance());
	}
	
	bool willBeInShootToKillZone(Coordinate prospectiveLocation)
	{
		Coordinate nextLocation = moveLocation(location);
		return (moveLocation(nextLocation).distance(prospectiveLocation) < killShotDistance());
	}
	
	bool inDeadZone(Coordinate currentLocation)
	{
		return moveLocation(location).distance(currentLocation) <= DEAD_ZONE;
	}
	
	bool willBeInDeadZone(Coordinate currentLocation)
	{
		Coordinate nextLocation = moveLocation(location);
		return (moveLocation(nextLocation).distance(currentLocation) <= DEAD_ZONE);
	}
};

class Player
{
	public:
	Coordinate location;
	int movement;
	bool shooting;
	Enemy targetEnemy;
	Coordinate destination;
	
	Player()
	{
		movement = PLAYER_MOVE_RATE;
		shooting = false;
	}
	
	Player& operator=(Player other)
	{
		this->location = other.location;
		this->movement = other.movement;
		this->shooting = other.shooting;
		this->targetEnemy = other.targetEnemy;
		this->destination = other.destination;
	}

	void move(Coordinate _destination)
	{
		shooting = false;
		destination = _destination;

		int distance = location.distance(destination);
		if (distance <= movement)
		{
			location = destination;
		}
		else
		{
			float angle = location.angleTo(destination);
			location = Coordinate((int)round(movement * cos(angle) + location.x), (int)round(movement * sin(angle) + location.y));
		}
	}
	
	void shoot(Enemy enemy)
	{
		targetEnemy = enemy;
		shooting = true;
	}
	
	Coordinate randomMove()
	{
		float angle = randomRotation(randomEngine);
		float distance = randomDistance(randomEngine);
		return Coordinate((int)round(distance * cos(angle) + location.x), (int)round(distance * sin(angle) + location.y));
	}
};

class State
{
	public:
	Player player;
	map<int, Datum> data;
	map<int, Enemy> enemies;
	bool killed;
	bool terminal;
	int totalLifeAtStart;
	int shotsFired;
	bool initialized;
	clock_t turnStartTime;

	State()
	{
		initialized = false;
		totalLifeAtStart = 0;
		shotsFired = 0;
		clear();
	}

	State& operator=(State other)
	{
		this->player = other.player;
		for (map<int, Datum>::iterator it = other.data.begin(); it != other.data.end(); ++it)
		{
			data[it->first] = it->second;
		}
		
		for (map<int, Enemy>::iterator it = other.enemies.begin(); it != other.enemies.end(); ++it)
		{
			enemies[it->first] = it->second;
		}
		
		this->killed = other.killed;
		this->terminal = other.terminal;
		this->totalLifeAtStart = other.totalLifeAtStart;
		this->shotsFired = other.shotsFired;
		this->initialized = other.initialized;
	}
	
	void clear()
	{
		player.location = Coordinate();
		data.clear();
		enemies.clear();
		killed = false;
	}

	void updatePlayer(Coordinate location)
	{
		player.location = location;
	}
	
	void updateData(int id, Coordinate location)
	{
		data[id] = Datum(id, location);
	}
	
	void updateEnemy(int id, Coordinate location, int life)
	{
		enemies[id] = Enemy(id, location, life);
		int minDistance = numeric_limits<int>::max();
		for (map<int, Datum>::iterator it = data.begin(); it != data.end(); ++it)
		{
			int distance = location.distance((*it).second.location);

			if (minDistance > distance)
			{
				minDistance = distance;
				enemies[id].targetDatum = (*it).second;
			}
			else if (minDistance == distance)
			{
				if (enemies[id].targetDatum.id > (*it).first)
				{
					enemies[id].targetDatum = (*it).second;
				}
			}
		}
		
		if (!initialized)
		{
			totalLifeAtStart += life;
			player.targetEnemy = enemies[0];
		}
	}
	
	bool timeout()
	{
		return clock() > turnStartTime + (double)CLOCKS_PER_SEC * SIMULATION_DEADLINE;
	}
	
	int damage()
	{
		float distance = player.location.distance(enemies[player.targetEnemy.id].location);
		float damage = DAMAGE_NUMERATOR / pow(distance, DAMAGE_EXPONENT);
		return (int)round(damage);
	}
	
	void simulate()
	{
		int bestScore = numeric_limits<int>::min();
		int oldDistance = numeric_limits<int>::max();
		player.targetEnemy = enemies[0];
		State checkState;

		bool shootToKillFound = false;
		for (map<int, Enemy>::iterator it = checkState.enemies.begin(); it != checkState.enemies.end(); ++it)
		{
			if (/*!((*it).second.dead()) &&*/ (*it).second.inShootToKillZone(player.location))
			{
				shootToKillFound = true;
				checkState = simulate((*it).second);
				int checkScore = checkState.evaluate();
//				cerr << "shot score " << checkScore << " bestScore " << bestScore << endl;
				if (checkScore > bestScore)
				{
					cerr << "inShootToKillZone " << checkState.player.location << endl;
					bestScore = checkScore;
					player.targetEnemy = (*it).second;
					player.shooting = true;
				}
			}
		}

		if (!shootToKillFound)
		{
			int i = 0;
			bool killerMoveFound = false;
			while (!timeout())
			{
				i++;
				checkState = simulate(player.randomMove());
				bool killerMove = false;
				bool deadMove = false;
				int bestDistance = numeric_limits<int>::max();
				for (map<int, Enemy>::iterator it = checkState.enemies.begin(); it != checkState.enemies.end(); ++it)
				{
					if (!((*it).second.dead()))
					{
						int distance = checkState.player.destination.distance((*it).second.moveLocation((*it).second.location));
						if (bestDistance > distance)
						{
							bestDistance = distance;
							checkState.player.targetEnemy = (*it).second;
						}

						if ((*it).second.willBeInShootToKillZone(checkState.player.location))
						{
							killerMove = true;
		//					cerr << "willBeInShootToKillZone " << checkState.player.location << endl;
							for (map<int, Enemy>::iterator it2 = checkState.enemies.begin(); it2 != checkState.enemies.end(); ++it2)
							{
								if (!((*it).second.dead()) && ((*it2).second.inDeadZone(checkState.player.location) || (*it2).second.willBeInDeadZone(checkState.player.location)))
								{
		//							cerr << "inDeadZone || willBeInDeadZone " << checkState.player.location << endl;
									killerMove = false;
									deadMove = true;
									break;
								}
							}
						}
					}
				}
				
				if (killerMove)
				{
					killerMoveFound = true;
					int checkScore = checkState.evaluate();
					cerr << "killerMove score " << checkScore << " bestScore " << bestScore << endl;
					if (checkScore >= bestScore)
					{
						bestScore = checkScore;
						player.destination = checkState.player.destination;
						player.shooting = false;
						player.targetEnemy = checkState.player.targetEnemy;
					}
				}
				else if (!killerMoveFound && !deadMove)
				{
					if (bestDistance < oldDistance)
					{
						oldDistance = bestDistance;
						int checkScore = checkState.evaluate();
						cerr << "bestDistance score " << checkScore << " bestScore " << bestScore << endl;
						if (checkScore >= bestScore)
						{
							bestScore = checkScore;
							player.destination = checkState.player.destination;
							player.shooting = false;
							player.targetEnemy = checkState.player.targetEnemy;
						}
						cerr << "destination " << checkState.player.destination << " enemydestination " << checkState.player.targetEnemy.moveLocation(checkState.player.targetEnemy.location) << " bestDistance " << bestDistance << endl;
					}
				}
			}
			
			cerr << "iterations " << i << endl;
		}
	}
	
	State simulate(Enemy simEnemy)
	{
		State checkState = *this;
		checkState.player.shoot(simEnemy);
		return checkState;
	}

	State simulate(Coordinate simDestination)
	{
		State checkState = *this;
		checkState.player.move(simDestination);
		return checkState;
	}

	int evaluate()
	{
		terminal = false;

		if (killed)
		{
			terminal = true;
			return 0;
		}
		
		int total = 0;
		
		if (data.size() == 0)
		{
			terminal = true;
		}
		total += DATA_SAVED_POINTS * data.size();

		bool anyAlive = false;
		for (map<int, Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		{
			if ((*it).second.dead())
			{
				total += ENEMY_KILLED_POINTS;
			}
			else
			{
				anyAlive = true;
			}
		}
		
		if (!anyAlive)
		{
			terminal = true;
		}
		
		total += data.size() * max(0, (totalLifeAtStart - (3 * shotsFired))) * 3;
		
		return total;
	}

	void step()
	{
		// Enemies move
		for (map<int, Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		{
			(*it).second.move();
		}
		
		// Player moves
		if (!player.shooting)
		{
			player.move(player.destination);
		}
		
		// Enemies kill player
		for (map<int, Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		{
			if (player.location.distance((*it).second.location) < DEAD_ZONE)
			{
				killed = true;
			}
		}
		
		// Player shoots
		if (player.shooting)
		{
			enemies[player.targetEnemy.id].hit(damage());
			shotsFired++;
		}
		
		// Player kills enemy
		for (map<int, Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		{
			if ((*it).second.dead())
			{
				enemies.erase(it);
				break;
			}
		}

		// Enemies capture data
		for (map<int, Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		{
			if ((*it).second.onDatum())
			{
				data.erase((*it).second.targetDatum.id);
			}
		}
	}
	
	string printAction()
	{
		if (player.shooting)
		{
			return "SHOOT " + to_string(player.targetEnemy.id);
		}
		else
		{
			return "MOVE " + to_string(player.destination.x) + " " + to_string(player.destination.y);
		}
	}
};

/**
 * Shoot enemies before they collect all the incriminating data!
 * The closer you are to an enemy, the more damage you do but don't get too close or you'll get killed.
 **/
int main()
{
	State state;

    // game loop
    while (1)
	{
		state.clear();
        int x;
        int y;
        cin >> x >> y; cin.ignore();
		state.turnStartTime = clock();
		state.updatePlayer(Coordinate(x, y));
		
        int dataCount;
        cin >> dataCount; cin.ignore();
        for (int i = 0; i < dataCount; i++)
		{
            int dataId;
            int dataX;
            int dataY;
            cin >> dataId >> dataX >> dataY; cin.ignore();

			state.updateData(dataId, Coordinate(dataX, dataY));
		}
        int enemyCount;
        cin >> enemyCount; cin.ignore();
        for (int i = 0; i < enemyCount; i++)
		{
            int enemyId;
            int enemyX;
            int enemyY;
            int enemyLife;
            cin >> enemyId >> enemyX >> enemyY >> enemyLife; cin.ignore();

			state.updateEnemy(enemyId, Coordinate(enemyX, enemyY), enemyLife);
		}
		
		state.initialized = true;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

		state.simulate();
        cout << state.printAction() << endl; // MOVE x y or SHOOT id
    }
}