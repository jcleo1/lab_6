// C program for implementation of Simulation 
#include <stdio.h> 
#include <limits.h>
#include <stdlib.h>
#include "process.h"
#include "util.h"

#define MAX_LINE 256
#define DEFAULT_QUANTUM 2

// Function to find the waiting time for all processes using FCFS accounting for arrival times
void findWaitingTime(ProcessType plist[], int n)
{ 
    int current_time = 0;

    for (int i = 0; i < n; i++) {
        // If CPU is idle waiting for process arrival
        if (current_time < plist[i].art) {
            current_time = plist[i].art;
        }
        plist[i].wt = current_time - plist[i].art;
        if (plist[i].wt < 0) {
            plist[i].wt = 0;
        }
        current_time += plist[i].bt;
    }
} 
  
// Function to calculate turnaround time: tat = wt + bt
void findTurnAroundTime(ProcessType plist[], int n)
{ 
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].bt + plist[i].wt; 
    }
} 
  
// Function to sort processes by priority (higher priority value executes first)
int my_comparer(const void *this, const void *that)
{ 
    const ProcessType *p1 = (const ProcessType *)this;
    const ProcessType *p2 = (const ProcessType *)that;

    // Highest priority first (descending)
    if (p2->pri != p1->pri) {
        return p2->pri - p1->pri;
    }
    // Tie-breaker: earlier arrival time first
    return p1->art - p2->art;
} 

// Function to calculate waiting time for Shortest Job First (SRTF / Preemptive SJF)
void findWaitingTimeSJF(ProcessType plist[], int n)
{ 
    int rem_bt[n]; 
    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt; 
    }
  
    int complete = 0, t = 0, minm = INT_MAX; 
    int shortest = 0, finish_time; 
    int check = 0; 
  
    while (complete != n) { 
        minm = INT_MAX;
        check = 0;

        for (int j = 0; j < n; j++) { 
            if ((plist[j].art <= t) && (rem_bt[j] < minm) && rem_bt[j] > 0) { 
                minm = rem_bt[j]; 
                shortest = j; 
                check = 1; 
            } 
        } 
  
        // CPU idle step
        if (check == 0) { 
            t++; 
            continue; 
        } 
  
        rem_bt[shortest]--; 
  
        if (rem_bt[shortest] == 0) { 
            complete++; 
            check = 0; 
            finish_time = t + 1; 
  
            plist[shortest].wt = finish_time - plist[shortest].bt - plist[shortest].art; 
            if (plist[shortest].wt < 0) {
                plist[shortest].wt = 0; 
            }
        } 
        t++; 
    } 
} 

// Function to calculate waiting time for Round Robin accounting for arrival times
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    int rem_bt[n]; 
    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt; 
        plist[i].wt = 0;
    }
  
    int t = 0;
    int complete = 0;
  
    while (complete != n) 
    { 
        int executed = 0;
  
        for (int i = 0; i < n; i++) 
        { 
            // Only execute if process has arrived and needs CPU time
            if (plist[i].art <= t && rem_bt[i] > 0) 
            { 
                executed = 1;

                if (rem_bt[i] > quantum) 
                { 
                    t += quantum; 
                    rem_bt[i] -= quantum; 
                } 
                else
                { 
                    t = t + rem_bt[i]; 
                    plist[i].wt = t - plist[i].bt - plist[i].art; 
                    if (plist[i].wt < 0) {
                        plist[i].wt = 0;
                    }
                    rem_bt[i] = 0; 
                    complete++;
                } 
            } 
        } 
  
        // Advance clock if CPU is idle because no ready process has arrived yet
        if (!executed) {
            t++;
        }
    } 
} 

// Functions to calculate average time
void findavgTimeFCFS(ProcessType plist[], int n) 
{ 
    findWaitingTime(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n******** FCFS ********\n");
}

void findavgTimeSJF(ProcessType plist[], int n) 
{ 
    findWaitingTimeSJF(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n******** SJF ********\n");
}

void findavgTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    findWaitingTimeRR(plist, n, quantum); 
    findTurnAroundTime(plist, n); 
    printf("\n******** RR (Quantum = %d) ********\n", quantum);
}

void findavgTimePriority(ProcessType plist[], int n) 
{ 
    // Sort processes by priority using qsort
    qsort(plist, n, sizeof(ProcessType), my_comparer);

    // Apply waiting time calculation
    findWaitingTime(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n******** Priority ********\n");
}

void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcess ID\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n"); 
  
    for (int i = 0; i < n; i++) 
    { 
        total_wt = total_wt + plist[i].wt; 
        total_tat = total_tat + plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].art, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

ProcessType * initProc(char *filename, int *n) 
{
    FILE *input = fopen(filename, "r");
    if (!input) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }

    int count = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), input)) {
        if (line[0] != '\n' && line[0] != '#') {
            count++;
        }
    }
    rewind(input);

    ProcessType *plist = (ProcessType *)malloc(sizeof(ProcessType) * count);
    if (!plist) {
        fclose(input);
        return NULL;
    }

    int i = 0;
    while (fgets(line, sizeof(line), input)) {
        if (line[0] != '\n' && line[0] != '#') {
            sscanf(line, "%d %d %d %d", &plist[i].pid, &plist[i].art, &plist[i].bt, &plist[i].pri);
            plist[i].wt = 0;
            plist[i].tat = 0;
            i++;
        }
    }
    fclose(input);
    *n = count;
    return plist;
}

// Driver code 
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = DEFAULT_QUANTUM;
    ProcessType *proc_list;
  
    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path> [quantum]\n");
        return 1;
    }
    if (argc >= 3) {
        quantum = atoi(argv[2]);
    }
    
    // FCFS
    proc_list = initProc(argv[1], &n);
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // SJF
    proc_list = initProc(argv[1], &n);
    findavgTimeSJF(proc_list, n); 
    printMetrics(proc_list, n);
    free(proc_list);

    // Priority
    proc_list = initProc(argv[1], &n);
    findavgTimePriority(proc_list, n); 
    printMetrics(proc_list, n);
    free(proc_list);

    // RR
    proc_list = initProc(argv[1], &n);
    findavgTimeRR(proc_list, n, quantum); 
    printMetrics(proc_list, n);
    free(proc_list);
    
    return 0; 
}