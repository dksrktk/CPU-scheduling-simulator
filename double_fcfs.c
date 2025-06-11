#include <stdio.h>
#include <stdlib.h>
#include "process.h"

static GanttSlot gantt_log[2][1000];
static int gantt_len[2];

static int cmp_arrival(const void* a, const void* b) {
    const Process* const* pp1 = a;
    const Process* const* pp2 = b;
    const Process* p1 = *pp1;
    const Process* p2 = *pp2;
    if (p1->arrival_time < p2->arrival_time) return -1;
    if (p1->arrival_time > p2->arrival_time) return  1;
    return 0;
}

int cmp_process(Process* p1, Process* p2)
{
    if(p2 == NULL)
        return 1;
    if(p1->pid == p2->pid)
        return 0;
    return 1;
}

void run_double_fcfs(Process* processes, int n)
{
    Process* ptrs[n];
    int current_time = 0;
    Process* running[2] = {NULL, NULL};
    int total_waiting = 0, total_turnaround = 0, total_process_time = 0;
    for(int i = 0; i < n; i++)
    {
        total_process_time += processes[i].burst_time;
        ptrs[i] = &processes[i];
    }

    while(total_process_time > 0)
    {
        qsort(ptrs, n, sizeof(ptrs[0]), cmp_arrival);
        if(running[0] == NULL)
            for(int i = 0; i < n; i++)
                if(ptrs[i]->arrival_time <= current_time && ptrs[i]->remaining_time > 0 && cmp_process(ptrs[i], running[1]))
                {
                    running[0] = ptrs[i];
                    break;
                }
        if(running[1] == NULL)
            for(int i = 0; i < n; i++)
                if(ptrs[i]->arrival_time <= current_time && ptrs[i]->remaining_time > 0 && cmp_process(ptrs[i], running[0]))
                {
                    running[1] = ptrs[i];
                    break;
                }
        if(running[0] != NULL || running[1] != NULL)
        {
            if(running[0] != NULL)
            {
                Process* pt = running[0];
                if(pt->burst_time == pt->remaining_time)
                    pt->start_time = current_time;
                pt->remaining_time --;
                if(pt->remaining_time == 0)
                {
                    pt->finish_time = current_time + 1;
                    pt->turnaround_time = pt->finish_time - pt->original_arrival_time;
                    pt->waiting_time = pt->turnaround_time - pt->burst_time - pt->total_io_time;
                    running[0] = NULL;

                    total_waiting += pt->waiting_time;
                    total_turnaround += pt->turnaround_time;
                }
                if (pt->io_index < pt->io_num && pt->burst_time - pt->remaining_time == pt->io_start_time[pt->io_index])
                {
                    pt->arrival_time = current_time + pt->io_duration[pt->io_index] + 1;
                    pt->io_index++;
                    running[0] = NULL;
                }

                gantt_log[0][gantt_len[0]].time = current_time;
                gantt_log[0][gantt_len[0]].pid = pt->pid;
                gantt_len[0]++;
                total_process_time --;
            }
            if(running[1] != NULL)
            {
                Process* pt = running[1];
                if(pt->burst_time == pt->remaining_time)
                    pt->start_time = current_time;
                pt->remaining_time --;
                if(pt->remaining_time == 0)
                {
                    pt->finish_time = current_time + 1;
                    pt->turnaround_time = pt->finish_time - pt->original_arrival_time;
                    pt->waiting_time = pt->turnaround_time - pt->burst_time - pt->total_io_time;
                    running[1] = NULL;

                    total_waiting += pt->waiting_time;
                    total_turnaround += pt->turnaround_time;
                }
                if (pt->io_index < pt->io_num && pt->burst_time - pt->remaining_time == pt->io_start_time[pt->io_index])
                {
                    pt->arrival_time = current_time + pt->io_duration[pt->io_index] + 1;
                    pt->io_index++;
                    running[1] = NULL;
                }

                gantt_log[1][gantt_len[1]].time = current_time;
                gantt_log[1][gantt_len[1]].pid = pt->pid;
                gantt_len[1]++;
                total_process_time --;
            }
        }
        current_time ++;
    }

    print_gantt(gantt_log[0], gantt_len[0]);
    print_gantt(gantt_log[1], gantt_len[1]);
    printf("\n[Results]\n");
    for(int i = 0; i < n; i++)
        printf("P%d: Waiting = %d, Turnaround = %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / n);
}
