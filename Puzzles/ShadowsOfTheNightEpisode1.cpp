#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Building
{
    public:
    int width;
    int height;
    int centerX;
    int centerY;
    int minX;
    int maxX;
    int minY;
    int maxY;
    vector<vector<bool>> windows;
    
    Building()
    {
    }
    
    Building(int _width, int _height)
    {
        Building();
        width = _width;
        height = _height;
        windows = vector<vector<bool>>(width, vector<bool>(height, true));
        resetSearch();
    }
    
    void resetSearch()
    {
        minX = 0;
        maxX = width;
        minY = 0;
        maxY = height;
    }
    
    void updateCenter()
    {
        cerr << minX << " " << maxX << " " << minY << " " << maxY << endl;
        centerX = minX + ((maxX - minX) / 2);
        centerY = minY + ((maxY - minY) / 2);
    }

    friend ostream &operator<<(ostream &os, Building const &m)
    {
        os << m.width << "x" << m.height << endl;
        for (int y = 0; y < m.height; ++y)
        {
            for (int x = 0; x < m.width; ++x)
            {
                os << (m.windows[x][y] ? "T" : "F");
            }
            os << endl;
        }

        return os;
    }
};

class Batman
{
    private:
    Building* building;

    void clearWindows(int startX, int endX, int startY, int endY)
    {
        cerr << "Clearing: " << startX << " " << endX << " " << startY << " " << endY << endl;
        for (int i = startX; i < endX; ++i)
        {
            for (int j = startY; j < endY; ++j)
            {
                building->windows[i][j] = false;
            }
        }
    }

    public:
    int x;
    int y;
    
    Batman()
    {
    }
    
    Batman(int _x, int _y, Building* _building)
    {
        x = _x;
        y = _y;
        building = _building;
    }
    
    void clearBelow()
    {
        building->maxY = y-1;
    }
    
    void clearAbove()
    {
        building->minY = y+1;
    }

    void clearRight()
    {
        building->maxX = x-1;
    }

    void clearLeft()
    {
        building->minX = x+1;
    }
	    
    void rayUp()
    {
        clearLeft();
        clearRight();
        clearBelow();
    }
    
    void rayDown()
    {
        clearLeft();
        clearRight();
        clearAbove();
    }
    
    void rayLeft()
    {
        clearRight();
        clearAbove();
        clearBelow();
    }
    
    void rayRight()
    {
        clearLeft();
        clearAbove();
        clearBelow();
    }
    
    void rayUpLeft()
    {
        clearRight();
        clearBelow();
    }
    
    void rayUpRight()
    {
        clearLeft();
        clearBelow();
    }
    
    void rayDownLeft()
    {
        clearRight();
        clearAbove();
    }
    
    void rayDownRight()
    {
        clearLeft();
        clearAbove();
    }
    
    void updateHeat(string direction)
    {
        int firstChar = 0;
        int lastChar = direction.length() - 1;

        // Clear direction
        if      (direction == "U")  { rayUp(); }
        else if (direction == "R")  { rayRight(); }
        else if (direction == "D")  { rayDown(); }
        else if (direction == "L")  { rayLeft(); }
        else if (direction == "UR") { rayUpRight(); }
        else if (direction == "DR") { rayDownRight(); }
        else if (direction == "DL") { rayDownLeft(); }
        else if (direction == "UL") { rayUpLeft(); }
    }
    
    void jump()
    {
        building->updateCenter();
        x = building->centerX;
        y = building->centerY;
    }

    friend ostream &operator<<(ostream &os, Batman const &m)
    {
        os << "(" << m.x << "," << m.y << ")" << endl;
        for (int y = 0; y < m.building->height; ++y)
        {
            for (int x = 0; x < m.building->width; ++x)
            {
                if (m.x == x && m.y == y)
                {
                    os << "b";
                }
                else
                {
                    os << (m.building->windows[x][y] ? "T" : "F");
                }
            }
            os << endl;
        }

        return os;
    }
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int W; // width of the building.
    int H; // height of the building.
    cin >> W >> H; cin.ignore();
    
    Building building(W, H);

    int N; // maximum number of turns before game over.
    cin >> N; cin.ignore();
    int X0;
    int Y0;
    cin >> X0 >> Y0; cin.ignore();
    Batman batman(X0, Y0, &building);

    // game loop
    while (1) {
        string BOMB_DIR; // the direction of the bombs from batman's current location (U, UR, R, DR, D, DL, L or UL)
        cin >> BOMB_DIR; cin.ignore();
        
        batman.updateHeat(BOMB_DIR);
        batman.jump();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << batman.x << " " << batman.y << endl; // the location of the next window Batman should jump to.
    }
}