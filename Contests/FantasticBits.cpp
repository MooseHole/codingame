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
	
	Coordinate (int _x, int _y)
	{
		x = _x;
		y = _y;
	}
	
	float distance(Coordinate other)
	{
		Math.sqrt(distanceSquared(other));
	}

	float distanceSquared(Coordinate other)
	{
		Math.pow(other.x - x, 2) + Math.pow(other.y - y, 2);
	}
}

class Vector
{
	public:
	int x;
	int y;

	Vector (int _x, int _y)
	{
		x = _x;
		y = _y;
	}
}

class Entity
{
	public:
	int id;
	string type;
	Coordinate position;
	Vector velocity;
	int state;
	int radius;
	bool exists;
	
	Entity(int _id, string _type, int x, int y, int vx, int vy, int _state, int _radius = 0)
	{
		id = _id;
		type = _type;
		position = Coordinate(x, y);
		velocity = Vector(x, y);
		state = _state;
		radius = _radius;
		exists = true;
	}
	
    bool operator==(const Entity& other)
    {
        return id == other.id;
    }
	
	bool Collision(Entity other)
	{
		return (position.distance(other.position) <= (radius + other.radius))
	}
	
	void setExistence()
	{
		exists = true;
	}
	
	virtual void resetExistence()
	{
		exists = false;
	}
};

class Wizard : public Entity
{
	public:
	int goal;

	Wizard(int _id, int x, int y, int vx, int vy, int _state, int _goal)
	{
		id = _id;
		type = "WIZARD";
		position = Coordinate(x, y);
		velocity = Vector(x, y);
		state = _state;
		radius = 400;
		exists = true;
		
		goal = _goal;
	}
	
	bool HasSnaffle()
	{
		return state == 1;
	}
};

class Snaffle : public Entity
{
	public:
	Snaffle(int _id, int x, int y, int vx, int vy)
	{
		id = _id;
		type = "SNAFFLE";
		position = Coordinate(x, y);
		velocity = Vector(x, y);
		state = 0;
		radius = 150;
		exists = true;
	}
};

class Pole : public Entity
{
	public:
	Pole(int x, int y)
	{
		id = -1;
		type = "POLE";
		position = Coordinate(x, y);
		velocity = Vector(0, 0);
		state = 0;
		radius = 300;
		exists = true;
	}
	
	// Always exists
	virtual void resetExistence()
	{
	}
};

class Field
{
	Coordinate edge;
	
	public:
	Field(int xMax, int yMax)
	{
		edge = Coordinate(xMax, yMax);
	}
	
	bool InBounds(Coordinate check)
	{
		return check.x >= 0 && check.y >= 0 && check.x <= edge.x && check.y <= edge.y;
	}
	
	int Goal(Entity entity)
	{
		if (entity.position.y - entity.radius >= 1750 && entity.position.y - entity.radius <= 5750)
		{
			if (Math.abs(entity.position.x - 16000) <= entity.radius)
			{
				return 1;
			}
			else if (Math.abs(entity.position.x - 0) <= entity.radius)
			{
				return 0;
			}
		}
		
		return -1;
	}
}

/**
 * Grab Snaffles and try to throw them through the opponent's goal!
 * Move towards a Snaffle and use your team id to determine where you need to throw it.
 **/
int main()
{
    int myTeamId; // if 0 you need to score on the right of the map, if 1 you need to score on the left
    cin >> myTeamId; cin.ignore();
	
	vector<Entity> entities;
	
	Field field = Field(16000, 7500);

	entities.push_back(Pole(    0, 1750));
	entities.push_back(Pole(    0, 5750));
	entities.push_back(Pole(16000, 1750));
	entities.push_back(Pole(16000, 5750));
	bool initialRun = true;

    // game loop
    while (1) {
        int numEntities; // number of entities still in game
        cin >> numEntities; cin.ignore();
		
		for (vector<Entity>::iterator it = entities.begin(); it != entities.end(); ++it)
		{
			(*it).resetExistence();
		}
		
        for (int i = 0; i < numEntities; i++)
		{
            int entityId; // entity identifier
            string entityType; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
            int x; // position
            int y; // position
            int vx; // velocity
            int vy; // velocity
            int state; // 1 if the wizard is holding a Snaffle, 0 otherwise
            cin >> entityId >> entityType >> x >> y >> vx >> vy >> state; cin.ignore();

			vector<Entity>::iterator it = find (entities.begin(), entities.end(), entityId);
			if (it != entities.end())
			{
				(*it).position = Coordinate(x, y);
				(*it).velocity = Vector(vx, vy);
				(*it).state = state;
			}
			else
			{
				switch (entityType)
				{
					case "SNAFFLE":
						entities.push_back(Snaffle(entityId, x, y, vx, vy));
						break;
					case "WIZARD":
						entities.push_back(Wizard(entityId, x, y, vx, vy, state, myTeamId));
						break;
						break;
					case "OPPONENT_WIZARD":
						entities.push_back(Wizard(entityId, x, y, vx, vy, state, myTeamId == 1 ? 0 : 1));
						break;
				}
			}
        }

        for (int i = 0; i < 2; i++) {

            // Write an action using cout. DON'T FORGET THE "<< endl"
            // To debug: cerr << "Debug messages..." << endl;


            // Edit this line to indicate the action for each wizard (0 <= thrust <= 150, 0 <= power <= 500)
            // i.e.: "MOVE x y thrust" or "THROW x y power"
            cout << "MOVE 8000 3750 100" << endl;
        }
    }
}