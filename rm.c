#include <stdio.h>
#include <stdlib.h>
#include "process.h"

static GanttSlot gantt_log[1000];
static int gantt_len;

static int cmp_period(const void* a, const void* b)
{
    const Process* p1 = (const Process*)a;
    const Process* p2 = (const Process*)b;
    int per1 = p1->deadline - p1->original_arrival_time;
    int per2 = p2->deadline - p2->original_arrival_time;
    if (per1 != per2) return per1 - per2;
    return p1->arrival_time - p2->arrival_time;
}

void run_rm(Process* processes, int n)
{
    int cpu_used;
    int current_time = 0;
    int total_waiting = 0, total_turnaround = 0, total_process_time = 0;
    for(int i = 0; i < n; i++)
        total_process_time += processes[i].burst_time;
    cpu_used = total_process_time * 100;
    while(total_process_time > 0)
    {
        int pt = -1;
        qsort(processes, n, sizeof(Process), cmp_period);
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
    printf("CPU Utilized Rate: %.2f%%\n", (float)cpu_used / current_time);
}
