#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

class Coordinate
{
    public:
    int x;
    int y;
    
    Coordinate()
    {}
    
    Coordinate(int x, int y) : x(x), y(y)
    {}
    
    Coordinate& operator=(const Coordinate& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }
    
    bool operator==(const Coordinate& other)
    {
        return x == other.x && y == other.y;
    }

    int distance(Coordinate to)
    {
        int distance = 0;
        distance += abs(to.x - x);
        distance += abs(to.y - y);
        return distance;
		
		return((int)rint(sqrt(pow((double)(to.x - x), 2.0) + pow((double)(to.y - y), 2.0))));
	}

    friend ostream &operator<<(ostream &os, Coordinate const &m)
    {
        return os << "(" << m.x << "," << m.y << ")";
    }        
};

class Zone
{
    public:
    Coordinate location;
    int owner;
    int id;
    
    Zone() : location(Coordinate(-1, -1))
    {
	}
    
    Zone(int id, int owner = -1) : id(id), owner(owner)
    {
        Zone();
    }
    
    Zone(Coordinate location, int _id = -1, int _owner = -1) : location(location)
    {
        Zone(_id, _owner);
    }
	
    friend ostream &operator<<(ostream &os, Zone const &m)
    {
        return os << "Zone " << m.id << ": " << m.location << m.owner;
    }
};

class Drone
{
    public:
    Coordinate location;
    int id;
    int owner;
    Coordinate destination;
	Zone* zone;
    
    Drone()
	{
		zone = NULL;
	}
    
    Drone(int id, int owner = -1) : id(id), owner(owner)
    {
        Drone();
    }
    
    Drone(Coordinate _location, int _id = -1, int _owner = -1)
    {
        Drone(_id, _owner);
        location = _location;
    }
    
    bool inZone(Zone* zone)
    {
        return zone->location.distance(location) <= 100;
    }
 
    int turnsToZone(Zone* zone)
    {
        return zone->location.distance(location) / 100;
    }
    
    Coordinate closestZone(vector<Zone> zones)
    {
        int minDistance = 10000000;
        Coordinate closeLocation;
        for (vector<Zone>::iterator zone = zones.begin(); zone != zones.end(); ++zone)
        {
            int distance = (*zone).location.distance(location);
            if (distance < minDistance)
            {
                minDistance = distance;
                closeLocation = (*zone).location;
            }            
        }
        
        return closeLocation;
    }

    void move()
    {
        cout << destination.x << " " << destination.y << endl;
    }

    friend ostream &operator<<(ostream &os, Drone const &m)
    {
        return os << "Drone " << m.id << ": " << m.location << m.owner;
    }        
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int P; // number of players in the game (2 to 4 players)
    int ID; // ID of your player (0, 1, 2, or 3)
    int D; // number of drones in each team (3 to 11)
    int Z; // number of zones on the map (4 to 8)
    cin >> P >> ID >> D >> Z; cin.ignore();

    vector<Zone> zones;
    for (int id = 0; id < Z; id++) {
        int X; // corresponds to the position of the center of a zone. A zone is a circle with a radius of 100 units.
        int Y;
        cin >> X >> Y; cin.ignore();
        zones.push_back(Zone(Coordinate(X, Y), id));
    }

    vector<Drone> drones;
    for (int owner = 0; owner < P; owner++)
    {
        for (int id = 0; id < D; id++)
        {
            drones.push_back(Drone(id, owner));
        }
    }
    
    // game loop
    while (1) {
        for (int i = 0; i < Z; i++) {
            int TID; // ID of the team controlling the zone (0, 1, 2, or 3) or -1 if it is not controlled. The zones are given in the same order as in the initialization.
            cin >> TID; cin.ignore();
        }

        for (vector<Drone>::iterator drone = drones.begin(); drone != drones.end(); ++drone)
        {
            int DX; // The first D lines contain the coordinates of drones of a player with the ID 0, the following D lines those of the drones of player 1, and thus it continues until the last player.
            int DY;
            cin >> DX >> DY; cin.ignore();
            
            (*drone).location = Coordinate(DX, DY);
			(*drone).zone = NULL;
			for (vector<Zone>::iterator zone = zones.begin(); zone != zones.end(); ++zone)
			{
				if ((*drone).inZone(zone))
				{
					(*drone).zone = (zone);
				}
			}

            (*drone).destination = (*drone).closestZone(zones);

            if ((*drone).owner == ID)
            {
                (*drone).move();
            }
        }
    }
}