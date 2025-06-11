#include <stdio.h>
#include <stdlib.h>
#include "process.h"

static GanttSlot gantt_log[1000];
static int gantt_len;

static int ready_queue[3][100];
static int inqueue[100];
static int front[3] = {0}, rear[3] = {0};
static int ready_time[100];

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

void print_queue(Process* processes, int level)
{
    printf("Q%d : {", level);
    for(int i = front[level]; i < rear[level]; i ++)
        printf("p%d ", processes[ready_queue[level][i]].pid);
    printf("}\n");
}

void debug(Process* processes, int n)
{
    printf("remaining ");
    for(int i = 0; i < n; i ++)
        printf("p%d %d, ",processes[i].pid, processes[i].remaining_time);
    printf("\n");
    for(int i = 0; i < 3; i ++)
        print_queue(processes, i);
}

void run_mlfq(Process* processes, int n)
{
    qsort(processes, n, sizeof(Process), cmp_arrival);
    int num[3];
    int current_time = 0;
    int total_waiting = 0, total_turnaround = 0, total_process_time = 0;
    int time_quantum[2];
    time_quantum[0] = 2;
    time_quantum[1] = 4;
    for(int i = 0; i < 3; i ++)
        num[i] = 0;
    for(int i = 0; i < n; i++)
    {
        processes[i].level = 0;
        total_process_time += processes[i].burst_time;
    }
    while(total_process_time > 0)
    {
        int process_time, level, pt;
        int runned_lev = -1;
        for(int i = 0; i < n; i ++)
            if(processes[i].arrival_time <= current_time && inqueue[i] == 0)
            {
                enqueue(processes[i].level, i);
                inqueue[i] = 1;
            }
//        debug(processes, n);
        for(level = 0; level < 3; level ++)
            if(!is_empty(level))
                break;
        if(level == 3)
            current_time ++;
        else if(level < 2)
        {
            pt = dequeue(level);
            if(processes[pt].burst_time == processes[pt].remaining_time)
                processes[pt].start_time = current_time;
            int io_time;
            if(processes[pt].io_index < processes[pt].io_num)
                io_time = processes[pt].io_start_time[processes[pt].io_index] - processes[pt].burst_time + processes[pt].remaining_time;
            else
                io_time = processes[pt].remaining_time;
            process_time = time_quantum[level] < io_time ? time_quantum[level] : io_time;
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
                if(process_time == time_quantum[level])
                    processes[pt].level ++;
            }
            else
            {
                if(process_time == time_quantum[level])
                    processes[pt].level ++;
                enqueue(processes[pt].level, pt);
            }
            current_time += process_time;
            total_process_time -= process_time;
        }
        else
        {
            pt = dequeue(2);
            if(processes[pt].io_index < processes[pt].io_num)
            {
                process_time = processes[pt].io_start_time[processes[pt].io_index] - processes[pt].burst_time + processes[pt].remaining_time;
                for(int j = current_time; j <  current_time + process_time; j ++)
                {
                    gantt_log[gantt_len].time = j;
                    gantt_log[gantt_len].pid = processes[pt].pid;
                    gantt_len ++;
                }
                processes[pt].arrival_time = current_time + process_time + processes[pt].io_duration[processes[pt].io_index];
                processes[pt].remaining_time -= process_time;
                total_process_time -= process_time;
                processes[pt].io_index ++;
                current_time += process_time;
                inqueue[pt] = 0;
            }
            else
            {
                processes[pt].finish_time = current_time + processes[pt].remaining_time;
                processes[pt].waiting_time = processes[pt].finish_time - processes[pt].burst_time - processes[pt].original_arrival_time  - processes[pt].total_io_time;
                processes[pt].turnaround_time = processes[pt].finish_time - processes[pt].original_arrival_time;
                total_waiting += processes[pt].waiting_time;
                total_turnaround += processes[pt].turnaround_time;
                total_process_time -= processes[pt].remaining_time;
                for(int j = current_time; j <  current_time + processes[pt].remaining_time; j ++)
                {
                    gantt_log[gantt_len].time = j;
                    gantt_log[gantt_len].pid = processes[pt].pid;
                    gantt_len ++;
                }
                current_time += processes[pt].remaining_time;
                processes[pt].remaining_time = 0;
            }
        }
        if(level < 3)
        {
            int sz1 = size_queue(1), sz2 = size_queue(2);
            for(int i = 0; i < sz1; i ++)
            {
                int pr = dequeue(1);
                ready_time[pr] += process_time;
                if(ready_time[pr] > 9 && pr != pt)
                {
                    ready_time[pr] = 0;
                    processes[pr].level --;
                    enqueue(0, pr);
                }
                else
                    enqueue(1, pr);
            }
            for(int i = 0; i < sz2; i ++)
            {
                int pr = dequeue(2);
                ready_time[pr] += process_time;
                if(ready_time[pr] > 9 && pr != pt)
                {
                    ready_time[pr] = 0;
                    processes[pr].level --;
                    enqueue(1, pr);
                }
                else
                    enqueue(2, pr);
            }
            ready_time[pt] = 0;
        }
    }

    print_gantt(gantt_log, gantt_len);
    printf("\n[Results]\n");
    for(int i = 0; i < n; i++)
        printf("P%d: Waiting = %d, Turnaround = %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / n);
}
