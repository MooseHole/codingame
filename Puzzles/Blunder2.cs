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
    public bool Visited;
    public int BestCash;

    public Room(string initializer)
    {
        var attributes = initializer.Split(' ');
        ID = int.Parse(attributes[0]);
        Cash = int.Parse(attributes[1]);
        exits[0] = attributes[2] == "E" ? -1 : int.Parse(attributes[2]);
        exits[1] = attributes[3] == "E" ? -1 : int.Parse(attributes[3]);
    }

    public void Deposit(int fromRoomAmount)
    {
        BestCash = Math.Max(BestCash, fromRoomAmount + Cash);
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
        Queue<int> process = new Queue<int>();
        HashSet<int> initialPath = new HashSet<int>() { initialRoom };
        var initialRoomPath = new RoomPath(initialRoom, initialPath, rooms[initialRoom].Cash);
        Console.Error.WriteLine($"Queueing initial room: {initialRoomPath}");
        rooms[initialRoom].Deposit(0);
        process.Enqueue(initialRoom);

        int bestCash = -1;
        while (process.Any())
        {
            int currentRoomNum = process.Dequeue();
            if (rooms[currentRoomNum].Visited)
            {
                Console.Error.WriteLine($"Already visited {currentRoomNum}");
                continue;
            }

            Dictionary<int, int> exitBestCash = new Dictionary<int, int>();
            foreach (var exit in rooms[currentRoomNum].exits)
            {
                if (rooms[currentRoomNum].Visited)
                {
                    Console.Error.WriteLine($"Duplicate room found from {currentRoomNum} to {exit}");
                    continue;
                }

                if (exit < 0)
                {
                    Console.Error.WriteLine($"Found outside exit from {currentRoomNum}.  Checking {rooms[currentRoomNum].BestCash}");
                    bestCash = Math.Max(bestCash, rooms[currentRoomNum].BestCash);
                }
                else
                {
                    Console.Error.WriteLine($"Queueing exit from room {currentRoomNum}: {exit} and depositing {rooms[currentRoomNum].BestCash}");
                    rooms[exit].Deposit(rooms[currentRoomNum].BestCash);
                    exitBestCash.Add(exit, rooms[exit].BestCash);
                }
            }

            if (exitBestCash.Count == 2)
            {
                if (exitBestCash.First().Value > exitBestCash.Last().Value)
                {
                    process.Enqueue(exitBestCash.First().Key);
                    process.Enqueue(exitBestCash.Last().Key);
                }
                else
                {
                    process.Enqueue(exitBestCash.Last().Key);
                    process.Enqueue(exitBestCash.First().Key);
                }
            }
            else if (exitBestCash.Count == 1)
            {
                process.Enqueue(exitBestCash.First().Key);
            }

            rooms[currentRoomNum].Visited = true;
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