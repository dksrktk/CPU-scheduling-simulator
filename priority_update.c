#include <stdio.h>
#include <stdlib.h>
#include "process.h"

static GanttSlot gantt_log[1000];
static int gantt_len;

static int cmp_priority(const void* a, const void* b)
{
    const Process* p1 = (const Process*)a;
    const Process* p2 = (const Process*)b;
    if(p1->priority != p2->priority)
        return p1->priority - p2->priority;
    return p1->arrival_time - p2->arrival_time;
}

void run_priority_update(Process* processes, int n, int alpha, int beta)
{
    qsort(processes, n, sizeof(Process), cmp_priority);

    int current_time = 0;
    int total_waiting = 0, total_turnaround = 0, total_process_time = 0;
    for(int i = 0; i < n; i++)
        total_process_time += processes[i].burst_time;

    while(total_process_time > 0)
    {
        int pt = -1;
        qsort(processes, n, sizeof(Process), cmp_priority);
        while(pt == -1)
        {
            for(int i = 0; i < n; i++)
                if(processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
                {
                    pt = i;
                    break;
                }
            if(pt == -1)
                current_time++;
        }
        for(int i = 0; i < n; i++)
            if(processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
            {
                if(i == pt)
                    processes[i].priority -= alpha;
                else
                    processes[i].priority -= beta;
            }
        if(processes[pt].burst_time == processes[pt].remaining_time)
            processes[pt].start_time = current_time;
        processes[pt].remaining_time --;
        if(processes[pt].remaining_time == 0)
        {
            processes[pt].finish_time = current_time + 1;
            processes[pt].turnaround_time = processes[pt].finish_time - processes[pt].original_arrival_time;
            processes[pt].waiting_time = processes[pt].turnaround_time - processes[pt].burst_time - processes[pt].total_io_time;

            total_waiting += processes[pt].waiting_time;
            total_turnaround += processes[pt].turnaround_time;
        }
        if (processes[pt].io_index < processes[pt].io_num && processes[pt].burst_time - processes[pt].remaining_time == processes[pt].io_start_time[processes[pt].io_index])
        {
            processes[pt].arrival_time = current_time + processes[pt].io_duration[processes[pt].io_index] + 1;
            processes[pt].io_index++;
        }

        gantt_log[gantt_len].time = current_time;
        gantt_log[gantt_len].pid = processes[pt].pid;
        gantt_len++;

        current_time++;
        total_process_time --;
    }

    print_gantt(gantt_log, gantt_len);
    printf("\n[Results]\n");
    for(int i = 0; i < n; i++)
        printf("P%d: Waiting = %d, Turnaround = %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / n);
}
