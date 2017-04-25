#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>

using namespace std;

#define COLUMNS 35
#define ROWS    20
#define MAX_BACK_IN_TIME 25

class Coordinate
{
    public:
    int x;
    int y;
    
    Coordinate() : x(-1), y(-1)
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
    }
	
	bool operator< (const Coordinate& other) const
	{
		return x < other.x || y < other.y;
	}

    friend ostream &operator<<(ostream &os, Coordinate const &m)
    {
        return os << "(" << m.x << "," << m.y << ")";
    }        
};

class Cell
{
    #define MAX_PLAYERS 4
    #define NEUTRAL     -1

    private:

    public:
    int owner;
    bool containsPlayer[MAX_PLAYERS];
    bool scratch; // Temp variable for calculations
	Coordinate absolutePosition;
    
    Cell()
    {
        reset();
    }
    
    Cell(int x, int y) : absolutePosition(Coordinate(x,y))
    {
        Cell();
    }

    void reset()
    {
        setNeutral();
        removePlayers();
        setScratch(false);
    }
    
    void setNeutral()
    {
        owner = NEUTRAL;
    }
    
    void removePlayers()
    {
        for (int playerHere = 0; playerHere < MAX_PLAYERS; ++playerHere)
        {
            containsPlayer[playerHere] = false;
        }
    }
    
    void setScratch(bool set)
    {
        scratch = set;
    }

    void addPlayer(int player)
    {
        containsPlayer[player] = true;
    }
    
    void setOwner(int player)
    {
        owner = player;
    }
    
    void setOwner(char contents)
    {
        if (contents == '.')
        {
            setNeutral();
        }
        else
        {
            setOwner(contents - '0');
        }
    }
    
    bool isNeutral() const
    {
        return owner == NEUTRAL;
    }
    
    bool isOwner(int player) const
    {
        return owner == player;
    }
    
    bool isLost() const
    {
        return !isNeutral() && !isOwner(0);
    }
    
    friend ostream &operator<<(ostream &os, Cell const &m)
    {
        if (m.isNeutral())
        {
            os << '.';
        }
        else
        {
            os << m.owner;
        }

		os << m.absolutePosition.x << "," << m.absolutePosition.y;
        return os;
    }    
};

class Player
{
    public:
    int name;
    int score;
    Coordinate location;
    bool backInTimeLeft;
    
    Player()
    {
        resetScore();
        backInTimeLeft = true;        
    }
    
    Player(int name) : name(name)
    {
        Player();
    }
    
    void setLocation(Coordinate _location)
    {
        location = _location;
    }
    
    void setBackInTimeLeft(int status)
    {
		backInTimeLeft = (status > 0);
    }
    
    void resetScore()
    {
        score = 0;
    }
    
    void incrementScore()
    {
        score++;
    }
	
    friend ostream &operator<<(ostream &os, Player const &m)
    {
        return os << "[" << m.name << "] " << m.score << m.location << " " << (m.backInTimeLeft ? "T" : "F");
    }
};

class Board
{
    private:
    int rows;
    int columns;
    Coordinate origin;
	bool spoiled;
	int spoiledState;

    public:
    vector<vector<Cell*> > cells;

    Board()
    {}
    
    Board(int columns, int rows, vector<vector<Cell*> > _cells, Coordinate origin = Coordinate(0,0)) : columns(columns), rows(rows), origin(origin), spoiled(false)
    {
		// Map the cells from their absolute positions to this board's local coordinate system
		for (int x = origin.x; x < origin.x + columns; ++x)
		{
			vector<Cell*> thisColumn;

			for (int y = origin.y; y < origin.y + rows; ++y)
			{
				thisColumn.push_back(_cells[x][y]);
			}
			
			cells.push_back(thisColumn);
		}
        Board();
    }

    void setLine(string line, int currentRow, vector<Player>* players)
    {
        for (int cellIndex = 0; cellIndex < columns; ++cellIndex)
        {
            Cell* cell = cells[cellIndex][currentRow];
            cell->setOwner(line[cellIndex]);
            for (vector<Player>::iterator player = players->begin(); player != players->end(); ++player)
            {
                if (cell->isOwner((*player).name))
                {
                    (*player).incrementScore();
                }
            }
        }
    }
    
    bool isNeutral(Coordinate location)
    {
        return cells[location.x][location.y]->isNeutral();
    }
    
    bool isOnEdge(Coordinate absolutePosition)
    {
		return (absolutePosition.x == origin.x || absolutePosition.x == origin.x + columns - 1 ||
                absolutePosition.y == origin.y || absolutePosition.y == origin.y + rows - 1);
    }

	// Is this board already taken?
	bool isSpoiled(int state)
	{
		// If someone went back in time since last time this board was checked
		if (spoiledState != state)
		{
			// Recheck this board for spoilage
			spoiled = false;
			spoiled = checkSpoiled();
			spoiledState = state;
		}

		return spoiled;
	}
	
	vector<Coordinate> getEdges()
	{
		vector<Coordinate> edges;
		int left = origin.x;
		int right = origin.x + columns - 1;
		int top = origin.y;
		int bottom = origin.y + rows - 1;
		
		for (int x = left; x <= right; ++x)
		{
			edges.push_back(Coordinate(x, top));
			edges.push_back(Coordinate(x, bottom));
		}
		
		for (int y = top; y <= bottom; ++y)
		{
			edges.push_back(Coordinate(left, y));
			edges.push_back(Coordinate(right, y));
		}
		
		return edges;
	}
    
	int numNeutralCells()
	{
		int amount = 0;
        for (int x = 0; x < columns; ++x)
        {
            for (int y = 0; y < rows; ++y)
            {
				if (cells[x][y]->isNeutral())
				{
					amount++;
				}
            }
        }
		
		return amount;
	}
    
    void resetScratch()
    {
        for (int x = 0; x < columns; ++x)
        {
            for (int y = 0; y < rows; ++y)
            {
                cells[x][y]->setScratch(false);
            }
        }
    }
    
    Coordinate nextToCapture(Coordinate thisLocation)
    {
        int minDistance = 10000;
        Cell* nextCell = NULL;

        for (int x = 0; x < columns; ++x)
        {
            for (int y = 0; y < rows; ++y)
            {
				Cell* targetCell = cells[x][y];
                if (!targetCell->scratch && targetCell->isNeutral() && isOnEdge(targetCell->absolutePosition))
                {
                    int dist = thisLocation.distance(targetCell->absolutePosition);
                    if (dist < minDistance)
                    {
                        minDistance = dist;
                        nextCell = targetCell;
                    }
                }
            }
        }
        
        if (nextCell != NULL)
        {
            nextCell->setScratch(true);
            return nextCell->absolutePosition;
        }
        else
        {
            return Coordinate(-1, -1);
        }
    }
    
    vector<Coordinate> locationsToCapture(Coordinate myLocation)
    {
        resetScratch();
		vector<Coordinate> locations;
        Coordinate nextLocation = myLocation;
        while(true)
        {
            nextLocation = nextToCapture(nextLocation);
            if (nextLocation.x >= 0)
            {
				locations.push_back(nextLocation);
            }
            else
            {
                break;
            }
        }

        return locations;
    }
	
	int totalSize() const
	{
		return columns * rows;
	}
	
	bool checkSpoiled()
	{
		if (!spoiled)
		{
			for (int x = 0; x < columns; ++x)
			{
				for (int y = 0; y < rows; ++y)
				{
					// If this cell already belongs to a different player
					if (cells[x][y]->isLost())
					{
						spoiled = true;
						return spoiled;
					}
				}
			}
		}
		
		return spoiled;
	}
	
	// The number of turns before an enemy can take one of the cells.  Returns 0 if not viable
	int numTurnsToSpoil(vector<Player>* players)
	{
		vector<Coordinate> edges = getEdges();
		int minimumDistance = 100000;

		for (vector<Coordinate>::iterator position = edges.begin(); position != edges.end(); ++position)
		{
			for (vector<Player>::iterator player = players->begin() + 1; player != players->end(); ++player)
			{
				int checkDistance = (*player).location.distance(*position);
				if (checkDistance < minimumDistance)
				{
					minimumDistance = checkDistance;
				}
			}
        }
		
		return minimumDistance;
	}

	// Find the closest cell that is open and run for it
    Coordinate getOpenCell(Coordinate location)
    {
        Coordinate openCell;
		int leastDistance = 100000;
		openCell = Coordinate(rand() % columns, rand() % rows);  // Default to running around randomly
		vector<Coordinate> openCells;
		
		// Find closest open cell
		for (int x = 0; x < columns; ++x)
		{
			for (int y = 0; y < rows; ++y)
			{
				if (cells[x][y]->isNeutral())
				{
					int cellDistance = location.distance(cells[x][y]->absolutePosition);

					// Don't stall on this cell.  This can happen if someone else enters a neutral cell at the same time as me.
					if (cellDistance == 0)
					{
						continue;
					}

					if (cellDistance < leastDistance)
					{
						leastDistance = cellDistance;
						openCells.clear();
					}

					if (cellDistance == leastDistance)
					{
						openCells.push_back(cells[x][y]->absolutePosition);
					}
				}
			}
		}
		
		if (!openCells.empty())
		{
			openCell = openCells[rand() % openCells.size()];
		}

        return openCell;
    }
	
	bool isOnBoard(Coordinate location)
	{
		return (location.x >= origin.x) && (location.x < origin.x + columns) && (location.y >= origin.y) && (location.y < origin.y + rows);
	}
	
	bool operator< (const Board& other) const
	{
		// This is the default sort order, so return the most desirable properties
		return !spoiled && other.spoiled || totalSize() > other.totalSize();
	}

    friend ostream &operator<<(ostream &os, Board const &m)
    {
        os << "Board[" << m.columns << "," << m.rows << "] at " << m.origin << endl;
        for (int row = 0; row < m.rows; ++row)
        {
            for (int column = 0; column < m.columns; ++column)
            {
                os << *m.cells[column][row];
            }
            
            os << endl;
        }
        
        return os;
    }
};

// Create a state to keep track of whether the spoiled state is stale
int createSpoiledState(vector<Player>* players)
{
	int state = 0;
	for (vector<Player>::iterator player = players->begin(); player != players->end(); ++player)
	{
		if (player->backInTimeLeft)
		{
			state |= (1 << player->name);
		}
	}

	return state;
}

// Generate all boards of the desired size and associate them with cells that intersect their edges
void addCheckBoards(vector<vector<Cell*> >* cells, map<Coordinate, vector<Board*> >* checkBoardEdges, Coordinate sizes)
{
	for (int x = 0; x < COLUMNS - sizes.x; ++x)
	{
		for (int y = 0; y < ROWS - sizes.y; ++y)
		{
			Board* board = new Board(sizes.x, sizes.y, *cells, Coordinate(x, y));

			vector<Coordinate> edges = board->getEdges();
			for(vector<Coordinate>::iterator it = edges.begin(); it < edges.end(); ++it)
			{
				Coordinate edgeLocation = Coordinate((*it).x, (*it).y);
				(*checkBoardEdges)[edgeLocation].push_back(board);
			}
		}
	}
}

// Number of turns it will take to follow this path
int numTurnsToCompletePath(Coordinate startLocation, vector<Coordinate> path)
{
	int turns = 0;
	vector<Coordinate>::iterator it = path.begin();
	Coordinate previousLocation = startLocation;

	for (vector<Coordinate>::iterator it = path.begin(); it != path.end(); ++it)
	{
		turns += previousLocation.distance(*it);
		previousLocation = (*it);
	}
	
	return turns;
}

// Check to see if this board is worth taking
vector<Coordinate> checkBoard(Board board, int &mostCapturableCells, vector<Player>* players, int spoiledState)
{
    vector<Coordinate> dummy;
	Coordinate myLocation = players->front().location;

	if (!board.isSpoiled(spoiledState) && board.totalSize() > mostCapturableCells)
	{
		vector<Coordinate> movePattern;
		movePattern = board.locationsToCapture(myLocation);
		if (movePattern.size() < board.numTurnsToSpoil(players))
		{
			int neutralCells = board.numNeutralCells();
			if ((numTurnsToCompletePath(myLocation, movePattern) < neutralCells) && (neutralCells > mostCapturableCells))
			{
        		cerr << "GO!" << endl;
				mostCapturableCells = neutralCells;
				return movePattern;
			}
		}
	}
	
	return dummy;
}

// Attempt to find a large field to capture
vector<Coordinate> findLargeField(vector<Player>* players, map<Coordinate, vector<Board*> >* checkBoardEdges, double deadline, int spoiledState)
{
	vector<Coordinate> movePattern;
	vector<Coordinate> checkPattern;

	vector<Board*> boards = (*checkBoardEdges)[players->front().location];
	sort(boards.begin(), boards.end());
	
	int mostCapturableCells = 0;
	for (vector<Board*>::iterator board = boards.begin(); clock() < deadline && board != boards.end(); ++board)
	{
		checkPattern = checkBoard(*(*board), mostCapturableCells, players, spoiledState);
		if (!checkPattern.empty())
		{
			movePattern = checkPattern;
		}
	}

	return movePattern;
}

void addBoardSizes(vector<vector<Cell*> >* cells, map<Coordinate, vector<Board*> >* checkBoardEdges, vector<Coordinate>* checkBoardSizes, double deadline)
{
	// Add more board sizes to check
	if (!checkBoardSizes->empty())
	{
		// Use all the time if we haven't done our back in time trick yet
	    while (clock() < deadline)
	    {
	        int numCheckBoardSizes = checkBoardSizes->size();
	        if (numCheckBoardSizes > 0)
	        {
       			int checkBoardSizeIndex = rand() % numCheckBoardSizes;
       			vector<Coordinate>::iterator it = checkBoardSizes->begin() + checkBoardSizeIndex;
       			addCheckBoards(cells, checkBoardEdges, *it);
       			checkBoardSizes->erase(it);
	        }
			else
			{
				break;
			}
	    }
	}
	else
	{
		cerr << "All checkBoardSizes deployed!" << endl;
	}
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    vector<vector<Cell*> > cells = vector<vector<Cell*> >(COLUMNS, vector<Cell*>(ROWS));

    for (int x = 0; x < COLUMNS; ++x)
    {
        for (int y = 0; y < ROWS; ++y)
        {
			Cell* cell = new Cell(x, y);
			cells[x][y] = cell;
        }
    }
	
    int opponentCount; // Opponent count
    cin >> opponentCount; cin.ignore();
    
    vector<Player> players;
    players.push_back(Player(0)); // Add myself
    for (int name = 1; name <= opponentCount; ++name)
    {
        players.push_back(Player(name));
    }

    Board board(COLUMNS, ROWS, cells);
	vector<Coordinate> movePattern;
	vector<Coordinate> checkBoardSizes;
	map<Coordinate, vector<Board*> > checkBoardEdges;
    Coordinate destination;

    for (int x = 3; x < COLUMNS; ++x)
    {
        for (int y = 3; y < ROWS; ++y)
        {
			checkBoardSizes.push_back(Coordinate(x, y));
		}
	}

	int nextGameRound = 1;

    // game loop
    while (1)
    {
        int gameRound;
        cin >> gameRound; cin.ignore();

		clock_t start;
		double duration;
		start = clock();
		double addBoardDeadline = start + (double) CLOCKS_PER_SEC * 0.010;
		double findBoardDeadline = start + (double) CLOCKS_PER_SEC * 0.090;
		
		// If we went back in time
		if (gameRound != nextGameRound)
		{
			// Don't keep moving on predetermined path
			movePattern.clear();
/*
			// Try to get rid of the spoiled tags since they may not be spoiled now
			for (map<Coordinate, vector<Board*> >::iterator checkBoards = checkBoardEdges.begin(); clock() < findBoardDeadline && checkBoards != checkBoardEdges.end(); ++checkBoards)
			{
				for (vector<Board*>::iterator checkBoard = (*checkBoards).second.end() - 1; clock() < findBoardDeadline && checkBoard != (*checkBoards).second.begin(); --checkBoard)
				{
					(*checkBoard)->spoiled = false;
				}
			}
			
			cerr << "Unspoil ended at " << clock() << " with deadline " << findBoardDeadline << endl;
*/
		}
		nextGameRound = gameRound + 1;

        int x; // Your x position
        int y; // Your y position
        int backInTimeLeft; // Remaining back in time
        cin >> x >> y >> backInTimeLeft; cin.ignore();        
        players[0].resetScore();
        players[0].setLocation(Coordinate(x,y));
        players[0].setBackInTimeLeft(backInTimeLeft);
        for (int name = 1; name <= opponentCount; name++)
        {
            int opponentX; // X position of the opponent
            int opponentY; // Y position of the opponent
            int opponentBackInTimeLeft; // Remaining back in time of the opponent
            cin >> opponentX >> opponentY >> opponentBackInTimeLeft; cin.ignore();

            players[name].resetScore();
            players[name].setLocation(Coordinate(opponentX, opponentY));
            players[name].setBackInTimeLeft(opponentBackInTimeLeft);
        }
		
        for (int row = 0; row < ROWS; row++)
        {
            string line; // One line of the map ('.' = free, '0' = you, otherwise the id of the opponent)
            cin >> line; cin.ignore();
            
            board.setLine(line, row, &players);
        }

		// If we're just following a pattern or we haven't gone back in time
		if (!movePattern.empty() || backInTimeLeft > 0)
		{
			// Allow all the time to keep adding boards
			addBoardDeadline = findBoardDeadline;
		}
		
        // Add more board sizes to check
		addBoardSizes(&cells, &checkBoardEdges, &checkBoardSizes, addBoardDeadline);

		// If already used my back in time and I don't have a pattern
		if (movePattern.empty() && backInTimeLeft == 0)
		{
		    // Attempt to find a pattern
			cerr << "Number of Boards to search: " << checkBoardSizes.size() << endl;
			movePattern = findLargeField(&players, &checkBoardEdges, findBoardDeadline, createSpoiledState(&players));
		}

        // Add more board sizes to check
		addBoardSizes(&cells, &checkBoardEdges, &checkBoardSizes, findBoardDeadline);

        if (backInTimeLeft > 0 && gameRound == MAX_BACK_IN_TIME)
        {
			// Always go back in time on turn MAX_BACK_IN_TIME.  This allows MAX_BACK_IN_TIME turns of filling my buffers while possibly messing up the opponents.
            cout << "BACK " << MAX_BACK_IN_TIME << endl;
        }
		else if (movePattern.empty())
        {
            // Do default behavior
			if (destination.x < 0 || !(*cells[destination.x][destination.y]).isNeutral() || players[0].location == destination)
			{
				destination = board.getOpenCell(players[0].location);
			}

			cerr << "On my way to " << destination << endl;
			cout << destination.x << " " << destination.y << " Doo be doo" << endl; // action: "x y" to move or "BACK rounds" to go back in time
        }
		else
		{
		    cerr << "Pattern:";
		    for (vector<Coordinate>::iterator it = movePattern.begin(); it != movePattern.end(); ++it)
		    {
		        cerr << (*it);
		    }
		    cerr << endl;

			cout << movePattern.front().x << " " << movePattern.front().y << " This area looks nice" << endl; // action: "x y" to move or "BACK rounds" to go back in time
			if (movePattern.front().distance(Coordinate(x,y)) <= 1)
			{
    			movePattern.erase(movePattern.begin());
			}
		}

		duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
		cerr << "Duration: " << duration << endl;
    }
}
