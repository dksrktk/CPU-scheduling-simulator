#include <stdio.h>
#include <stdlib.h>
#include "process.h"

static GanttSlot gantt_log[1000];
static int gantt_len;

static int cmp_remaining(const void* a, const void* b)
{
    const Process* p1 = (const Process*)a;
    const Process* p2 = (const Process*)b;
    if (p1->remaining_time != p2->remaining_time)
        return p1->remaining_time - p2->remaining_time;
    return p1->arrival_time - p2->arrival_time;
}

void run_sjf_np(Process* processes, int n)
{
    int current_time = 0;
    int total_waiting = 0, total_turnaround = 0, total_process_time = 0;
    for (int i = 0; i < n; i++)
        total_process_time += processes[i].burst_time;

    while (total_process_time > 0)
    {
        qsort(processes, n, sizeof(Process), cmp_remaining);

        int pt = -1;
        while(pt == -1)
        {
            for(int i = 0; i < n; i++)
                if(processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time)
                {
                    pt = i;
                    break;
                }
            if (pt == -1)
                current_time++;
        }

        if(processes[pt].io_index == 0)
            processes[pt].start_time = current_time;

        if(processes[pt].io_num == processes[pt].io_index)
        {
            processes[pt].finish_time = current_time + processes[pt].remaining_time;
            processes[pt].waiting_time = processes[pt].finish_time - processes[pt].total_io_time - processes[pt].burst_time - processes[pt].original_arrival_time;
            processes[pt].turnaround_time = processes[pt].finish_time - processes[pt].original_arrival_time;

            for(int j = current_time; j < processes[pt].finish_time; j ++)
            {
                gantt_log[gantt_len].time = j;
                gantt_log[gantt_len].pid = processes[pt].pid;
                gantt_log[gantt_len].io = 0;
                gantt_len ++;
            }

            total_waiting    += processes[pt].waiting_time;
            total_turnaround += processes[pt].turnaround_time;
            total_process_time -= processes[pt].remaining_time;
            current_time     = processes[pt].finish_time;
            processes[pt].remaining_time = 0;
        }
        else
        {
            int seg_start = 0;
            if(processes[pt].io_index > 0)
               seg_start = processes[pt].io_start_time[processes[pt].io_index - 1];
            int seg_end = processes[pt].io_start_time[processes[pt].io_index];
            int run_len = seg_end - seg_start;

            for(int j = current_time; j < current_time + run_len; j ++)
            {
                gantt_log[gantt_len].time = j;
                gantt_log[gantt_len].pid = processes[pt].pid;

                gantt_len ++;
            }
            gantt_log[gantt_len].io = 1;
            processes[pt].remaining_time -= run_len;
            total_process_time -= run_len;
            current_time += run_len;
            processes[pt].io_index++;
            processes[pt].arrival_time = current_time + processes[pt].io_duration[processes[pt].io_index - 1];
        }
    }

    print_gantt(gantt_log, gantt_len);
    printf("\n[Results]\n");
    for (int i = 0; i < n; ++i)
        printf("P%d: Waiting = %d, Turnaround = %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / n);
}
