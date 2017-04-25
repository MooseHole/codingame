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
    
    // The Manhattan distance between this coordinate and the input
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

class Square
{
	public:
	Coordinate location;
	bool water;
	int lakeSize;
	int checked;
	
	Square() : water(false), lakeSize(0), checked(-1) {}
	
    friend ostream &operator<<(ostream &os, Square const &m)
    {
        return os << (m.water ? "O" : "#");
    }        
};

class Grid
{
    private:

	public:
    int rows, columns;
	vector<vector<Square*> > squares;
	
	Grid(){}

	Grid(int columns, int rows) : columns(columns), rows(rows)
	{
		for (int x = 0; x < columns; ++x)
		{
			vector<Square*> thisColumn;

			for (int y = 0; y < rows; ++y)
			{
				thisColumn.push_back(new Square());
			}
			
			squares.push_back(thisColumn);
		}
	}

	void addRow(int rowNum, string row)
	{
	    for (int x = 0; x < row.length(); x++)
		{
		    squares[x][rowNum]->water = (row[x] == 'O');
		    squares[x][rowNum]->location = Coordinate(x, rowNum);
		}
	}
	
    friend ostream &operator<<(ostream &os, Grid const &m)
    {
        os << "    :0   5|  10|  15|  20|  25|  30|  35|  40|  45|  50|  55|  60|  65|  70|  75|  80|  85|  90|  95| 100|" << endl;
	    for (int y = 0; y < m.rows; y++)
    	{
    	    if (y < 10)
    	    {
    	        continue;
    	    }
		    if (y < 1000)
		    {
    		    os << " ";
		    }
		    if (y < 100)
		    {
    		    os << " ";
		    }
		    if (y < 10)
		    {
    		    os << " ";
		    }
		    os << y << ":";
    		for (int x = 0; x < m.columns; x++)
    		{
				os << *m.squares[x][y];
			}
			
			os << endl;
		}
		
        return os;
    }
};

int lastColumn;
int lastRow;
Grid grid;
int checkIndex;
vector<Square*> squaresToProcess;

bool checkSquare(Square* thisSquare)
{
   // Return 0 for land
    bool countMe = thisSquare->water && thisSquare->checked != checkIndex;
	
	thisSquare->checked = checkIndex;
	
	return countMe;
}

void getNeighbors(Square* thisSquare)
{
	if (thisSquare->location.x > 0)          squaresToProcess.push_back(grid.squares[thisSquare->location.x-1][thisSquare->location.y]);
	if (thisSquare->location.x < lastColumn) squaresToProcess.push_back(grid.squares[thisSquare->location.x+1][thisSquare->location.y]);
	if (thisSquare->location.y > 0)          squaresToProcess.push_back(grid.squares[thisSquare->location.x][thisSquare->location.y-1]);
	if (thisSquare->location.y < lastRow)    squaresToProcess.push_back(grid.squares[thisSquare->location.x][thisSquare->location.y+1]);
}

static int findLakeArea(Square* thisSquare)
{
	squaresToProcess.clear();
	squaresToProcess.push_back(thisSquare);
	int totalArea = 0;

	while (!squaresToProcess.empty())
	{
		Square* check = squaresToProcess.back();
		squaresToProcess.pop_back();
		if (checkSquare(check))
		{
			totalArea++;
			getNeighbors(check);
		}
	}

    return totalArea;
}


/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{

    int L;
    cin >> L; cin.ignore();
    int H;
    cin >> H; cin.ignore();

    lastColumn = L-1;
    lastRow = H-1;
	grid = Grid(L, H);

    for (int i = 0; i < H; i++) {
        string row;
        getline(cin, row);

		grid.addRow(i, row);
    }
	
//	cerr << grid << endl;
	
    int N;
    cin >> N; cin.ignore();
    vector<Coordinate> checks;
    for (int i = 0; i < N; i++) {
        int X;
        int Y;
        cin >> X >> Y; cin.ignore();
        checks.push_back(Coordinate(X, Y));
//        cerr << checks[i] << endl;
    }
    for (int i = 0; i < N; i++) {

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        checkIndex = i;
        cout << findLakeArea(grid.squares[checks[i].x][checks[i].y]) << endl;
    }
}