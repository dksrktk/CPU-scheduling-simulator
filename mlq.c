#include <stdio.h>
#include <stdlib.h>
#include "process.h"

static GanttSlot gantt_log[1000];
static int gantt_len;

static int ready_queue[3][100];
static int inqueue[100];
static int front[3] = {0}, rear[3] = {0};

static void enqueue(int level, int pid)
{
    ready_queue[level][rear[level]++] = pid;
}

static int dequeue(int level)
{
    return ready_queue[level][front[level]++];
}

static int is_empty(int level)
{
    return front[level] == rear[level];
}

static int size_queue(int level)
{
    return rear[level] - front[level];
}

static int cmp_arrival(const void* a, const void* b)
{
    const Process* p1 = (const Process*)a;
    const Process* p2 = (const Process*)b;
    return p1->arrival_time - p2->arrival_time;
}

void run_mlq(Process* processes, int n, int time_quantum)
{
    qsort(processes, n, sizeof(Process), cmp_arrival);
    int current_time = 0;
    int total_waiting = 0, total_turnaround = 0, total_process_time = 0;

    for(int i = 0; i < n; i++)
    {
        processes[i].level = processes[i].pid % 3;
        total_process_time += processes[i].burst_time;
    }

    while(total_process_time > 0)
    {
        int level;
        for(int i = 0; i < n; i ++)
            if(current_time >= processes[i].arrival_time && inqueue[i] == 0 && processes[i].remaining_time > 0)
            {
                enqueue(processes[i].level, i);
                inqueue[i] = 1;
            }
        for(level = 0; level < 3; level ++)
            if(!is_empty(level))
                break;
        if(level < 3)
        {
            if(level != 1)
            {
                int i = dequeue(level);
                if(processes[i].burst_time == processes[i].remaining_time)
                    processes[i].start_time = current_time;
                if(processes[i].io_index < processes[i].io_num)
                {
                    int process_time = processes[i].io_start_time[processes[i].io_index] - processes[i].burst_time + processes[i].remaining_time;
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
                    inqueue[i] = 0;
                }
                else
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
                        gantt_len ++;
                    }
                    current_time += processes[i].remaining_time;
                    processes[i].remaining_time = 0;
                }
            }
            else
            {
                int pt = dequeue(1);
                if(processes[pt].burst_time == processes[pt].remaining_time)
                    processes[pt].start_time = current_time;
                int io_time;
                if(processes[pt].io_index < processes[pt].io_num)
                    io_time = processes[pt].io_start_time[processes[pt].io_index] - processes[pt].burst_time + processes[pt].remaining_time;
                else
                    io_time = processes[pt].remaining_time;
                int process_time = time_quantum < io_time ? time_quantum : io_time;

                processes[pt].remaining_time -= process_time;
                for(int j = 0; j < process_time; j ++)
                {
                    gantt_log[gantt_len].time = current_time + j;
                    gantt_log[gantt_len].pid = processes[pt].pid;
                    gantt_len ++;
                }

                if(processes[pt].remaining_time == 0)
                {
                    processes[pt].finish_time = current_time + process_time;
                    processes[pt].turnaround_time = processes[pt].finish_time - processes[pt].original_arrival_time;
                    processes[pt].waiting_time = processes[pt].turnaround_time - processes[pt].burst_time - processes[pt].total_io_time;
                    total_waiting += processes[pt].waiting_time;
                    total_turnaround += processes[pt].turnaround_time;
                }
                else if(processes[pt].io_start_time[processes[pt].io_index] == processes[pt].burst_time - processes[pt].remaining_time)
                {
                    inqueue[pt] = 0;
                    processes[pt].arrival_time = current_time + process_time + processes[pt].io_duration[processes[pt].io_index];
                    processes[pt].io_index ++;
                }
                else
                    enqueue(1, pt);
                current_time += process_time;
                total_process_time -= process_time;
            }
        }
        else
            current_time++;
    }

    print_gantt(gantt_log, gantt_len);
    printf("\n[Results]\n");
    for(int i = 0; i < n; i++)
        printf("P%d: Waiting = %d, Turnaround = %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / n);
}
