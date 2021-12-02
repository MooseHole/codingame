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

class Solution
{
    static Dictionary<int, Room> rooms = new Dictionary<int, Room>();

    static int EvaluateRoom(int initialRoom)
    {
        int currentRoomNum = initialRoom;
        rooms[initialRoom].Deposit(0);

        do
        {
            foreach (var exit in rooms[currentRoomNum].exits)
            {
                if (exit >= 0)
                {
                    Console.Error.WriteLine($"Depositing {rooms[currentRoomNum].BestCash} from {currentRoomNum} to {exit}");
                    rooms[exit].Deposit(rooms[currentRoomNum].BestCash);
                }
            }

            rooms[currentRoomNum].Visited = true;
            currentRoomNum = rooms.Values.Where(r => !r.Visited).OrderByDescending(r => r.BestCash).FirstOrDefault()?.ID ?? -1;
        } while (currentRoomNum >= 0);

        return rooms.Values.Max(r => r.BestCash);
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