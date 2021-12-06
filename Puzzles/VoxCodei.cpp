#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>

#define ECHO_INPUT true

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

	Coordinate& operator=(const Coordinate& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	bool operator==(const Coordinate& other) const
	{
		return x == other.x && y == other.y;
	}

	Coordinate operator+(const Coordinate& other)
	{
		return Coordinate(x + other.x, y + other.y);
	}

	Coordinate operator*(const Coordinate& other)
	{
		return Coordinate(x * other.x, y * other.y);
	}

	Coordinate& operator+=(const Coordinate& other)
	{
		*this = *this + other;
		return *this;
	}

	Coordinate& operator*=(const Coordinate& other)
	{
		*this = *this * other;
		return *this;
	}

	bool operator <(const Coordinate& other) const
	{
		if (x < other.x)
		{
			return true;
		}

		if (x > other.x)
		{
			return false;
		}
		
		if (y < other.y)
		{
			return true;
		}

		return false;
	}

	bool inBounds(Coordinate corner)
	{
		return x >= 0 && y >= 0 && x <= corner.x && y <= corner.y;
	}

	string GetOutput()
	{
		return to_string(x) + " " + to_string(y);
	}

	friend ostream& operator<<(ostream& os, Coordinate const& m)
	{
		return os << "(" << m.x << "," << m.y << ")";
	}
};

class Bomb
{
public:
	Coordinate Location;
	int TurnExploding;

	Bomb() : Bomb(Coordinate(-10, -10), 0)
	{
	}

	Bomb(Coordinate location, int roundsRemaining) :
		Location(location),
		TurnExploding(roundsRemaining - 3)
	{
	}

	Bomb& operator=(const Bomb& other)
	{
		Location = other.Location;
		TurnExploding = other.TurnExploding;
		return *this;
	}
};

class Surveillance
{
public:
	Coordinate Location;

	Surveillance(Coordinate location) : Location(location)
	{
	}
};

class Passive
{
public:
	Coordinate Location;

	Passive(Coordinate location) : Location(location)
	{
	}
};

class Firewall
{
public:

	Firewall(int w, int h) : width(w), height(h)
	{
	}

	Firewall& operator=(const Firewall& other)
	{
		surveillanceNodes = other.surveillanceNodes;
		passiveNodes = other.passiveNodes;
		bombs = other.bombs;
		simulatorBombLocations = other.simulatorBombLocations;
		return *this;
	}

	void AddSurveillance(Coordinate location)
	{
		surveillanceNodes.insert(location);
	}

	void AddPassive(Coordinate location)
	{
		passiveNodes.insert(location);
	}

	void AddBomb(Coordinate location, int roundsRemaining)
	{
		bombs[location] = Bomb(location, roundsRemaining);
	}

	bool CanPlaceBomb(Coordinate location)
	{
		return !AnythingExists(location) && BombMightHitSomething(location);
	}

	void SetRound(int round)
	{
		vector<Coordinate> explodeNow;

		// Figure out what gets blown up
		for (const auto& bomb : bombs)
		{
			if (bomb.second.TurnExploding == round)
			{
				explodeNow.push_back(bomb.first);
			}
		}

		ExplodeBombs(explodeNow);
	}

	bool Simulate(int round, int bombsLeft)
	{
		SetRound(round);

		// This is the end of the game
		if (round == 1)
		{
			return surveillanceNodes.size() <= 0;
		}

		if (bombsLeft <= 0)
		{
			if (bombs.size() == 0)
			{
				return false;
			}

			return Simulate(round - 1, bombsLeft);
		}

		for (Coordinate location : simulatorBombLocations)
		{
			Firewall check = *this;
			check.AddBomb(location, round);

			if (check.Simulate(round - 1, bombsLeft - 1))
			{
				return true;
			}
		}

		Firewall checkWait = *this;
		if (checkWait.Simulate(round - 1, bombsLeft))
		{
			return true;
		}

		return false;
	}

	string Simulator(int round, int bombsLeft)
	{
		if (bombsLeft <= 0)
		{
			return "WAIT";
		}

		SetRound(round);

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				Coordinate location(x, y);

				if (CanPlaceBomb(location) && BombMightHitSomething(location))
				{
					simulatorBombLocations.insert(location);
				}
			}
		}

		// check each location, simulate next round, check done, check each location, simulate each round, check done, 
		for (Coordinate location : simulatorBombLocations)
		{
			Firewall check = *this;
			check.AddBomb(location, round);

			if (check.Simulate(round - 1, bombsLeft - 1))
			{
				AddBomb(location, round);
				return location.GetOutput();
			}
		}

		return "WAIT";
	}

private:
	set<Coordinate> surveillanceNodes;
	set<Coordinate> passiveNodes;
	map<Coordinate, Bomb> bombs;
	set<Coordinate> simulatorBombLocations;
	int width = 0;
	int height = 0;
	const vector<Coordinate> blastLocations{
		Coordinate(-1, 0),  Coordinate(-2, 0),  Coordinate(-3, 0),
		Coordinate(1, 0),  Coordinate(2, 0),  Coordinate(3, 0),
		Coordinate(0, -1),  Coordinate(0, -2),  Coordinate(0, -3),
		Coordinate(0, 1),  Coordinate(0, 2),  Coordinate(0, 3)
	};

	bool BombMightHitSomething(Coordinate Location)
	{
		vector<Coordinate> blast;
		for (Coordinate blastLocation : blastLocations)
		{
			if (SurveillanceExists(Location + blastLocation))
			{
				return true;
			}
		}

		return false;
	}

	bool ExplodeLocation(Coordinate location)
	{
		if (PassiveExists(location))
		{
			return false;
		}

		surveillanceNodes.erase(location);
		simulatorBombLocations.insert(location);
		return true;
	}

	vector<Coordinate> Explode(Coordinate location)
	{
		vector<Coordinate> otherBombs;

		// North
		for (int y = -1; y >= -3; y--)
		{
			if (!ExplodeLocation(Coordinate(location.x, location.y + y)))
			{
				break;
			}
		}

		// East
		for (int x = 1; x <= 3; x++)
		{
			if (!ExplodeLocation(Coordinate(location.x + x, location.y)))
			{
				break;
			}
		}

		// South
		for (int y = 1; y <= 3; y++)
		{
			if (!ExplodeLocation(Coordinate(location.x, location.y + y)))
			{
				break;
			}
		}

		// West
		for (int x = -1; x >= -3; x--)
		{
			if (!ExplodeLocation(Coordinate(location.x + x, location.y)))
			{
				break;
			}
		}

		return otherBombs;
	}

	void ExplodeBombs(vector<Coordinate> explodeNow)
	{
		if (explodeNow.size() < 1)
		{
			return;
		}

		vector<Coordinate> cascade;

		// Explode the bombs
		for (Coordinate bomb : explodeNow)
		{
			if (!BombExists(bomb))
			{
				continue;
			}

			vector<Coordinate> otherBombs = Explode(bomb);

			// Save any bombs that just got hit by the blast
			cascade.insert(cascade.end(), otherBombs.begin(), otherBombs.end());
		}

		// Remove all that just directly blew up
		for (Coordinate remove : explodeNow)
		{
			bombs.erase(remove);
		}

		ExplodeBombs(cascade);
	}

	bool PassiveExists(Coordinate location)
	{
		return passiveNodes.find(location) != passiveNodes.end();
	}

	bool SurveillanceExists(Coordinate location)
	{
		return surveillanceNodes.find(location) != surveillanceNodes.end();
	}

	bool BombExists(Coordinate location)
	{
		auto search = bombs.find(location);
		if (search == bombs.end())
		{
			return false;
		}

		return true;
	}

	bool AnythingExists(Coordinate location)
	{
		return BombExists(location) || SurveillanceExists(location) || PassiveExists(location);
	}
};

int main()
{
    int width; // width of the firewall grid
    int height; // height of the firewall grid
    cin >> width >> height; cin.ignore();
#ifdef ECHO_INPUT
	cerr << width << " " << height << endl;
#endif

	Firewall mainFirewall(width, height);
	
	for (int i = 0; i < height; i++)
	{
        string map_row;
        getline(cin, map_row); // one line of the firewall grid
#ifdef ECHO_INPUT
		cerr << map_row << endl;
#endif
		for (int j = 0; j < width; j++)
		{
			switch (map_row[j])
			{
			case '@':
				mainFirewall.AddSurveillance(Coordinate(j, i));
				break;
			case '#':
				mainFirewall.AddPassive(Coordinate(j, i));
				break;
			default:
				break;
			}
		}
    }

    // game loop
    while (1) {
        int rounds; // number of rounds left before the end of the game
        int bombs; // number of bombs left
        cin >> rounds >> bombs; cin.ignore();
#ifdef ECHO_INPUT
		cerr << rounds << " " << bombs << endl;
#endif
		cout << mainFirewall.Simulator(rounds, bombs) << endl;
    }
}