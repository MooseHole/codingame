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
        Console.Error.WriteLine($"Room({initializer})");
        var attributes = initializer.Split(' ');
        ID = int.Parse(attributes[0]);
        Cash = int.Parse(attributes[1]);
        exits[0] = attributes[2] == "E" ? -1 : int.Parse(attributes[2]);
        exits[1] = attributes[3] == "E" ? -1 : int.Parse(attributes[3]);
    }
}

class Solution
{
    static void Main(string[] args)
    {
        int N = int.Parse(Console.ReadLine());
        Dictionary<int, Room> rooms = new Dictionary<int, Room>();
        for (int i = 0; i < N; i++)
        {
            var room = new Room(Console.ReadLine());
            rooms.Add(room.ID, room);
        }

        bool[] visited = new bool[N];
        int[] cash = new int[N];
        List<int> queue = new List<int>();
        visited[0] = true;
        queue.Add(0);
        int bestCash = 0;
        while (queue.Any())
        {
            // Dequeue a vertex from queue
            // and print it
            int roomNumber = queue.First();
            Console.Error.Write(roomNumber + " ");
            queue.RemoveAt(0);
            cash[roomNumber] += rooms[roomNumber].Cash;

            // Get all adjacent vertices of the
            // dequeued vertex s. If a adjacent
            // has not been visited, then mark it
            // visited and enqueue it
            foreach (var exit in rooms[roomNumber].exits)
            {
                if (exit < 0)
                {
                    Console.Error.WriteLine("$" + cash[roomNumber]);
                    bestCash = Math.Max(bestCash, cash[roomNumber]);
                }
                else
                {
                    if (!visited[exit])
                    {
                        visited[exit] = true;
                        cash[exit] += cash[roomNumber];
                        queue.Add(exit);
                    }
                }
            }
        }

        Console.WriteLine(bestCash);
    }
}