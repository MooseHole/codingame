#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

enum class Compass { TOP, BOTTOM, LEFT, RIGHT, NONE };

unordered_map<string, Compass> compassMap =
{
    {"TOP", Compass::TOP},
    {"BOTTOM", Compass::BOTTOM},
    {"LEFT", Compass::LEFT},
    {"RIGHT", Compass::RIGHT}
};

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

    bool operator==(const Coordinate& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Coordinate& other) const
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
    Compass _direction;

public:
    Direction()
    {
        _direction = Compass::NONE;
    }

    Direction(Compass direction) : _direction(direction)
    {}

    Direction& operator=(const Compass &d) 
    {
        _direction = d;

        return *this;
    }

    Compass getDirection()
    {
        return _direction;
    }

    Compass GetEntranceFromExit()
    {
	switch (_direction)
        {
            case Compass::TOP:
                return Compass::BOTTOM;
            case Compass::BOTTOM:
                return Compass::TOP;
            case Compass::LEFT:
                return Compass::RIGHT;
            case Compass::RIGHT:
                return Compass::LEFT;
            default:
                return Compass::NONE;
        }
    }

    bool operator==(const Compass& other) const
    {
        return _direction == other;
    }

    bool operator==(const Direction& other) const
    {
        return *this == other._direction;
    }

    bool operator!=(const Compass& other) const
    {
        return !(*this == other);
    }

    bool operator!=(const Direction& other) const
    {
        return !(*this == other);
    }

    friend ostream & operator<<(ostream & Str, Direction const & v)
    {
	switch (v._direction)
        {
            case Compass::TOP:
                return Str << "Top";
            case Compass::BOTTOM:
                return Str << "Bottom";
            case Compass::LEFT:
                return Str << "Left";
            case Compass::RIGHT:
                return Str << "Right";
            default:
                return Str << "None";
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
        _exit = Compass::NONE;
        if (_entrance != Compass::LEFT && _entrance != Compass::RIGHT && _entrance != Compass::TOP)
        {
            return;
        }

        switch (_type)
        {
            case 0:
                break;
            case 1:
                _exit = Compass::BOTTOM;
                break;
            case 2:
            case 6:
                if (_entrance == Compass::LEFT)
                {
                    _exit = Compass::RIGHT;
                }
                else if (_entrance == Compass::RIGHT)
                {
                    _exit = Compass::LEFT;
                }
                break;
            case 3:
                if (_entrance == Compass::TOP)
                {
                    _exit = Compass::BOTTOM;
                }
                break;
            case 4:
                if (_entrance == Compass::TOP)
                {
                    _exit = Compass::LEFT;
                }
                else if (_entrance == Compass::RIGHT)
                {
                    _exit = Compass::BOTTOM;
                }
                break;
            case 5:
                if (_entrance == Compass::TOP)
                {
                    _exit = Compass::RIGHT;
                }
                else if (_entrance == Compass::LEFT)
                {
                    _exit = Compass::BOTTOM;
                }
                break;
            case 7:
                if (_entrance == Compass::TOP)
                {
                    _exit = Compass::BOTTOM;
                }
                else if (_entrance == Compass::RIGHT)
                {
                    _exit = Compass::BOTTOM;
                }
                break;
            case 8:
                if (_entrance == Compass::LEFT)
                {
                    _exit = Compass::BOTTOM;
                }
                else if (_entrance == Compass::RIGHT)
                {
                    _exit = Compass::BOTTOM;
                }
                break;
            case 9:
                if (_entrance == Compass::TOP)
                {
                    _exit = Compass::BOTTOM;
                }
                else if (_entrance == Compass::LEFT)
                {
                    _exit = Compass::BOTTOM;
                }
                break;
            case 10:
                if (_entrance == Compass::TOP)
                {
                    _exit = Compass::LEFT;
                }
                break;
            case 11:
                if (_entrance == Compass::TOP)
                {
                    _exit = Compass::RIGHT;
                }
                break;
            case 12:
                if (_entrance == Compass::RIGHT)
                {
                    _exit = Compass::BOTTOM;
                }
                break;
            case 13:
                if (_entrance == Compass::LEFT)
                {
                    _exit = Compass::BOTTOM;
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
        UpdateEntrance(Compass::TOP);
    }

    bool UpdateEntrance(const Compass direction)
    {
        if (CanEnter(direction))
        {
            _entrance = direction;
            RefreshExits();
            return true;
        }

        return false;
    }

    bool UpdateEntrance(const Direction direction)
    {
        if (direction == Compass::TOP) return UpdateEntrance(Compass::TOP);
        if (direction == Compass::LEFT) return UpdateEntrance(Compass::LEFT);
        if (direction == Compass::RIGHT) return UpdateEntrance(Compass::RIGHT);

        return false;
    }

    Direction GetExit()
    {
        return _exit;
    }

    bool CanEnter(Compass direction)
    {
        switch (direction)
        {
            case Compass::TOP:
                switch (_type)
                {
                    case 1:
                    case 3:
                    case 4:
                    case 5:
                    case 7:
                    case 9:
                    case 10:
                    case 11:
                        return true;
                    default:
                        return false;
                }
            case Compass::LEFT:
                switch (_type)
                {
                    case 1:
                    case 2:
                    case 5:
                    case 6:
                    case 8:
                    case 9:
                    case 13:
                        return true;
                    default:
                        return false;
                }
            case Compass::RIGHT:
                switch (_type)
                {
                    case 1:
                    case 2:
                    case 4:
                    case 6:
                    case 7:
                    case 8:
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
            case Compass::BOTTOM:
                return Coordinate(_location->x, _location->y + 1);
            case Compass::LEFT:
                return Coordinate(_location->x - 1, _location->y);
            case Compass::RIGHT:
                return Coordinate(_location->x + 1, _location->y);
            default:
                return *_location;
        }
    }

    string RotateLeft()
    {
        if (!_canRotate)
        {
            return "WAIT";
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

        return to_string(_location->x) + " " +  to_string(_location->y) + " LEFT";
    }

    string RotateRight()
    {
        if (!_canRotate)
        {
            return "WAIT";
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

        return to_string(_location->x) + " " +  to_string(_location->y) + " RIGHT";
    }

    Room& operator=(const Room &r) 
    {
        _entrance = r._entrance;
        _exit = r._exit;
        _type = r._type;
        _canRotate = r._canRotate;
        _location = r._location;

        return *this;
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

    void UpdateLocation(int x, int y, const string entrance)
    {
        cerr << "UpdateLocation(" << x << ", " << y << ", " << entrance << ")" << endl;
        Coordinate location(x, y);
        cerr << "UpdateLocation location: " << location << endl;
        _current = Rooms[location];
        cerr << "UpdateLocation _current: " << *_current << endl;
        _current->UpdateEntrance(compassMap[entrance]);
        cerr << "UpdateLocation updated the entrance" << endl;
    }

    Room* GetEnd(Coordinate* goal)
    {
        Room* iterator = _current;
        while(1)
        {
            Direction exit = iterator->GetExit();
            if (exit == Compass::NONE)
            {
                return iterator;
            }

            Direction nextEntrance = exit.GetEntranceFromExit();
            Coordinate nextLocation = iterator->GetNextLocation();
            iterator = Rooms[nextLocation];
            cerr << "iterator to " << nextLocation << " using entrance " << nextEntrance << endl;
            if (*iterator == *goal)
            {
                return iterator;
            }

            if (!iterator->UpdateEntrance(nextEntrance))
            {
                return iterator;
            }
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

        Room* end = Indy.GetEnd(exit);
        cerr << "Indy ends at: " << *end << endl;

        if (*end == *exit)
        {
            cout << "WAIT" << endl;           
        }
        else
        {
            cout << end->RotateLeft() << endl;           
        }
    }
}