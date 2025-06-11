#ifndef PROCESS_H
#define PROCESS_H

typedef struct
{
    int pid;
    int original_arrival_time;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;
    int deadline;

    int io_start_time[4];
    int io_duration[4];
    int io_index;
    int io_num;
    int state;
    int total_io_time;

    int start_time;
    int finish_time;
    int waiting_time;
    int turnaround_time;

    int level;
} Process;

//gantt log
typedef struct
{
    int time;
    int pid;
    int io;
} GanttSlot;


void print_gantt(GanttSlot* log, int len);
#endif
