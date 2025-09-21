#include <stdio.h>
#include <limits.h>

typedef struct {
    int pid;
    int arrival;
    int burst;
    int start;
    int completion;
    int turnaround;
    int waiting;
    int finished;   // bookkeeping for SJF simulation
} PCB;

int main(void) {

    PCB processes[] = {
        {1, 0, 5, 0, 0, 0, 0, 0},   /* P1 */
        {2, 2, 3, 0, 0, 0, 0, 0},   /* P2 */
        {3, 4, 1, 0, 0, 0, 0, 0}    /* P3 */
    };


    int n = sizeof(processes) / sizeof(processes[0]);
    int completed = 0;
    int time = 0;
    double total_tat = 0.0, total_wt = 0.0;

    printf("Gantt chart (SJF non-preemptive):\n");

    while(completed < n){

        int idx = -1;
        int minburst = INT_MAX; //infinity because later on there is a condition to update this, hence, it is easily replaceable

        for (int i = 0; i < n; i++){

            //find min element/ shortest job

            if(!processes[i].finished && processes[i].arrival <= time){

                if(processes[i].burst < minburst){

                    minburst = processes[i].burst;
                    idx = i;

                }

            }

        }

        if (idx == -1){

            int nextArrival = INT_MAX;

            for(int i = 0; i < n; i++){

                if(!processes[i].finished && processes[i].arrival < nextArrival){

                    nextArrival = processes[i].arrival;

                }

            }

            printf("[idle %d -> %d]", time, nextArrival);

            time = nextArrival;

            continue;

        }

        processes[idx].start = time;
        time += processes[idx].burst;
        processes[idx].completion = time;
        processes[idx].turnaround = processes[idx].completion - processes[idx].arrival;
        processes[idx].waiting = processes[idx].turnaround - processes[idx].burst;
        processes[idx].finished = 1;
        completed++;

        total_tat += processes[idx].turnaround;
        total_wt  += processes[idx].waiting;

        printf("[P%d %d->%d] ", processes[idx].pid, processes[idx].start, processes[idx].completion);


    }

}