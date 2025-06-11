#include <stdio.h>
#include <stdlib.h>
#include "process.h"

static GanttSlot gantt_log[1000];
static int gantt_len;

static int cmp_arrival(const void* a, const void* b)
{
    const Process* p1 = (const Process*)a;
    const Process* p2 = (const Process*)b;
    return p1->arrival_time - p2->arrival_time;
}

void run_fcfs(Process* processes, int n)
{
    qsort(processes, n, sizeof(Process), cmp_arrival);

    int current_time = 0;
    int total_waiting = 0, total_turnaround = 0, total_process_time = 0;

    for(int i = 0; i < n; i++)
        total_process_time += processes[i].burst_time;
    while(total_process_time > 0)
    {
        qsort(processes, n, sizeof(Process), cmp_arrival);
        int i;
        for(i = 0; i < n; i++)
            if(processes[i].remaining_time > 0)
            {
                if(current_time < processes[i].arrival_time)
                   current_time = processes[i].arrival_time;
                break;
            }
        if(processes[i].io_num == 0)
        {
            processes[i].start_time = current_time;
            processes[i].finish_time = current_time + processes[i].burst_time;
            processes[i].waiting_time = processes[i].start_time - processes[i].original_arrival_time;
            processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;

            total_waiting += processes[i].waiting_time;
            total_turnaround += processes[i].turnaround_time;
            total_process_time -= processes[i].remaining_time;
            processes[i].remaining_time = 0;
            for(int j = processes[i].start_time; j < processes[i].finish_time; j ++)
            {
                gantt_log[gantt_len].time = j;
                gantt_log[gantt_len].pid = processes[i].pid;
                gantt_len ++;
            }
            current_time += processes[i].burst_time;
        }
        else
        {
            if(processes[i].io_index == 0)
            {
                processes[i].start_time = current_time;
                int process_time = processes[i].io_start_time[processes[i].io_index];
                for(int j = current_time; j <  current_time + process_time; j ++)
                {
                    gantt_log[gantt_len].time = j;
                    gantt_log[gantt_len].pid = processes[i].pid;
                    gantt_len ++;
                }
                processes[i].arrival_time = current_time + process_time + processes[i].io_duration[processes[i].io_index];
                processes[i].remaining_time -= process_time;
                total_process_time -= process_time;
                processes[i].io_index ++;
                current_time += process_time;
            }
            else if(processes[i].io_index == processes[i].io_num)
            {
                processes[i].finish_time = current_time + processes[i].remaining_time;
                processes[i].waiting_time = processes[i].finish_time - processes[i].burst_time - processes[i].original_arrival_time  - processes[i].total_io_time;
                processes[i].turnaround_time = processes[i].finish_time - processes[i].original_arrival_time;
                total_waiting += processes[i].waiting_time;
                total_turnaround += processes[i].turnaround_time;
                total_process_time -= processes[i].remaining_time;
                for(int j = current_time; j <  current_time + processes[i].remaining_time; j ++)
                {
                    gantt_log[gantt_len].time = j;
                    gantt_log[gantt_len].pid = processes[i].pid;
                    gantt_log[gantt_len].io = 0;
                    gantt_len ++;
                }
                current_time += processes[i].remaining_time;
                processes[i].remaining_time = 0;
            }
            else
            {
                int process_time = processes[i].io_start_time[processes[i].io_index] - processes[i].io_start_time[processes[i].io_index - 1];
                for(int j = current_time; j <  current_time + process_time; j ++)
                {
                    gantt_log[gantt_len].time = j;
                    gantt_log[gantt_len].pid = processes[i].pid;
                    gantt_len ++;
                }
                gantt_log[gantt_len].io = 1;
                processes[i].arrival_time = current_time + process_time + processes[i].io_duration[processes[i].io_index];
                processes[i].remaining_time -= process_time;
                total_process_time -= process_time;
                processes[i].io_index ++;
                current_time += process_time;
            }
        }
    }

    print_gantt(gantt_log, gantt_len);
    printf("\n[Results]\n");
    for (int i = 0; i < n; ++i)
        printf("P%d: Waiting = %d, Turnaround = %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / n);
}
