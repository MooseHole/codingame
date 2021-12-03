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
    public int BestCash;

    public Room(string initializer)
    {
        var attributes = initializer.Split(' ');
        ID = int.Parse(attributes[0]);
        Cash = int.Parse(attributes[1]);
        exits[0] = attributes[2] == "E" ? -1 : int.Parse(attributes[2]);
        exits[1] = attributes[3] == "E" ? -1 : int.Parse(attributes[3]);
    }

    public bool Deposit(int fromRoomAmount)
    {
        int newAmount = fromRoomAmount + Cash;
        if (newAmount > BestCash)
        {
            BestCash = newAmount;
            return true;
        }

        return false;
    }
}

class Solution
{
    static Dictionary<int, Room> rooms = new Dictionary<int, Room>();

    static int EvaluateRoom(int initialRoom)
    {
        int currentRoomNum = initialRoom;
        rooms[initialRoom].Deposit(0);
        Stack<int> roomStack = new Stack<int>();
        roomStack.Push(currentRoomNum);

        do
        {
            foreach (var exit in rooms[currentRoomNum].exits)
            {
                if (exit >= 0)
                {
                    // Deposit current path's cash into the exit.
                    // If the amount was updated, process the exit
                    if (rooms[exit].Deposit(rooms[currentRoomNum].BestCash))
                    {
                        roomStack.Push(exit);
                    }
                }
            }

            currentRoomNum = -1;
            if (roomStack.Any())
            {
                currentRoomNum = roomStack.Pop();
            }
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