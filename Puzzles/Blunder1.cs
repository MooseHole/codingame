using System;
using System.Linq;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;

enum Compass { North, South, East, West }

enum SquareTypes : byte
{
    PermanentObstacle = (byte)'#',
    DestructableObstacle = (byte)'X',
    Blunder = (byte)'@',
    Goal = (byte)'$',
    South = (byte)'S',
    East = (byte)'E',
    North = (byte)'N',
    West = (byte)'W',
    Beer = (byte)'B',
    Inverter = (byte)'I',
    Teleporter = (byte)'T'
}

class Coordinate
{
    public int X;
    public int Y;

    public Coordinate()
    {
        X = -1;
        Y = -1;
    }

    public Coordinate(int x, int y)
    {
        X = x;
        Y = y;
    }

    public Coordinate(Coordinate coordinate)
    {
        X = coordinate.X;
        Y = coordinate.Y;
    }

    public override bool Equals(Object obj)
    {
        //Check for null and compare run-time types.
        if ((obj == null) || !this.GetType().Equals(obj.GetType()))
        {
            return false;
        }
        else
        {
            Coordinate coordinate = (Coordinate)obj;

            return (X == coordinate.X)
                && (Y == coordinate.Y);
        }
    }

    public override int GetHashCode()
    {
        return (X << 2) ^ Y;
    }

    public static bool operator ==(Coordinate lhs, Coordinate rhs)
    {
        if (lhs is null)
        {
            if (rhs is null)
            {
                return true;
            }

            // Only the left side is null.
            return false;
        }
        // Equals handles case of null on right side.
        return lhs.Equals(rhs);
    }

    public static bool operator !=(Coordinate lhs, Coordinate rhs) => !(lhs == rhs);


    public override string ToString()
    {
        return "(" + X + ", " + Y + ")";
    }
}

class Direction
{
    public Compass Way;

    public Direction(Compass way) => Way = way;

    public Direction(Direction direction) => Way = direction.Way;

    public Coordinate NextCoordinate(Coordinate current)
    {
        switch (Way)
        {
            case Compass.North:
                return new Coordinate(current.X, current.Y - 1);
            case Compass.South:
                return new Coordinate(current.X, current.Y + 1);
            case Compass.East:
                return new Coordinate(current.X + 1, current.Y);
            case Compass.West:
                return new Coordinate(current.X - 1, current.Y);
            default:
                return current;
        }
    }

    public override bool Equals(Object obj)
    {
        //Check for null and compare run-time types.
        if ((obj == null) || !this.GetType().Equals(obj.GetType()))
        {
            return false;
        }
        else
        {
            Direction direction = (Direction)obj;

            return (Way == direction.Way);
        }
    }

    public override int GetHashCode()
    {
        return (int)Way;
    }

    public static bool operator ==(Direction lhs, Direction rhs)
    {
        if (lhs is null)
        {
            if (rhs is null)
            {
                return true;
            }

            // Only the left side is null.
            return false;
        }
        // Equals handles case of null on right side.
        return lhs.Equals(rhs);
    }

    public static bool operator !=(Direction lhs, Direction rhs) => !(lhs == rhs);

    public override string ToString()
    {
        switch (Way)
        {
            case Compass.North:
                return "NORTH";
            case Compass.South:
                return "SOUTH";
            case Compass.East:
                return "EAST";
            case Compass.West:
                return "WEST";
            default:
                return "ERROR";
        }
    }
}

class Blunder
{
    public Coordinate Location;
    public Direction Facing;
    public bool Breaker;
    public bool Inverted;
    private bool _exploringObstaclePaths = false;

    public Blunder()
    {
        Facing = new Direction(Compass.South);
    }

    public Blunder(Coordinate position)
    {
        Location = position;
        Facing = new Direction(Compass.South);
    }

    public Blunder(Blunder blunder)
    {
        Location = new Coordinate(blunder.Location);
        Facing = new Direction(blunder.Facing);
        Breaker = blunder.Breaker;
        Inverted = blunder.Inverted;
    }

    public Coordinate LocationAhead => Facing.NextCoordinate(Location);

    public void Execute(Square square, Coordinate destination)
    {
        if (square is Obstacle obstacle && (!Breaker || !obstacle.Destructable))
        {
            HitObstacle();
            Console.Error.WriteLine("Hit obstacle, new Facing: " + Facing);
            return;
        }

        _exploringObstaclePaths = false;

        if (square is Teleport teleport)
        {
            Location = teleport.Destination;
            Console.Error.WriteLine("Hit teleport, new Location: " + Location);
            return;
        }
        else if (square is Beer)
        {
            DrinkBeer();
            Console.Error.WriteLine("Drank beer, new Breaker: " + Breaker);
        }
        else if (square is Inverter)
        {
            ReversePolarity();
            Console.Error.WriteLine("Reversed polarity, new Inverted: " + Inverted);
        }
        else if (square is Directional directional)
        {
            ChangeDirection(directional.Way);
            Console.Error.WriteLine("Changed direction, new Facing: " + Facing);
        }

        Location = destination;
        Console.Error.WriteLine("Updated Location to destination: " + Location);
    }

    public bool ReversePolarity() => Inverted = !Inverted;

    public bool DrinkBeer() => Breaker = !Breaker;

    public void HitObstacle()
    {
        if (!_exploringObstaclePaths)
        {
            Facing.Way = Inverted ? Compass.West : Compass.South;
            _exploringObstaclePaths = true;
            return;
        }

        // normal:   SOUTH, EAST, NORTH, WEST
        // inverted: WEST, NORTH, EAST, SOUTH
        switch (Facing.Way)
        {
            case Compass.North:
                Facing.Way = Inverted ? Compass.East : Compass.West;
                break;
            case Compass.East:
                Facing.Way = Inverted ? Compass.South : Compass.North;
                break;
            case Compass.South:
                Facing.Way = Inverted ? Compass.West : Compass.East;
                break;
            case Compass.West:
                Facing.Way = Inverted ? Compass.North : Compass.South;
                break;
            default:
                Facing.Way = Compass.South;
                break;
        }
    }

    void ChangeDirection(Compass newDirection)
    {
        Facing.Way = newDirection;
    }

    public override bool Equals(Object obj)
    {
        //Check for null and compare run-time types.
        if ((obj == null) || !this.GetType().Equals(obj.GetType()))
        {
            return false;
        }
        else
        {
            Blunder blunder = (Blunder)obj;

            // Don't find anything equal when the obstacle paths are being navigated
            return !blunder._exploringObstaclePaths
                && (Location == blunder.Location)
                && (Facing == blunder.Facing)
                && (Breaker == blunder.Breaker)
                && (Inverted == blunder.Inverted);
        }
    }

    public override int GetHashCode()
    {
        return (Location.GetHashCode() << 4) ^ (Facing.GetHashCode() << 2) ^ (Breaker ? 0x10 : 0) ^ (Inverted ? 0x01 : 0);
    }
}

class Square
{
    public Square()
    {
    }
}

class Obstacle : Square
{
    public bool Destructable;

    public Obstacle(bool destructable) : base()
    {
        Destructable = destructable;
    }
}

class Directional : Square
{
    public Compass Way;

    public Directional(Compass compass) : base()
    {
        Way = compass;
    }
}

class Teleport : Square
{
    public Coordinate Destination;

    public Teleport() : base()
    {
    }

    public void SetDestination(Coordinate destonation) => Destination = destonation;
}

class Goal : Square
{
    public Goal() : base()
    {
    }
}

class Beer : Square
{
    public Beer() : base()
    {
    }
}
class Inverter : Square
{
    public Inverter() : base()
    {
    }
}

class Map
{
    public Dictionary<Coordinate, Square> Squares = new Dictionary<Coordinate, Square>();
}

class Solution
{
    private static readonly Map _map = new Map();

    private static Square GetSquare(char type)
    {
        SquareTypes squareType = (SquareTypes)type;

        switch (squareType)
        {
            case SquareTypes.PermanentObstacle:
                return new Obstacle(false);
            case SquareTypes.DestructableObstacle:
                return new Obstacle(true);
            case SquareTypes.Goal:
                return new Goal();
            case SquareTypes.North:
                return new Directional(Compass.North);
            case SquareTypes.South:
                return new Directional(Compass.South);
            case SquareTypes.East:
                return new Directional(Compass.East);
            case SquareTypes.West:
                return new Directional(Compass.West);
            case SquareTypes.Beer:
                return new Beer();
            case SquareTypes.Inverter:
                return new Inverter();
            case SquareTypes.Teleporter:
                return new Teleport();
            case SquareTypes.Blunder:
            default:
                return new Square();
        }
    }

    static void Main(string[] args)
    {
        string[] inputs = Console.ReadLine().Split(' ');
        int L = int.Parse(inputs[0]);
        int C = int.Parse(inputs[1]);
        List<Coordinate> teleporters = new List<Coordinate>();
        Blunder blunder = new Blunder();
        List<Blunder> breadCrumbs = new List<Blunder>();

        for (int rowNum = 0; rowNum < L; rowNum++)
        {
            string row = Console.ReadLine();
            Console.Error.WriteLine(row);

            int colNum = 0;
            foreach (var type in row)
            {
                Coordinate location = new Coordinate(colNum, rowNum);
                _map.Squares.Add(location, GetSquare(type));

                if ((SquareTypes)type == SquareTypes.Teleporter)
                {
                    teleporters.Add(location);
                }
                else if ((SquareTypes)type == SquareTypes.Blunder)
                {
                    blunder = new Blunder(location);
                }

                colNum++;
            }
        }

        // There must be either 0 or 2 teleporters
        if (teleporters.Any())
        {
            ((Teleport)(_map.Squares[teleporters[0]])).Destination = teleporters[1];
            ((Teleport)(_map.Squares[teleporters[1]])).Destination = teleporters[0];
        }

        string output = string.Empty;

        while(true)
        {
            if (breadCrumbs.Contains(blunder))
            {
                Console.WriteLine("LOOP");
                System.Environment.Exit(0);
            }

            breadCrumbs.Add(new Blunder(blunder));

            var currentLocation = new Coordinate(blunder.Location);
            var nextLocationDirection = new Direction(blunder.Facing);
            var nextLocation = blunder.LocationAhead;
            var next = _map.Squares[blunder.LocationAhead];

            blunder.Execute(next, nextLocation);

            // If Blunder destroyed an obstacle
            if (_map.Squares[blunder.Location] is Obstacle)
            {
                _map.Squares[blunder.Location] = new Square();
                breadCrumbs.Clear();
                Console.Error.WriteLine("Obstacle destroyed at " + blunder.Location);
            }

            // If move happened, record it
            if (blunder.Location != currentLocation)
            {
                output += nextLocationDirection + "\n";
            }

            if (_map.Squares[blunder.Location] is Goal)
            {
                Console.Error.WriteLine("Goal hit at " + blunder.Location);
                Console.WriteLine(output);
                System.Environment.Exit(0);
            }
        }
    }
}