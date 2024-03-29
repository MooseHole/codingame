#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>

using namespace std;

#define SIMULATION_DEADLINE 0.030

enum Direction {DIRECTION_NONE = 0, DIRECTION_UP, DIRECTION_DOWN, DIRECTION_LEFT, DIRECTION_RIGHT, DIRECTION_FINAL_ELEMENT};
static minstd_rand randomEngine(clock());
static uniform_int_distribution<int> randomMove{0, DIRECTION_FINAL_ELEMENT-1};
clock_t turnStartTime;

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
	
	Coordinate(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
	
	Coordinate& operator=(Coordinate other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}
	
    bool operator<(const Coordinate& other) const
    {
        return index() < other.index();
    }
	
    bool operator==(const Coordinate& other) const
    {
        return x == other.x && y == other.y;
    }
	
    bool operator!=(const Coordinate& other) const
    {
		return !(*this == other);
    }
	
	int manhattan(const Coordinate& other) const
	{
		return abs(x - other.x) + abs(y - other.y);
	}
	
	int index() const
	{
		return 10000 + x * 100 + y;
	}
	
	Coordinate move(Direction direction)
	{
		Coordinate returnCoordinate;
		returnCoordinate.x = x;
		returnCoordinate.y = y;

		switch(direction)
		{
			case DIRECTION_UP:
				returnCoordinate.y--;
				break;
			case DIRECTION_DOWN:
				returnCoordinate.y++;
				break;
			case DIRECTION_LEFT:
				returnCoordinate.x--;
				break;
			case DIRECTION_RIGHT:
				returnCoordinate.x++;
				break;
			default:
				break;
		}

		return returnCoordinate;
	}

	friend ostream &operator<<(ostream &os, Coordinate const &m)
	{
		return os << "(" << m.x << "," << m.y << ")";
	}
};

class Entity
{
	protected:
	Entity(int _id, int _x, int _y, int _param0, int _param1, int _param2)
	{
		id = _id;
		location = Coordinate(_x, _y);
		param0 = _param0;
		param1 = _param1;
		param2 = _param2;
		dead = false;
	}
	
	public:
	int id;
	Coordinate location;
	int param0;
	int param1;
	int param2;
	bool dead;

	static Entity* Create(string type, int _id, int _x, int _y, int _param0, int _param1, int _param2);
	virtual string type() const = 0;
};

class Explorer : public Entity
{
	public:
	int distanceFromExplorer;

	Explorer(int id, int x, int y, int param0, int param1, int param2) : Entity(id, x, y, param0, param1, param2)
	{
	}
	
	void removeSanity(int amount)
	{
		param0 -= amount;
	}
	
	int sanity() const
	{
		return param0;
	}
	
	bool isSelf() const
	{
		return id == 0;
	}

	string type() const
	{
		return "EXPLORER";
	}
};

class Wanderer : public Entity
{
	public:
	Wanderer(int id, int x, int y, int param0, int param1, int param2) : Entity(id, x, y, param0, param1, param2)
	{
	}
	
	enum class State { SPAWNING = 0, WANDERING = 1 };
	
	// Number of turns before the spawn happens.  0 if already wandering
	int timeBeforeSpawn() const
	{
		if (state() == State::SPAWNING)
		{
			return param0;
		}
		
		return 0;
	}
	
	// SPAWNING or WANDERING
	State state() const
	{
		return static_cast<State>(param1);
	}
	
	// Returns the id of the target Explorer.  -1 if none (only on spawn)
	int target() const
	{
		return param2;
	}
	
	string type() const
	{
		return "WANDERER";
	}
};

Entity* Entity::Create(string type, int _id, int _x, int _y, int _param0, int _param1, int _param2)
{
	if (type.compare("EXPLORER") == 0)
	{
		return new Explorer(_id, _x, _y, _param0, _param1, _param2);
	}

	if (type.compare("WANDERER") == 0)
	{
		return new Wanderer(_id, _x, _y, _param0, _param1, _param2);
	}

	return nullptr;
}

class Cell
{
	public:
	static Cell* Create(char type);
	virtual bool inhabitable() const = 0;
	virtual char print() const = 0;
};

class Wall : public Cell
{
	bool inhabitable() const
	{
		return false;
	}
	
	char print() const
	{
		return '#';
	}
};

class Spawn : public Cell
{
	bool inhabitable() const
	{
		return true;
	}

	char print() const
	{
		return 'w';
	}
};

class Free : public Cell
{
	bool inhabitable() const
	{
		return true;
	}

	char print() const
	{
		return '.';
	}
};

Cell* Cell::Create(char type)
{
	switch (type)
	{
		case '#':
			return new Wall();
		case 'w':
			return new Spawn();
		case '.':
		default:
			return new Free();
	}
}

class Grid
{
	public:
	map<Coordinate, Cell*> cells;
	vector<Entity*> entities;
	int width;
	int height;
	int sanityLossLonely, sanityLossGroup, wandererSpawnTime, wandererLifeTime;
	
	Grid() {}

	~Grid()
	{
		for (auto it = cells.begin(); it != cells.end();)
		{
			delete((*it).second);
			cells.erase(it++);
		}
		cells.clear();
		for (auto it = entities.begin(); it != entities.end(); ++it)
		{
			delete(*it);
		}
		entities.clear();
	}

	Grid(const Grid &other)
	{
		width = other.width;
		height = other.height;

		// Deep copy the pointers
		for (auto it = other.cells.begin(); it != other.cells.end(); ++it)
		{
			addCell(it->first, it->second);
		}
		for (auto it = other.entities.begin(); it != other.entities.end(); ++it)
		{
			addEntity(*it);
		}
	}
	
	void addCell(Coordinate coordinate, Cell* cell)
	{
		cells[coordinate] = Cell::Create(cell->print());
	}

	void addRow(int row, int _width, string line)
	{
		width = _width;
		height = row; // WARNING! Assumes addRow is called consecutively and in order
		Coordinate current(0, row);
		for (auto i = 0; i < width; ++i)
		{
			current.x = i;
			cells[current] = Cell::Create(line[i]);
		}
	}

	void addSettings(int _sanityLossLonely, int _sanityLossGroup, int _wandererSpawnTime, int _wandererLifeTime)
	{
		sanityLossLonely = _sanityLossLonely;
		sanityLossGroup = _sanityLossGroup;
		wandererSpawnTime = _wandererSpawnTime;
		wandererLifeTime = _wandererLifeTime;
	}
	
	bool inBoundaries(Coordinate location)
	{
		return location.x >= 0 && location.y >= 0 && location.x < width && location.y < height;
	}
	
	Coordinate move(Coordinate current, Direction direction)
	{
		return current.move(direction);
	}

	bool canMove(Coordinate current, Direction direction)
	{
		Coordinate newLocation = current.move(direction);

		if (!inBoundaries(newLocation))
		{
			return false;
		}

		return cells[newLocation]->inhabitable();
	}
	
	void clearEntities()
	{
		for (auto it = entities.begin(); it != entities.end(); ++it)
		{
			delete (*it);
		}
		
		entities.clear();
	}

	void addEntity(Entity* entity)
	{
		entities.push_back(Entity::Create(entity->type(), entity->id, entity->location.x, entity->location.y, entity->param0, entity->param1, entity->param2));
	}

	void addEntity(string type, int _id, int _x, int _y, int _param0, int _param1, int _param2)
	{
		entities.push_back(Entity::Create(type, _id, _x, _y, _param0, _param1, _param2));
	}
	
	int score() const
	{
		int score = 0;
		score += static_cast<Explorer*>(entities[0])->sanity() * 10;
		score -= static_cast<Explorer*>(entities[1])->sanity();
		score -= static_cast<Explorer*>(entities[2])->sanity();
		score -= static_cast<Explorer*>(entities[3])->sanity();
		return score;
	}
	
	// Process sanity losses
	void step()
	{
		for (int i = 0; i < 4; i++)
		{
			if (entities[i]->type().compare("EXPLORER") == 0)
			{
				static_cast<Explorer*>(entities[i])->distanceFromExplorer = 99999999;
			}
			else
			{
				// Explorers are all at the beginning
				break;
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (entities[i]->type().compare("EXPLORER") == 0)
			{
				for (int j = i+1; j < 4; j++)
				{
					if (entities[j]->type().compare("EXPLORER") == 0)
					{
						int thisDistance = static_cast<Explorer*>(entities[i])->location.manhattan(static_cast<Explorer*>(entities[j])->location);
						if (thisDistance < static_cast<Explorer*>(entities[i])->distanceFromExplorer)
						{
							static_cast<Explorer*>(entities[i])->distanceFromExplorer = thisDistance;
						}
						if (thisDistance < static_cast<Explorer*>(entities[j])->distanceFromExplorer)
						{
							static_cast<Explorer*>(entities[j])->distanceFromExplorer = thisDistance;
						}
					}
				}
				
				for (auto entity = entities.begin(); entity != entities.end(); ++entity)
				{
					if (entities[i]->location == (*entity)->location)
					{
						if ((*entity)->type().compare("WANDERER") == 0)
						{
							static_cast<Explorer*>(entities[i])->removeSanity(20);
							(*entity)->dead = true;
						}
					}
				}
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (entities[i]->type().compare("EXPLORER") == 0)
			{
				if (static_cast<Explorer*>(entities[i])->distanceFromExplorer <= 2)
				{
					static_cast<Explorer*>(entities[i])->removeSanity(sanityLossGroup);
				}
				else
				{
					static_cast<Explorer*>(entities[i])->removeSanity(sanityLossLonely);
				}
				
				if (static_cast<Explorer*>(entities[i])->sanity() <= 0)
				{
					entities[i]->dead = true;
				}
			}
		}
		
		for (auto entity = entities.begin(); entity != entities.end();)
		{
			if ((*entity)->dead)
			{
				entity = entities.erase(entity);
			}
			else
			{
				++entity;
			}
		}
	}
	
	friend ostream &operator<<(ostream &os, Grid const &m)
	{
		os << ' ';
		int column = 0;
		for (column = 0; column < m.width; ++column)
		{
			os << column;
		}

		int row = 0;
		for (auto cell = m.cells.begin(); cell != m.cells.end(); ++cell)
		{
			if (column >= m.width)
			{
				os << endl;
				column = 0;
				os << row++;
			}

			os << cell->second->print();
			column++;
		}

		return os;
	}
};

Grid grid;

void updateNextLocation(Coordinate current, Direction direction, Coordinate target, int &closest, Coordinate &next)
{
	if (!grid.canMove(current, direction))
	{
		return;
	}

	Coordinate check = grid.move(current, direction);
	int distance = check.manhattan(target);
	if (distance < closest)
	{
		closest = distance;
		next = check;
	}
}

Coordinate getNextLocation(Grid thisGrid, Wanderer* wanderer)
{
	if (wanderer->target() < 0)
	{
		return wanderer->location;
	}
	for (auto it = thisGrid.entities.begin(); it != thisGrid.entities.end(); ++it)
	{
		if ((*it)->id == wanderer->target())
		{
			int closest = 999999;
			Coordinate current = wanderer->location;
			Coordinate next = wanderer->location;
			updateNextLocation(current, DIRECTION_NONE,  (*it)->location, closest, next);
			updateNextLocation(current, DIRECTION_UP,    (*it)->location, closest, next);
			updateNextLocation(current, DIRECTION_RIGHT, (*it)->location, closest, next);
			updateNextLocation(current, DIRECTION_DOWN,  (*it)->location, closest, next);
			updateNextLocation(current, DIRECTION_LEFT,  (*it)->location, closest, next);
			return next;
		}
	}
	
	return wanderer->location;
}

Coordinate getNextLocation(Grid thisGrid, Explorer* explorer)
{
	Direction direction = static_cast<Direction>(randomMove(randomEngine));
	if (thisGrid.canMove(explorer->location, direction))
	{
		return explorer->location.move(direction);
	}

	return explorer->location;
}

Coordinate getNextLocation(Grid thisGrid, Entity* entity)
{
	if (entity->type().compare("EXPLORER") == 0)
	{
		return getNextLocation(thisGrid, static_cast<Explorer*>(entity));
	}
	if (entity->type().compare("WANDERER") == 0)
	{
		return getNextLocation(thisGrid, static_cast<Wanderer*>(entity));
	}
	return entity->location;
}

void printLocations(Grid thisGrid)
{
	for (auto entity = thisGrid.entities.begin(); entity != thisGrid.entities.end(); ++entity)
	{
		cerr << "[" << (*entity)->id << "] at " << (*entity)->location << " to " << getNextLocation(thisGrid, *entity) << endl;
	}
}

Coordinate simulate()
{
	int numSteps = 10;
	int numTrials = 10000000;
	int bestScore = -99999999;
	Coordinate bestMove = grid.entities[0]->location;

	for (auto trial = 0; trial < numTrials; ++trial)
	{
		if (clock() > turnStartTime + (double)CLOCKS_PER_SEC * SIMULATION_DEADLINE)
		{
			cerr << "Timeout at trial " << trial << endl;
			break;
		}

		Grid sim = grid;
		bool thisIsFirstMove = true;
		Coordinate firstMove;
		for (auto step = 0; step < numSteps; ++step)
		{
			for (auto entity = sim.entities.begin(); entity != sim.entities.end(); ++entity)
			{
				(*entity)->location = getNextLocation(sim, *entity);
				if (thisIsFirstMove)
				{
					firstMove = (*entity)->location;
					thisIsFirstMove = false;
				}
			}
			sim.step();
		}
		int thisScore = sim.score();
		if (thisScore > bestScore)
		{
			bestScore = thisScore;
			bestMove = firstMove;
		}
	}
	
	return bestMove;
}

/**
 * Survive the wrath of Kutulu
 * Coded fearlessly by JohnnyYuge & nmahoude (ok we might have been a bit scared by the old god...but don't say anything)
 **/
int main()
{
    int width;
    cin >> width; cin.ignore();
    int height;
    cin >> height; cin.ignore();
    for (int i = 0; i < height; i++) {
        string line;
        getline(cin, line);
		grid.addRow(i, width, line);
    }
    int sanityLossLonely; // how much sanity you lose every turn when alone, always 3 until wood 1
    int sanityLossGroup; // how much sanity you lose every turn when near another player, always 1 until wood 1
    int wandererSpawnTime; // how many turns the wanderer take to spawn, always 3 until wood 1
    int wandererLifeTime; // how many turns the wanderer is on map after spawning, always 40 until wood 1
    cin >> sanityLossLonely >> sanityLossGroup >> wandererSpawnTime >> wandererLifeTime; cin.ignore();
	grid.addSettings(sanityLossLonely, sanityLossGroup, wandererSpawnTime, wandererLifeTime);

    // game loop
    while (1) {
        int entityCount; // the first given entity corresponds to your explorer
        cin >> entityCount; cin.ignore();
		turnStartTime = clock();
		grid.clearEntities();
        for (int i = 0; i < entityCount; i++) {
            string entityType;
            int id;
            int x;
            int y;
            int param0;
            int param1;
            int param2;
            cin >> entityType >> id >> x >> y >> param0 >> param1 >> param2; cin.ignore();
			grid.addEntity(entityType, id, x, y, param0, param1, param2);
        }

//		cerr << grid << endl;
//		printLocations(grid);
//		simulate();
        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

//        cout << "WAIT" << endl; // MOVE <x> <y> | WAIT
		Coordinate bestMove = simulate();
		cout << "MOVE " << bestMove.x << " " << bestMove.y << endl;
    }
}