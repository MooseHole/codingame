using System;
using System.Linq;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;

class Room
{
    public int ID;
    public int Cash;
    public int[] exits = new int[2];

    public Room(string initializer)
    {
        var attributes = initializer.Split(' ');
        ID = int.Parse(attributes[0]);
        Cash = int.Parse(attributes[1]);
        exits[0] = attributes[2] == "E" ? -1 : int.Parse(attributes[2]);
        exits[1] = attributes[3] == "E" ? -1 : int.Parse(attributes[3]);
    }
}

struct RoomPath
{
    public int room;
    public HashSet<int> path;
    public int cash;

    public RoomPath(int room, HashSet<int> oldPath, int cash)
    {
        this.room = room;
        this.path = new HashSet<int>(oldPath) { room };
        this.cash = cash;
    }

    public bool Contains(int roomNumber)
    {
        return path.Contains(roomNumber);
    }

    public override string ToString()
    {
        return $"RoomPath room {room} path {string.Join("-", path)} cash ${cash}";
    }
}

class Solution
{
    static Dictionary<int, Room> rooms = new Dictionary<int, Room>();

    static int EvaluateRoom(int initialRoom)
    {
        Queue<RoomPath> process = new Queue<RoomPath>();
        HashSet<int> initialPath = new HashSet<int>() { initialRoom };
        var initialRoomPath = new RoomPath(initialRoom, initialPath, rooms[initialRoom].Cash);
        Console.Error.WriteLine($"Queueing initial room: {initialRoomPath}");
        process.Enqueue(initialRoomPath);

        int bestCash = -1;
        while (process.Any())
        {
            RoomPath roomPath = process.Dequeue();

            foreach (var exit in rooms[roomPath.room].exits)
            {
                if (roomPath.Contains(exit))
                {
                    Console.Error.WriteLine($"Duplicate room found from {roomPath.room} to {exit}");
                    continue;
                }

                if (exit < 0)
                {
                    Console.Error.WriteLine($"Found outside exit.  Checking {roomPath}");
                    bestCash = Math.Max(bestCash, roomPath.cash);
                }
                else
                {
                    var newRoomPath = new RoomPath(exit, roomPath.path, rooms[exit].Cash + roomPath.cash);
                    Console.Error.WriteLine($"Queueing exit from room {roomPath.room}: {newRoomPath}");
                    process.Enqueue(newRoomPath);
                }
            }
        }

        return bestCash;
    }

    static void Main(string[] args)
    {
        string input = Console.ReadLine();
        Console.Error.WriteLine(input);
        int N = int.Parse(input);
        for (int i = 0; i < N; i++)
        {
            input = Console.ReadLine();
            Console.Error.WriteLine(input);
            var room = new Room(input);
            rooms.Add(room.ID, room);
        }

        Console.WriteLine(EvaluateRoom(0));
    }
}