#include <stdio.h>

typedef struct {
    int pid;
    int arrival;
    int burst;
    int rem;     // remaining time
    int start;
    int completion;
    int turnaround;
    int waiting;
    int finished;
} PCB;

int main(void) {
    
    PCB processes[] = {
        {1, 0, 5, 5, 0, 0, 0, 0, 0},
        {2, 1, 3, 3, 0, 0, 0, 0, 0},
        {3, 2, 4, 4, 0, 0, 0, 0, 0}
    };

    int n = sizeof(processes)/sizeof(processes[0]);
    int quantum = 2;
    int time = 0, completed = 0;
    double total_tat = 0, total_wt = 0;

    printf("Gantt chart (RR, q=%d):\n", quantum);

    int queue[100], head = 0, tail = 0;
    int inq[10] = {0};

    // enqueue processes that arrive at t=0
    for (int i = 0; i < n; i++)
        if (processes[i].arrival == 0) {
            queue[tail++] = i;
            inq[i] = 1;
        }

    while (completed < n) {
        if (head == tail) {
            // no ready processes → advance time
            int nextArrival = 1<<30;
            for (int i = 0; i < n; i++)
                if (!processes[i].finished && processes[i].arrival < nextArrival)
                    nextArrival = processes[i].arrival;
            printf("[idle %d->%d] ", time, nextArrival);
            time = nextArrival;
            for (int i = 0; i < n; i++)
                if (!processes[i].finished && processes[i].arrival == time && !inq[i]) {
                    queue[tail++] = i;
                    inq[i] = 1;
                }
            continue;
        }

        int idx = queue[head++];
        inq[idx] = 0;

        if (processes[idx].start == 0 && time >= processes[idx].arrival)
            processes[idx].start = time;

        int run = (processes[idx].rem < quantum) ? processes[idx].rem : quantum;
        printf("[P%d %d->%d] ", processes[idx].pid, time, time+run);
        processes[idx].rem -= run;
        int oldtime = time;
        time += run;

        // enqueue any new arrivals during this slice
        for (int j = 0; j < n; j++) {
            if (!processes[j].finished && !inq[j] && processes[j].arrival > oldtime && processes[j].arrival <= time) {
                queue[tail++] = j;
                inq[j] = 1;
            }
        }

        if (processes[idx].rem == 0) {
            processes[idx].completion = time;
            processes[idx].turnaround = processes[idx].completion - processes[idx].arrival;
            processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
            processes[idx].finished = 1;
            completed++;
            total_tat += processes[idx].turnaround;
            total_wt += processes[idx].waiting;
        } else {
            // put it back in queue
            queue[tail++] = idx;
            inq[idx] = 1;
        }
    }

    printf("\n\nPID Arr Burst Completion TAT WT\n");
    for (int i = 0; i < n; i++) {
        printf("%3d %4d %5d %10d %3d %3d\n",
               processes[i].pid, processes[i].arrival, processes[i].burst,
               processes[i].completion, processes[i].turnaround, processes[i].waiting);
    }

    printf("\nAverage TAT = %.2f, Average WT = %.2f\n",
           total_tat/n, total_wt/n);

    return 0;
}