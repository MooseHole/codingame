#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <queue>

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

int width; // width of the firewall grid
int height; // height of the firewall grid

class Firewall
{
public:

	Firewall()
	{
	}

	Firewall& operator=(const Firewall& other)
	{
		surveillanceNodes = other.surveillanceNodes;
		passiveNodes = other.passiveNodes;
		bombs = other.bombs;
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
		return !AnythingExists(location) && BombScore(location);
	}

	vector<Coordinate> ExecuteRound(int round)
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

		newPotentialNodes.clear();

		ExplodeBombs(explodeNow);

		return newPotentialNodes;
	}

	int BombScore(Coordinate Location)
	{
		int score = 0;
		vector<Coordinate> blast;
		vector<Coordinate> blastLocations = GetBlastLocations(Location);
		for (Coordinate blastLocation : blastLocations)
		{
			if (SurveillanceExists(blastLocation))
			{
				score++;
			}
		}

		return score;
	}

	bool NoSurveillanceLeft() const
	{
		return surveillanceNodes.empty();
	}
	bool NoBombsLeft() const
	{
		return bombs.empty();
	}

private:
	set<Coordinate> surveillanceNodes;
	set<Coordinate> passiveNodes;
	map<Coordinate, Bomb> bombs;
	vector<Coordinate> newPotentialNodes;

	const vector<Coordinate> blastZone{
		Coordinate(-1, 0),  Coordinate(-2, 0),  Coordinate(-3, 0),
		Coordinate(1, 0),  Coordinate(2, 0),  Coordinate(3, 0),
		Coordinate(0, -1),  Coordinate(0, -2),  Coordinate(0, -3),
		Coordinate(0, 1),  Coordinate(0, 2),  Coordinate(0, 3)
	};

	vector<Coordinate> GetBlastLocations(Coordinate Location)
	{
		vector<Coordinate> blast;
		for (Coordinate blastLocation : blastZone)
		{
			blast.push_back(blastLocation + Location);
		}

		return blast;
	}

	// Returns false if explosion should not proceed farther
	bool ExplodeLocation(Coordinate location)
	{
		if (PassiveExists(location))
		{
			return false;
		}

		if (location.x < 0 || location.x >= width || location.y < 0 || location.y >= height)
		{
			return false;
		}

		if (surveillanceNodes.erase(location) > 0)
		{
			newPotentialNodes.push_back(location);
		}

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

Firewall mainFirewall;

class CompareBombScore
{
public:
	bool operator() (Coordinate first, Coordinate second)
	{
		return mainFirewall.BombScore(first) < mainFirewall.BombScore(second);
	}
};

class Simulator
{
private:
	bool SimulateRecurse(Firewall firewall, priority_queue<Coordinate, vector<Coordinate>, CompareBombScore> q, int round, int bombsLeft)
	{
		vector<Coordinate> newLocations = firewall.ExecuteRound(round);
		for (Coordinate newLocation : newLocations)
		{
			q.push(newLocation);
		}


		if (firewall.NoSurveillanceLeft())
		{
			// Cleared te board.  Pass.
			return true;
		}

		if (round == 0)
		{
			// Out of time.  Fail.
			return false;
		}

		if (bombsLeft <= 0)
		{
			if (firewall.NoBombsLeft())
			{
				// No bombs left to place, no bombs on board, but surveillance remains.  Fail.
				return false;
			}

			return SimulateRecurse(firewall, q, round - 1, bombsLeft);
		}

		auto& tempLocations = q;
		while (!tempLocations.empty())
		{
			Coordinate location = tempLocations.top();
			tempLocations.pop();
			Firewall check = firewall;
			check.AddBomb(location, round);

			if (SimulateRecurse(check, q, round - 1, bombsLeft - 1))
			{
				Outputs.push_back(location.GetOutput());
				return true;
			}
		}

		Firewall checkWait = firewall;
		if (SimulateRecurse(checkWait, q, round - 1, bombsLeft))
		{
			Outputs.push_back("WAIT");
			return true;
		}

		return false;
	}

public:
	vector<string> Outputs;

	void Simulate(int round, int bombsLeft)
	{
		if (bombsLeft <= 0)
		{
			Outputs.push_back("WAIT");
			return;
		}

		priority_queue<Coordinate, vector<Coordinate>, CompareBombScore> simulatorBombLocations;

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				Coordinate location(x, y);

				if (mainFirewall.CanPlaceBomb(location) && mainFirewall.BombScore(location))
				{
					simulatorBombLocations.push(location);
				}
			}
		}

		// check each location, simulate next round, check done, check each location, simulate each round, check done, 
		auto& tempLocations = simulatorBombLocations;
		while (!tempLocations.empty())
		{
			Coordinate location = tempLocations.top();
			tempLocations.pop();

			Firewall check = mainFirewall;
			check.AddBomb(location, round);

			if (SimulateRecurse(check, simulatorBombLocations, round - 1, bombsLeft - 1))
			{
				mainFirewall.AddBomb(location, round);
				Outputs.push_back(location.GetOutput());
				return;
			}
		}

		Outputs.push_back("WAIT");
		return;
	}
};

int main()
{
    cin >> width >> height; cin.ignore();
#ifdef ECHO_INPUT
	cerr << width << " " << height << endl;
#endif
	
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

	Simulator simulator;

	// Initial conditions
	int rounds; // number of rounds left before the end of the game
	int bombs; // number of bombs left
	cin >> rounds >> bombs; cin.ignore();
#ifdef ECHO_INPUT
	cerr << rounds << " " << bombs << endl;
#endif

	// Simulate the whole thing
	simulator.Simulate(rounds, bombs);
	std::reverse(simulator.Outputs.begin(), simulator.Outputs.end());

	int outputIndex = 0;
	cout << simulator.Outputs[outputIndex++] << endl;

	// game loop
    while (1) {
        int rounds; // number of rounds left before the end of the game
        int bombs; // number of bombs left
        cin >> rounds >> bombs; cin.ignore();
#ifdef ECHO_INPUT
		cerr << rounds << " " << bombs << endl;
#endif

		if (outputIndex >= simulator.Outputs.size())
		{
			cout << "WAIT" << endl;
		}
		else
		{
			cout << simulator.Outputs[outputIndex++] << endl;
		}
    }
}