#include <stdio.h>
#include <stdlib.h>
#include "process.h"

static GanttSlot gantt_log[1000];
static int gantt_len;

static int ready_queue[100];
static int inqueue[100];
static int front = 0, rear = 0;

static void enqueue(int pid)
{
    ready_queue[rear++] = pid;
}

static int dequeue()
{
    return ready_queue[front++];
}

static int is_empty()
{
    return front == rear;
}


void run_rr(Process* processes, int n, int time_quantum)
{
    int current_time = 0;
    int total_waiting = 0, total_turnaround = 0, total_process_time = 0;
    for(int i = 0; i < n; i++)
        total_process_time += processes[i].burst_time;

    while(total_process_time > 0)
    {
        int pt = -1;
        while(pt == -1)
        {
            for(int i = 0; i < n; i++)
                if(processes[i].arrival_time <= current_time && inqueue[i] == 0)
                {
                    inqueue[i] = 1;
                    enqueue(i);
                }
            if(!is_empty())
                pt = dequeue();
            else
                current_time ++;
        }
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
            gantt_log[gantt_len - 1].io = 1;
        }
        else
            enqueue(pt);
        current_time += process_time;
        total_process_time -= process_time;
    }

    print_gantt(gantt_log, gantt_len);
    printf("\n[Results]\n");
    for(int i = 0; i < n; i++)
        printf("P%d: Waiting = %d, Turnaround = %d\n", processes[i].pid, processes[i].waiting_time, processes[i].turnaround_time);
    printf("\nAverage Waiting Time: %.2f\n", (float)total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround / n);
}
