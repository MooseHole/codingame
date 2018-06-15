#include <iostream>
#include <string>
#include <vector>
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
	public:
	int id;
	Coordinate location;
	int param0;
	int param1;
	int param2;

	Entity(int _id, int _x, int _y, int _param0, int _param1, int _param2)
	{
		id = _id;
		location = Coordinate(_x, _y);
		param0 = _param0;
		param1 = _param1;
		param2 = _param2;
	}
	
	virtual string type() const = 0;
	virtual Coordinate nextLocation() const = 0;
};

vector<Entity*> entities;

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

	Coordinate nextLocation() const
	{
		return location;
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
	
	void checkNext(Coordinate check, Coordinate target, int &closest, Coordinate &next)
	{
		int distance = check.manhattan(target);
		if (distance < closest)
		{
			closest = distance;
			next = location;
		}
	}
	
	Coordinate getNextLocation()
	{
		if (target() < 0)
		{
			return location;
		}
		for (auto it = entities.begin(); it != entities.end(); ++it)
		{
			if ((*it)->id == target())
			{
				int closest = 999999;
				Coordinate next = location;

				Coordinate check = location;
				checkNext(check, (*it)->location, closest, next);
				check.x -= 1;
				checkNext(check, (*it)->location, closest, next);
				check.x += 2;
				checkNext(check, (*it)->location, closest, next);
				check = location;
				check.y -= 1;
				checkNext(check, (*it)->location, closest, next);
				check.y += 2;
				checkNext(check, (*it)->location, closest, next);
				return next;
			}
		}
		
		return location;
	}
	
	string type() const
	{
		return "WANDERER";
	}
};

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
        for (int i = 0; i < entityCount; i++) {
            string entityType;
            int id;
            int x;
            int y;
            int param0;
            int param1;
            int param2;
            cin >> entityType >> id >> x >> y >> param0 >> param1 >> param2; cin.ignore();
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << "WAIT" << endl; // MOVE <x> <y> | WAIT
    }
}