#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define BOMB_DETONATION_TIME	3
#define BOMB_RANGE	3
#define SIMULATION_TIME 0.095

enum Direction
{
	UNKNOWN,
	VERTICAL,
	HORIZONTAL,
	NONE
};

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

	bool inBounds(Coordinate corner)
	{
		return x >= 0 && y >= 0 && x <= corner.x && y <= corner.y;
	}

	friend ostream &operator<<(ostream &os, Coordinate const &m)
	{
		return os << "(" << m.x << "," << m.y << ")";
	}
};

static minstd_rand randomEngine(clock());
class Grid;
class Bomb;
class Node;
Coordinate floorCorner;
vector<vector<char>> nodeFloor;
double turnStartTime;

class Movement
{
	public:
	Coordinate location;
	Direction direction;
	Coordinate movement;
	Coordinate nextLocation;
	bool possible;
	
	Movement(){}
	
	Movement(Coordinate _location, Direction _direction, Coordinate _movement)
	{
		location = _location;
		direction = _direction;
		movement = _movement;
		nextLocation = location + movement;
		possible = nextLocation.inBounds(floorCorner) && nodeFloor[nextLocation.x][nextLocation.y] != '#';
		updateNextLocation();
	}
	
	Movement& operator=(const Movement& other)
	{
		location = other.location;
		direction = other.direction;
		movement = other.movement;
		nextLocation = other.nextLocation;
		possible = other.possible;
	}
	
	void updateNextLocation()
	{
		if (possible)
		{
			nextLocation = location + movement;
			if (!nextLocation.inBounds(floorCorner) || nodeFloor[nextLocation.x][nextLocation.y] == '#')
			{
				movement *= Coordinate(-1, -1);	// Flip direction
				nextLocation = location + movement;
			}
		}
	}

	void tick(bool compare = true)
	{
		if (possible)
		{
			location = nextLocation;
	
			if (compare && nodeFloor[nextLocation.x][nextLocation.y] != '@')
			{
				possible = false;
			}

			updateNextLocation();
		}		
	}
};

class Node
{
	public:
	bool destroyed;
	Movement movementUp, movementDown, movementLeft, movementRight, movementStationary;
	Movement movementConfirmed;
	bool firstTurn;
	Coordinate previousLocation;
	
	Node(){}
	
	Node(int x, int y)
	{
		destroyed = false;
		Coordinate location = Coordinate(x, y);
		movementUp = Movement(location, Direction::VERTICAL, Coordinate(0, -1));
		movementDown = Movement(location, Direction::VERTICAL, Coordinate(0, 1));
		movementLeft = Movement(location, Direction::HORIZONTAL, Coordinate(-1, 0));
		movementRight = Movement(location, Direction::HORIZONTAL, Coordinate(1, 0));
		movementStationary = Movement(location, Direction::NONE, Coordinate(0, 0));
		movementConfirmed.possible = false;
		previousLocation = location;
		firstTurn = true;
	}
	
	Node& operator=(const Node& other)
	{
		destroyed = other.destroyed;
		movementUp = other.movementUp;
		movementDown = other.movementDown;
		movementLeft = other.movementLeft;
		movementRight = other.movementRight;
		movementStationary = other.movementStationary;
		movementConfirmed = other.movementConfirmed;
		firstTurn = other.firstTurn;
		previousLocation = other.previousLocation;
	}
	
	bool isDirectionKnown() const
	{
		return movementConfirmed.possible;
	}
	
	Coordinate location() const
	{
		if (movementConfirmed.possible)
		{
			return movementConfirmed.location;
		}
		
		return Coordinate(-1,-1);
	}
	
	Coordinate nextLocation() const
	{
		if (movementConfirmed.possible)
		{
			return movementConfirmed.nextLocation;
		}
		
		return Coordinate(-1,-1);
	}
	
	Coordinate prevLocation() const
	{
		if (movementConfirmed.possible)
		{
			return previousLocation;
		}
		
		return Coordinate(-1,-1);
	}
	
	bool tick()
	{
		if (firstTurn)
		{
			firstTurn = false;
			return false;
		}

		if (movementConfirmed.possible)
		{
			previousLocation = location();
			movementConfirmed.tick(false);
		}
		else
		{
			movementUp.tick();
			movementDown.tick();
			movementLeft.tick();
			movementRight.tick();
			movementStationary.tick();

			int possibleDirections = 0;
			
			if (movementUp.possible)
			{
				movementConfirmed = movementUp;
				possibleDirections++;
			}
			
			if (movementDown.possible)
			{
				movementConfirmed = movementDown;
				possibleDirections++;
			}
			
			if (movementLeft.possible)
			{
				movementConfirmed = movementLeft;
				possibleDirections++;
			}
			
			if (movementRight.possible)
			{
				movementConfirmed = movementRight;
				possibleDirections++;
			}
			
			if (movementStationary.possible)
			{
				movementConfirmed = movementStationary;
				possibleDirections++;
			}

			movementConfirmed.possible = (possibleDirections == 1);
		}

		firstTurn = false;
	}
	
	string directionString() const
	{
		if (!movementConfirmed.possible)
		{
			return "?";
		}
		else if (movementConfirmed.movement == Coordinate(-1, 0))
		{
			return "<";
		}
		else if (movementConfirmed.movement == Coordinate(1, 0))
		{
			return ">";
		}
		else if (movementConfirmed.movement == Coordinate(0, -1))
		{
			return "^";
		}
		else if (movementConfirmed.movement == Coordinate(0, 1))
		{
			return "v";
		}
		else
		{
			return "+";
		}
	}

	friend ostream &operator<<(ostream &os, Node const &m)
	{
		return os << "[" << m.location() << m.directionString() << (m.destroyed ? "." : "@") << (m.movementUp.possible ? "U" : "") << (m.movementDown.possible ? "D" : "") << (m.movementLeft.possible ? "L" : "") << (m.movementRight.possible ? "R" : "") << (m.movementStationary.possible ? "S" : "") << "]";
	}
};

class Bomb
{
	public:
	Coordinate location;
	int timeToDetonation;
	bool destroyed;
	bool exploding;
	vector<Coordinate> detonationRange;
	
	Bomb(){}
	
	Bomb(int x, int y)
	{
		destroyed = false;
		exploding = false;
		location.x = x;
		location.y = y;
		timeToDetonation = BOMB_DETONATION_TIME;
		
		detonationRange.push_back(location);
		for (int detonationX = location.x; detonationX >= 0 && detonationX > location.x-BOMB_RANGE; detonationX--)
		{
			if (nodeFloor[detonationX][location.y] == '#')
			{
				break;
			}
			
			detonationRange.push_back(Coordinate(detonationX, location.y));
		}
		for (int detonationX = location.x; detonationX <= floorCorner.x && detonationX < location.x+BOMB_RANGE; detonationX++)
		{
			if (nodeFloor[detonationX][location.y] == '#')
			{
				break;
			}
			
			detonationRange.push_back(Coordinate(detonationX, location.y));
		}
		for (int detonationY = location.y; detonationY >= 0 && detonationY > location.y-BOMB_RANGE; detonationY--)
		{
			if (nodeFloor[location.x][detonationY] == '#')
			{
				break;
			}
			
			detonationRange.push_back(Coordinate(location.x, detonationY));
		}
		for (int detonationY = location.y; detonationY <= floorCorner.y && detonationY < location.y+BOMB_RANGE; detonationY++)
		{
			if (nodeFloor[location.x][detonationY] == '#')
			{
				break;
			}
			
			detonationRange.push_back(Coordinate(location.x, detonationY));
		}
	}

	Bomb& operator=(const Bomb& other)
	{
		location = other.location;
		timeToDetonation = other.timeToDetonation;
		destroyed = other.destroyed;
		exploding = other.exploding;
		for (vector<Coordinate>::const_iterator it = other.detonationRange.begin(); it != other.detonationRange.end(); ++it)
		{
			detonationRange.push_back(*it);
		}
	}

	bool tick()
	{
		exploding = false;
		timeToDetonation--;
		if (!destroyed && timeToDetonation <= 0)
		{
			return true;
		}
		
		return false;
	}
	
	bool kaboom()
	{
		if (!destroyed)
		{
			destroyed = true;
			exploding = true;
			return true;
		}
		
		return false;
	}

	friend ostream &operator<<(ostream &os, Bomb const &m)
	{
		return os << "{" << m.location << m.timeToDetonation << (m.destroyed ? "." : "O") << (m.exploding ? "BOOM" : "") << "}";
	}
};

class Grid
{
	public:
	vector<Bomb> bombs;
	vector<Node> nodes;
	vector<Coordinate> detonations;
	vector<Coordinate> placements;
	bool firstTurn;
	
	Grid(){}
	
	Grid(int width, int height)
	{
		firstTurn = true;
		floorCorner = Coordinate(width-1, height-1);
		for (int x = 0; x < width; ++x)
		{
			vector<char> column;
			column.resize(height, '.');
			nodeFloor.push_back(column);
		}
	}

	Grid& operator=(const Grid& other)
	{
		firstTurn = other.firstTurn;
		for (vector<Coordinate>::const_iterator it = other.detonations.begin(); it != other.detonations.end(); ++it)
		{
			detonations.push_back(*it);
		}
		for (vector<Node>::const_iterator it = other.nodes.begin(); it != other.nodes.end(); ++it)
		{
			nodes.push_back(*it);
		}
		for (vector<Bomb>::const_iterator it = other.bombs.begin(); it != other.bombs.end(); ++it)
		{
			bombs.push_back(*it);
		}
		for (vector<Coordinate>::const_iterator it = other.placements.begin(); it != other.placements.end(); ++it)
		{
			placements.push_back(*it);
		}
	}

	void update (int row, string input)
	{
		for (int x = 0; x <= floorCorner.x; ++x)
		{
			nodeFloor[x][row] = input[x];
			
			if (firstTurn)
			{
				if (input[x] == '@')
				{
					nodes.push_back(Node(x, row));
				}
			}
		}
	}
	
	int remainingActiveNodes() const
	{
		int remaining = 0;

		for (vector<Node>::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if (!(*node).destroyed)
			{
				remaining++;
			}
		}
		
		return remaining;
	}
	
	bool anyUnknownNodes() const
	{
		for (vector<Node>::const_iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if (!(*node).isDirectionKnown())
			{
				return true;
			}
		}
		
		return false;
	}
	
	void updateBombs()
	{
		bool anyKaboom = false;
		detonations.clear();

		// Check any timewise explosions
		for (vector<Bomb>::iterator bomb = bombs.begin(); bomb != bombs.end(); ++bomb)
		{
			if ((*bomb).tick())
			{
				for (vector<Coordinate>::iterator det = (*bomb).detonationRange.begin(); det != (*bomb).detonationRange.end(); ++det)
				{
					detonations.push_back(*det);
				}

				(*bomb).kaboom();
				anyKaboom = true;
			}
		}

		// Check any chain explosions
		while (anyKaboom)
		{
			anyKaboom = false;
			for (vector<Bomb>::iterator bomb = bombs.begin(); bomb != bombs.end(); ++bomb)
			{
				if (!(*bomb).destroyed)
				{
					bool thisBombChained = false;

					for (vector<Coordinate>::iterator detLocation = detonations.begin(); detLocation != detonations.end(); ++detLocation)
					{
						if ((*bomb).location == (*detLocation))
						{
							thisBombChained = true;
							break;
						}
					}
					
					if (thisBombChained)
					{
						for (vector<Coordinate>::iterator det = (*bomb).detonationRange.begin(); det != (*bomb).detonationRange.end(); ++det)
						{
							detonations.push_back(*det);
						}

						(*bomb).kaboom();
						anyKaboom = true;
					}
				}
			}
		}
	}
	
	void destroyNodes()
	{
		for (vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if ((*node).destroyed)
			{
				continue;
			}

			Coordinate oldLocation = (*node).location();

			for (vector<Coordinate>::iterator detLocation = detonations.begin(); detLocation != detonations.end(); ++detLocation)
			{
				if (oldLocation == (*detLocation))
				{
					(*node).destroyed = true;
					break;
				}
			}
		}
	}

	void moveNodes()
	{
		// Move active nodes
		for (vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if ((*node).destroyed)
			{
				continue;
			}

			(*node).tick();
		}
	}
	
	void tick()
	{
		updateBombs();
		destroyNodes();
		moveNodes();
		
		firstTurn = false;
	}
	
	bool placeBomb(Coordinate place)
	{
		if (nodeFloor[place.x][place.y] == '#')
		{
			return false;
		}

		for (vector<Node>::iterator node = nodes.begin(); node != nodes.end(); ++node)
		{
			if (
				false
//			 ||	((*node).location() == place)
//			 || ((*node).nextLocation() == place)
			 || ((*node).prevLocation() == place)
				)
			{
				return false;
			}
		}
		
		bombs.push_back(Bomb(place.x, place.y));
		placements.push_back(place);
		return true;
	}
	
	void placeWait()
	{
		placements.push_back(Coordinate(-1,-1));
	}

	friend ostream &operator<<(ostream &os, Grid const &m)
	{
		for (vector<Bomb>::const_iterator bomb = m.bombs.begin(); bomb != m.bombs.end(); ++bomb)
		{
			os << (*bomb);
		}
		for (vector<Node>::const_iterator node = m.nodes.begin(); node != m.nodes.end(); ++node)
		{
			os << (*node);
		}
		os << endl;

		for (int y = 0; y <= floorCorner.y; y++)
		{
			for (int x = 0; x <= floorCorner.x; x++)
			{
				bool showBomb = false;
				bool showNode = false;

				for (vector<Bomb>::const_iterator bomb = m.bombs.begin(); bomb != m.bombs.end(); ++bomb)
				{
					if (!(*bomb).destroyed && (*bomb).location.x == x && (*bomb).location.y == y)
					{
						os << (*bomb).timeToDetonation;
						showBomb = true;
						break;
					}
				}
				
				if (!showBomb)
				{
					for (vector<Node>::const_iterator node = m.nodes.begin(); node != m.nodes.end(); ++node)
					{
						if (!(*node).destroyed && (*node).location().x == x && (*node).location().y == y)
						{
							os << (*node).directionString();
							showNode = true;
							break;
						}
					}
				}
				
				if (!showBomb && !showNode)
				{
					os << nodeFloor[x][y];
				}
			}
			os << endl;
		}
		
		os << "--------------------" << endl;

		for (int y = 0; y <= floorCorner.y; y++)
		{
			for (int x = 0; x <= floorCorner.x; x++)
			{
				os << nodeFloor[x][y];
			}
			os << endl;
		}

		
		return os;
	}

};

Grid grid;

void simulate(int rounds, int bombs, double deadline)
{
	if (grid.anyUnknownNodes())
	{
		return;
	}

	uniform_int_distribution<int> randomPlace{0, 100};
	uniform_int_distribution<int> randomX{0, floorCorner.x};
	uniform_int_distribution<int> randomY{0, floorCorner.y};

	Coordinate placeLocation;
	int iterations = 0;
	int currentPlacements = grid.placements.size();
	int minimumCompletion = (currentPlacements > 0) ? currentPlacements : rounds;

if (currentPlacements > 0)
{
	// Don't try to do better
	return;
}
	
	while (clock() < deadline)
	{
		iterations++;
		Grid simGrid = grid;
		int remainingBombs = bombs;
		simGrid.placements.clear();
		
		for (int tick = 0; tick < rounds; ++tick)
		{
			simGrid.tick();

			bool place = randomPlace(randomEngine) > 0;
			if (place && remainingBombs > 0)
			{
				do
				{
					placeLocation = Coordinate(randomX(randomEngine), randomY(randomEngine));
				} while (!simGrid.placeBomb(placeLocation));

				remainingBombs--;
			}
			else
			{
				simGrid.placeWait();
			}

			if (!simGrid.remainingActiveNodes())
			{
				if (minimumCompletion > tick)
				{
					minimumCompletion = tick;
					grid.placements.clear();
					for (vector<Coordinate>::iterator it = simGrid.placements.begin(); it != simGrid.placements.end(); ++it)
					{
						grid.placements.push_back(*it);
					}
				}
				
				break;
			}
		}
	}

	cerr << iterations << " " << turnStartTime << " " << clock() << " " << deadline << " " << (double)CLOCKS_PER_SEC << " " << grid.placements.size() << endl;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int width; // width of the firewall grid
    int height; // height of the firewall grid
    cin >> width >> height; cin.ignore();
	grid = Grid(width, height);

    // game loop
    while (1)
	{
		turnStartTime = clock();

        int rounds; // number of rounds left before the end of the game
        int bombs; // number of bombs left
        cin >> rounds >> bombs; cin.ignore();
        for (int i = 0; i < height; i++)
		{
            string mapRow; // one line of the firewall grid
            getline(cin, mapRow);
			grid.update(i, mapRow);
        }
		grid.tick();
		cerr << grid << endl;
		
		simulate(rounds, bombs, turnStartTime + (double)CLOCKS_PER_SEC * SIMULATION_TIME);

		if (!grid.placements.empty() && grid.placements.front().x >= 0)
		{
			cout << grid.placements.front().x << " " << grid.placements.front().y << endl;
			grid.bombs.push_back(Bomb(grid.placements.front().x, grid.placements.front().y));
		}
		else
		{
			cout << "WAIT" << endl;
		}
		
		if (!grid.placements.empty() > 0)
		{
			grid.placements.erase (grid.placements.begin());
		}
    }
}
