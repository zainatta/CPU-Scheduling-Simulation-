#include "scheduler.h"
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>  
#include <stddef.h>  
// ---------------- Scheduling Algorithms ----------------
static void sort_by_arrival(Process proc[], int n) 
{
    for (int i = 0; i < n-1; i++) 
    {
        for (int j = 0; j < n-i-1; j++)
         {
            if (proc[j].arrivalTime > proc[j+1].arrivalTime) 
            {
                Process temp = proc[j];
                proc[j] = proc[j+1];
                proc[j+1] = temp;
            }
        }
    }
}


// FCFS Scheduling
Metrics fcfs_metrics(Process proc[], int n) 
{
    Metrics m = {0, 0, 0}; 
    int currentTime = 0;
    float totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;    
    sort_by_arrival(proc, n);
    for (int i = 0; i < n; i++) 
    {
        if (currentTime < proc[i].arrivalTime) 
        {
            currentTime = proc[i].arrivalTime;
        }

        int startTime = currentTime;
        int completionTime = currentTime + proc[i].burstTime;
        int turnaroundTime = completionTime - proc[i].arrivalTime;
        int waitingTime = turnaroundTime - proc[i].burstTime;
        int responseTime = startTime - proc[i].arrivalTime;
        totalTurnaround += turnaroundTime;
        totalWaiting += waitingTime;
        totalResponse += responseTime;
        currentTime = completionTime;
    }
    m.avgTurnaround = totalTurnaround / n;
    m.avgWaiting = totalWaiting / n;
    m.avgResponse = totalResponse / n;
    return m;
}

// SJF Scheduling (Non-preemptive)
Metrics sjf_metrics(Process proc[], int n) 
{
    Metrics m = {0, 0, 0};
    int currentTime = 0;
    float totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;
    Process *procCopy = (Process *)malloc(n * sizeof(Process));
    if (procCopy == NULL)
    {
        return m;
    }

    for (int i = 0; i < n; i++) 
    {
        procCopy[i] = proc[i];
    }
    
    bool *completed = (bool *)malloc(n * sizeof(bool));
    if (completed == NULL)
     {
        free(procCopy);
        return m;
    }
    for (int i = 0; i < n; i++)
     {
        completed[i] = false;
    }

    for (int i = 0; i < n-1; i++)
     {
        for (int j = 0; j < n-i-1; j++)
        {
            if (procCopy[j].arrivalTime > procCopy[j+1].arrivalTime) 
            {
                Process temp = procCopy[j];
                procCopy[j] = procCopy[j+1];
                procCopy[j+1] = temp;
            }
        }
    }

    int remaining = n;
    while (remaining > 0) 
    {
        int next = -1;
        int shortest = INT_MAX;

        for (int i = 0; i < n; i++) 
        {
            if (!completed[i] && procCopy[i].arrivalTime <= currentTime && procCopy[i].burstTime < shortest) 
            {
                shortest = procCopy[i].burstTime;
                next = i;
            }
        }
        
        if (next == -1)
        {
            currentTime++;
            continue;
        }
        
        int startTime = currentTime;
        int completionTime = currentTime + procCopy[next].burstTime;
        int turnaroundTime = completionTime - procCopy[next].arrivalTime;
        int waitingTime = turnaroundTime - procCopy[next].burstTime;
        int responseTime = startTime - procCopy[next].arrivalTime;

        totalTurnaround += turnaroundTime;
        totalWaiting += waitingTime;
        totalResponse += responseTime;

        completed[next] = true;
        currentTime = completionTime;
        remaining--;
    }
    
    free(procCopy);
    free(completed);
    m.avgTurnaround = totalTurnaround / n;
    m.avgWaiting = totalWaiting / n;
    m.avgResponse = totalResponse / n;
    return m;
}

// SJF Scheduling (Non-preemptive)
Metrics rr_metrics(Process proc[], int n, int timeQuantum) 
{
    Metrics m = {0, 0, 0};
    int currentTime = 0;
    float totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;
    Process *procCopy = (Process *)malloc(n * sizeof(Process));
    if (procCopy == NULL)
    {
        return m;
    }
    for (int i = 0; i < n; i++) 
    {
        procCopy[i] = proc[i];
        procCopy[i].remainingTime = proc[i].burstTime;
    }

    bool *responseRecorded = (bool *)malloc(n * sizeof(bool));
    if (responseRecorded == NULL) 
    {
        free(procCopy);
        return m;
    }
    for (int i = 0; i < n; i++) 
    {
        responseRecorded[i] = false;
    }
    
    int completed = 0;
    while (completed < n) 
    {
        bool anyProcessExecuted = false;
        
        for (int i = 0; i < n; i++) 
        {
            if (procCopy[i].remainingTime > 0 && procCopy[i].arrivalTime <= currentTime) 
            {
                anyProcessExecuted = true;
                if (!responseRecorded[i]) 
                {
                    totalResponse += currentTime - procCopy[i].arrivalTime;
                    responseRecorded[i] = true;
                }
                
                int executionTime = (procCopy[i].remainingTime < timeQuantum) ? 
                                   procCopy[i].remainingTime : timeQuantum;
                procCopy[i].remainingTime -= executionTime;
                currentTime += executionTime;
                
                if (procCopy[i].remainingTime == 0) 
                {
                    completed++;
                    int turnaroundTime = currentTime - procCopy[i].arrivalTime;
                    int waitingTime = turnaroundTime - procCopy[i].burstTime;
                    totalTurnaround += turnaroundTime;
                    totalWaiting += waitingTime;
                }
            }
        }
        
        if (!anyProcessExecuted) 
        {
            currentTime++;
        }
    }

    free(procCopy);
    free(responseRecorded);

    m.avgTurnaround = totalTurnaround / n;
    m.avgWaiting = totalWaiting / n;
    m.avgResponse = totalResponse / n;

    return m;
}