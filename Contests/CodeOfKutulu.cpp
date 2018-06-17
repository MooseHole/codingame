#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

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
	}
	
	public:
	int id;
	Coordinate location;
	int param0;
	int param1;
	int param2;

	static Entity* Create(string type, int _id, int _x, int _y, int _param0, int _param1, int _param2);
	virtual string type() const = 0;
};

class Explorer : public Entity
{
	public:
	Explorer(int id, int x, int y, int param0, int param1, int param2) : Entity(id, x, y, param0, param1, param2)
	{
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
	
	Grid() {}

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

void updateNextLocation(Coordinate check, Coordinate target, int &closest, Coordinate &next)
{
	if (check.x < 0 || check.y < 0 || check.x >= grid.width || check.y >= grid.height)
	{
		return;
	}
	
	if (!grid.cells[check]->inhabitable())
	{
		return;
	}

	int distance = check.manhattan(target);
	if (distance < closest)
	{
		closest = distance;
		next = check;
	}
}

Coordinate getNextLocation(Wanderer* wanderer)
{
	if (wanderer->target() < 0)
	{
		return wanderer->location;
	}
	for (auto it = grid.entities.begin(); it != grid.entities.end(); ++it)
	{
		if ((*it)->id == wanderer->target())
		{
			int closest = 999999;
			Coordinate next = wanderer->location;

			Coordinate check = wanderer->location;
			updateNextLocation(check, (*it)->location, closest, next);
			check.x -= 1;
			updateNextLocation(check, (*it)->location, closest, next);
			check.x += 2;
			updateNextLocation(check, (*it)->location, closest, next);
			check = wanderer->location;
			check.y -= 1;
			updateNextLocation(check, (*it)->location, closest, next);
			check.y += 2;
			updateNextLocation(check, (*it)->location, closest, next);
			return next;
		}
	}
	
	return wanderer->location;
}

Coordinate getNextLocation(Explorer* explorer)
{
	return explorer->location;
}

Coordinate getNextLocation(Entity* entity)
{
	if (entity->type().compare("EXPLORER") == 0)
	{
		return getNextLocation(static_cast<Explorer*>(entity));
	}
	if (entity->type().compare("WANDERER") == 0)
	{
		return getNextLocation(static_cast<Wanderer*>(entity));
	}
	return entity->location;
}

void printLocations(Grid thisGrid)
{
	for (auto entity = thisGrid.entities.begin(); entity != thisGrid.entities.end(); ++entity)
	{
		cerr << "[" << (*entity)->id << "] at " << (*entity)->location << " to " << getNextLocation(*entity) << endl;
	}
}

void simulate()
{
	Grid sim = grid;
	for (auto it = sim.entities.begin(); it != sim.entities.end(); ++it)
	{
		(*it)->location = getNextLocation(*it);
	}
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

    // game loop
    while (1) {
        int entityCount; // the first given entity corresponds to your explorer
        cin >> entityCount; cin.ignore();
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

        cout << "WAIT" << endl; // MOVE <x> <y> | WAIT
    }
}