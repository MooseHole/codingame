#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <algorithm>

using namespace std;

class Coordinate
{
public:
    int x;
    int y;

    Coordinate()
    {
        x = -1;
        y = -1;
    }

    Coordinate(int _x, int _y) : x(_x), y(_y) {}

    Coordinate& operator=(const Coordinate &c) 
    {
        x = c.x;
        y = c.y;

        return *this;
    }

    bool operator==(const Coordinate& other)
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Coordinate& other)
    {
        return !(*this == other);
    }

    bool operator <(const Coordinate& other) const
    {
        return (x < other.x) || (x == other.x && y < other.y);
    }

    friend ostream & operator<<(ostream & Str, Coordinate const & v)
    {
        return Str << "(" << v.x << ", " << v.y << ")";
    }
};

class Direction
{
    char _direction;

public:
    Direction()
    {
        _direction = 'x';
    }

    Direction(char direction) : _direction(direction)
    {}

    Direction& operator=(const char &d) 
    {
        _direction = d;

        return *this;
    }

    char getDirection()
    {
        return _direction;
    }

    char GetEntranceFromExit()
    {
	switch (_direction)
        {
            case 'T':
                return 'B';
            case 'B':
                return 'T';
            case 'L':
                return 'R';
            case 'R':
                return 'L';
            default:
                return 'x';
        }
    }

    bool operator==(const char& other)
    {
        return _direction == other;
    }

    bool operator==(const Direction& other)
    {
        return *this == other._direction;
    }

    bool operator!=(const char& other)
    {
        return !(*this == other);
    }

    bool operator!=(const Direction& other)
    {
        return !(*this == other);
    }

    friend ostream & operator<<(ostream & Str, Direction const & v)
    {
	switch (v._direction)
        {
            case 'T':
                return Str << "Top";
            case 'B':
                return Str << "Bottom";
            case 'L':
                return Str << "Left";
            case 'R':
                return Str << "Right";
            default:
                return Str << "UNKNOWN DIRECTION";
        }
    }
};

class Room
{
protected:
    Direction _entrance;
    Direction _exit;
    int _type;
    bool _canRotate;
    Coordinate* _location;

    void RefreshExits()
    {
        _exit = 'x';
        if (_entrance != 'L' && _entrance != 'R' && _entrance != 'T')
        {
            return;
        }

        switch (_type)
        {
            case 0:
                break;
            case 1:
                _exit = 'B';
                break;
            case 2:
            case 6:
                if (_entrance == 'L')
                {
                    _exit = 'R';
                }
                else if (_entrance == 'R')
                {
                    _exit = 'L';
                }
                break;
            case 3:
                if (_entrance == 'T')
                {
                    _exit = 'B';
                }
                break;
            case 4:
                if (_entrance == 'T')
                {
                    _exit = 'L';
                }
                else if (_entrance == 'R')
                {
                    _exit = 'B';
                }
                break;
            case 5:
                if (_entrance == 'T')
                {
                    _exit = 'R';
                }
                else if (_entrance == 'L')
                {
                    _exit = 'B';
                }
                break;
            case 7:
                if (_entrance == 'T')
                {
                    _exit = 'B';
                }
                else if (_entrance == 'R')
                {
                    _exit = 'B';
                }
                break;
            case 8:
                if (_entrance == 'L')
                {
                    _exit = 'B';
                }
                else if (_entrance == 'R')
                {
                    _exit = 'B';
                }
                break;
            case 9:
                if (_entrance == 'T')
                {
                    _exit = 'B';
                }
                else if (_entrance == 'L')
                {
                    _exit = 'B';
                }
                break;
            case 10:
                if (_entrance == 'T')
                {
                    _exit = 'L';
                }
                break;
            case 11:
                if (_entrance == 'T')
                {
                    _exit = 'R';
                }
                break;
            case 12:
                if (_entrance == 'R')
                {
                    _exit = 'B';
                }
                break;
            case 13:
                if (_entrance == 'L')
                {
                    _exit = 'B';
                }
                break;
        }
    }

public:
    Room(int type, Coordinate* location)
    {
        _canRotate = type > 0;
        _type = abs(type);
        _location = location;
        UpdateEntrance('T');
    }

    bool UpdateEntrance(char direction)
    {
        if (CanEnter(direction))
        {
            _entrance = direction;
            RefreshExits();
            return true;
        }

        return false;
    }

    bool UpdateEntrance(Direction direction)
    {
        if (direction == 'T') return UpdateEntrance('T');
        if (direction == 'L') return UpdateEntrance('L');
        if (direction == 'R') return UpdateEntrance('R');

        return false;
    }

    Direction GetExit()
    {
        return _exit;
    }

    bool CanEnter(char direction)
    {
        switch (direction)
        {
            case 'T':
                switch (_type)
                {
                    case 1:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 9:
                    case 10:
                    case 11:
                        return true;
                    default:
                        return false;
                }
            case 'L':
                switch (_type)
                {
                    case 1:
                    case 2:
                    case 4:
                    case 5:
                    case 6:
                    case 8:
                    case 9:
                    case 10:
                    case 13:
                        return true;
                    default:
                        return false;
                }
            case 'R':
                switch (_type)
                {
                    case 1:
                    case 2:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 11:
                    case 12:
                        return true;
                    default:
                        return false;
                }
            default:
                return false;
        }
    }

    Coordinate GetNextLocation()
    {
        switch (_exit.getDirection())
        {
            case 'B':
                return Coordinate(_location->x, _location->y + 1);
            case 'L':
                return Coordinate(_location->x - 1, _location->y);
            case 'R':
                return Coordinate(_location->x + 1, _location->y);
        }
    }

    void RotateLeft()
    {
        if (!_canRotate)
        {
            return;
        }

        switch (_type)
        {
            case 0:
            case 1:
                break;
            case 2:
                _type = 3;
                break;
            case 3:
                _type = 2;
                break;
            case 4:
                _type = 5;
                break;
            case 5:
                _type = 4;
                break;
            case 6:
                _type = 9;
                break;
            case 7:
                _type = 6;
                break;
            case 8:
                _type = 7;
                break;
            case 9:
                _type = 8;
                break;
            case 10:
                _type = 13;
                break;
            case 11:
                _type = 10;
                break;
            case 12:
                _type = 11;
                break;
            case 13:
                _type = 12;
                break;
        }
    }

    void RotateRight()
    {
        if (!_canRotate)
        {
            return;
        }

        switch (_type)
        {
            case 0:
            case 1:
                break;
            case 2:
                _type = 3;
                break;
            case 3:
                _type = 2;
                break;
            case 4:
                _type = 5;
                break;
            case 5:
                _type = 4;
                break;
            case 6:
                _type = 7;
                break;
            case 7:
                _type = 8;
                break;
            case 8:
                _type = 9;
                break;
            case 9:
                _type = 6;
                break;
            case 10:
                _type = 11;
                break;
            case 11:
                _type = 12;
                break;
            case 12:
                _type = 13;
                break;
            case 13:
                _type = 10;
                break;
        }
    }

    bool operator==(const Room& other)
    {
        return _location == other._location;
    }

    bool operator==(const Coordinate& other)
    {
        return *_location == other;
    }

    bool operator!=(const Room& other)
    {
        return !(*this == other);
    }

    bool operator!=(const Coordinate& other)
    {
        return !(*this == other);
    }

    friend ostream & operator<<(ostream & Str, Room const & v)
    {
        return Str << "Room " << *v._location << " Type: " << v._type << " Enter: " << v._entrance << " Exit: " << v._exit;
    }
};

map<Coordinate, Room*> Rooms;

class Entity
{
    Room* _current;

public:

    void UpdateLocation(int x, int y, string entrance)
    {
        cerr << "UpdateLocation(" << x << ", " << y << ", " << entrance << ")" << endl;
        Coordinate location(x, y);
        cerr << "UpdateLocation location: " << location << endl;
        _current = Rooms[location];
        cerr << "UpdateLocation _current: " << *_current << endl;
        _current->UpdateEntrance(entrance[0]);
        cerr << "UpdateLocation updated the entrance" << endl;
    }

    Room GetEnd(Coordinate* goal)
    {
        Room* iterator = _current;
        while(1)
        {
            Direction exit = iterator->GetExit();
            if (exit == 'x')
            {
                return *iterator;
            }
            Direction nextEntrance = exit.GetEntranceFromExit();
            Coordinate nextLocation = iterator->GetNextLocation();
            iterator = Rooms[nextLocation];
            if (*iterator == *goal)
            {
                return *iterator;
            }

            iterator->UpdateEntrance(nextEntrance);
        }
    }

    friend ostream & operator<<(ostream & Str, Entity const & v)
    {
        return Str << "Entity " << *v._current;
    }
};

int main()
{
    int W; // number of columns.
    int H; // number of rows.
    cin >> W >> H; cin.ignore();
    
    for (int i = 0; i < H; i++)
    {
        string LINE; // each line represents a line in the grid and contains W integers T. The absolute value of T specifies the type of the room. If T is negative, the room cannot be rotated.
        getline(cin, LINE);

        char space_char = ' ';
        vector<string> words{};

        stringstream sstream(LINE);
        string word;
        while (getline(sstream, word, space_char))
        {
            word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
            words.push_back(word);
        }

        int j = 0;
        for (const auto &type : words) // access by reference to avoid copying
        {  
            Coordinate* location = new Coordinate(j++, i);
            Rooms.insert(pair<Coordinate, Room*>(*location, new Room(stoi(type), location)));
            cerr << "Room added: " << *Rooms[*location] << endl;
        }
    }

    int EX; // the coordinate along the X axis of the exit.
    cin >> EX; cin.ignore();
    Coordinate* exit = new Coordinate(EX, H - 1);

    Entity Indy;


    // game loop
    while (1)
    {
        int XI;
        int YI;
        string POSI;
        cin >> XI >> YI >> POSI; cin.ignore();

        Indy.UpdateLocation(XI, YI, POSI);
        cerr << "Indy is at: " << Indy << endl;


        int R; // the number of rocks currently in the grid.
        cin >> R; cin.ignore();
        for (int i = 0; i < R; i++) {
            int XR;
            int YR;
            string POSR;
            cin >> XR >> YR >> POSR; cin.ignore();
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cerr << "Indy ends at: " << Indy.GetEnd(exit) << endl;

        // One line containing on of three commands: 'X Y LEFT', 'X Y RIGHT' or 'WAIT'
        cout << "WAIT" << endl;
    }
}