#include <stdio.h>

typedef struct {
    int pid;
    int arrival;
    int burst;
    int start;
    int completion;
    int turnaround;
    int waiting;
} PCB;

int main(void) {
    /* Hard-coded processes (our simplified PCBs) */
    PCB processes[] = {
        {1, 0, 5, 0, 0, 0, 0},   /* P1 */
        {2, 2, 3, 0, 0, 0, 0},   /* P2 */
        {3, 4, 1, 0, 0, 0, 0}    /* P3 */
    };

    int n = sizeof(processes) / sizeof(processes[0]);
    int time = 0;
    double total_tat = 0.0, total_wt = 0.0;

    printf("Gantt chart (FCFS):\n");

    for (int i = 0; i < n; i++) {

        // this is to deal with the arrival time being greater than 0, i.e., there will be an idle time.

        if (time < processes[i].arrival) {

            printf("[idle %d->%d] ", time, processes[i].arrival);
            time = processes[i].arrival;

        }
        
    //main FCFS algorithm
    processes[i].start = time;
    time = time + processes[i].burst;
    processes[i].completion = time;

    processes[i].turnaround = processes[i].completion - processes[i].arrival;
    processes[i].waiting = processes[i].turnaround - processes[i].burst;

    total_tat = total_tat + processes[i].turnaround;
    total_wt = total_wt + processes[i].waiting;

    printf("[P%d %d -> %d]", processes[i].pid, processes[i].start, processes[i].completion);

}

}