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
    public string BestPath;

    public Room(string initializer)
    {
        Console.Error.WriteLine($"Room({initializer})");
        var attributes = initializer.Split(' ');
        ID = int.Parse(attributes[0]);
        Cash = int.Parse(attributes[1]);
        exits[0] = attributes[2] == "E" ? -1 : int.Parse(attributes[2]);
        exits[1] = attributes[3] == "E" ? -1 : int.Parse(attributes[3]);
    }

    public void DespositCash(int checkCash, string checkPath)
    {
        if (checkCash > BestCash)
        {
            BestCash = checkCash;
            BestPath = checkPath;
        }
    }
}

class Solution
{
    static Dictionary<int, Room> rooms = new Dictionary<int, Room>();

    static int EvaluateRoom(int roomNumber, string path, int pathCash)
    {
        var roomString = "[" + roomNumber + "]";

        Room current = rooms[roomNumber];
        int myPathCash = current.Cash + pathCash;
        current.DespositCash(myPathCash, path + roomString);

        int bestCash = -1;
        foreach (var exit in rooms[roomNumber].exits)
        {
            var exitString = "[" + exit + "]";
            if (path.Contains(exitString))
            {
                Console.Error.WriteLine(path + " Contains " + exitString);
                continue;
            }

            if (exit < 0)
            {
                Console.Error.WriteLine(current.BestPath + " " + current.BestCash);
                return current.BestCash;
            }
            else
            {
                Console.Error.WriteLine($"Recursing from room {roomNumber}: EvaluateRoom({exit}, {current.BestPath}, {current.BestCash})");
                bestCash = Math.Max(bestCash, EvaluateRoom(exit, current.BestPath, current.BestCash));
            }
        }

        return bestCash;
    }

    static void Main(string[] args)
    {
        int N = int.Parse(Console.ReadLine());
        for (int i = 0; i < N; i++)
        {
            var room = new Room(Console.ReadLine());
            rooms.Add(room.ID, room);
        }


        Console.WriteLine(EvaluateRoom(0, string.Empty, 0));
    }
}