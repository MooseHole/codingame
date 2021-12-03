using System;
using System.Linq;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;

class Task
{
    public readonly int StartDate;
    public readonly int EndDate;

    public Task (int startDate, int duration)
    {
        StartDate = startDate;
        EndDate = StartDate + duration - 1;
    }
}

class Solution
{
    static List<Task> tasks = new List<Task>();

    static void Main(string[] args)
    {
        int N = int.Parse(Console.ReadLine());
        for (int i = 0; i < N; i++)
        {
            string[] inputs = Console.ReadLine().Split(' ');
            int J = int.Parse(inputs[0]);
            int D = int.Parse(inputs[1]);

            tasks.Add(new Task(J, D));
        }

        var tasksByEndDate = tasks.OrderBy(t => t.EndDate);

        Task previousTask = new Task(-1, -1);
        int numScheduled = 0;

        foreach (var task in tasksByEndDate)
        {
            if (task.StartDate > previousTask.EndDate)
            {
                previousTask = task;
                numScheduled++;
            }
        }


        Console.WriteLine(numScheduled);
    }
}