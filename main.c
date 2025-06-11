#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include <time.h>

void run_fcfs(Process* p, int n);
void run_sjf_np(Process* p, int n);
void run_sjf_p(Process* p, int n);
void run_ljf_np(Process* p, int n);
void run_ljf_p(Process* p, int n);
void run_priority_np(Process* p, int n);
void run_priority_p(Process* p, int n);
void run_rr(Process* p, int n, int tq);
void run_mlq(Process* p, int n, int tq);
void run_mlfq(Process* p, int n);
void run_priority_rr(Process* p, int n, int tq);
void run_priority_update(Process* p, int n, int alpha, int beta);
void run_priority_io_np(Process* p, int n);
void run_sif_np(Process* p, int n);
void run_rm(Process* p, int n);
void run_edf(Process* p, int n);
void run_double_fcfs(Process* p, int n);


int random_range(int a, int b)
{
    return rand() % (b - a + 1) + a;
}

int main()
{
    int n, g;
    printf("Number of Processes(Recommended : more than 2): ");
    scanf("%d", &n);
    Process* orig = malloc(sizeof(Process) * n);
    srand(time(NULL));
    printf("Random generation: 1\nManualy Generate :2\n");
    scanf("%d", &g);
    if(g == 1)
        for(int i = 0; i < n; i++)
        {
            orig[i].pid = i + 1;
            orig[i].original_arrival_time = orig[i].arrival_time = random_range(0, 10);
            orig[i].burst_time = orig[i].remaining_time = random_range(5, 20);
            orig[i].priority = random_range(0, 10);
            orig[i].deadline = orig[i].arrival_time + orig[i].burst_time + random_range(10, 20);
            int io_count = random_range(0, 4);
            int io_point = random_range(1, 4);
            orig[i].io_num = 0;
            orig[i].io_index = 0;
            orig[i].total_io_time = 0;
            for(int j = 0; j < io_count; j++)
            {
                if (io_point >= orig[i].burst_time - 1)
                    break;
                orig[i].io_start_time[j] = io_point;
                orig[i].io_duration[j]   = random_range(1, 5);
                orig[i].total_io_time += orig[i].io_duration[j];
                io_point += random_range(1, 4);
                orig[i].io_num++;
            }
            orig[i].state = 0;
            orig[i].level = 0;
            printf("[P%d] Arrival: %d, Burst: %d, Priority: %d, Deadline: %d\n", orig[i].pid, orig[i].arrival_time, orig[i].burst_time, orig[i].priority, orig[i].deadline);
            for(int j = 0; j < orig[i].io_num; j++)
                printf("  IO #%d: Start at %d, Duration %d\n", j+1, orig[i].io_start_time[j], orig[i].io_duration[j]);
        }
    else
        for(int i = 0; i < n; i++)
        {
            printf("[P%d]Generate. Arrival time, Burst time, Priority.\n", i + 1);
            scanf("%d %d %d", orig[i].original_arrival_time, orig[i].burst_time, orig[i].priority);
            orig[i].pid = i + 1;
            orig[i].arrival_time = orig[i].original_arrival_time;
            orig[i].remaining_time = orig[i].burst_time;
            orig[i].deadline = orig[i].arrival_time + orig[i].burst_time + random_range(10, 20);
            int io_count = random_range(0, 4);
            int io_point = 1;
            orig[i].io_num = 0;
            orig[i].io_index = 0;
            orig[i].total_io_time = 0;
            for(int j = 0; j < io_count; j++)
            {
                if (io_point >= orig[i].burst_time - 1)
                    break;
                orig[i].io_start_time[j] = io_point;
                orig[i].io_duration[j]   = random_range(1, 5);
                orig[i].total_io_time += orig[i].io_duration[j];
                io_point += random_range(1, 4);
                orig[i].io_num++;
            }
            orig[i].state = 0;
            orig[i].level = 0;
            printf("[P%d] Arrival: %d, Burst: %d, Priority: %d, Deadline: %d\n", orig[i].pid, orig[i].arrival_time, orig[i].burst_time, orig[i].priority, orig[i].deadline);
            for(int j = 0; j < orig[i].io_num; j++)
                printf("  IO #%d: Start at %d, Duration %d\n", j+1, orig[i].io_start_time[j], orig[i].io_duration[j]);
        }

    const char* name[15] = {"FCFS", "SJF_np", "SJF_p", "LJF_np", "LJF_p","Priority_np", "Priority_p", "RR", "MLQ", "MLFQ", "Priority_RR", "Priority_Update", "Priority_io_num", "Shortest IO First", "Double_FCFS"};
    for(int alg = 0; alg < 15; alg++)
    {
        Process* proc = malloc(sizeof(Process) * n);
        memcpy(proc, orig, sizeof(Process) * n);
        printf("\n=== Algorithm: %s ===\n", name[alg]);
        printf(">> START Algorithm %s (n=%d)\n", name[alg], n);
        switch(alg)
        {
            case 0: run_fcfs(proc, n); break;
            case 1: run_sjf_np(proc, n); break;
            case 2: run_sjf_p(proc, n); break;
            case 3: run_ljf_np(proc, n); break;
            case 4: run_ljf_p(proc, n); break;
            case 5: run_priority_np(proc, n); break;
            case 6: run_priority_p(proc, n); break;
            case 7: run_rr(proc, n, 4); break;
            case 8: run_mlq(proc, n, 4); break;
            case 9: run_mlfq(proc, n); break;
            case 10: run_priority_rr(proc, n, 4); break;
            case 11: run_priority_update(proc, n, 2, 1); break;
            case 12: run_priority_io_np(proc, n); break;
            case 13: run_sif_np(proc, n); break;
            case 14: run_double_fcfs(proc, n); break;
        }
        printf("<< END   Algorithm %s\n", name[alg]);
        free(proc);
    }

    int deadline1 = orig[0].deadline, deadline2 = orig[1].deadline;
    Process* subset = malloc(sizeof(Process) * 6);
    Process* subset2 = malloc(sizeof(Process) * 6);
    memcpy(&subset[0], &orig[0], sizeof(Process));
    memcpy(&subset[1], &orig[1], sizeof(Process));
    orig[0].pid += 2;
    orig[1].pid += 2;
    orig[0].original_arrival_time += deadline1;
    orig[0].arrival_time = orig[0].original_arrival_time;
    orig[1].original_arrival_time += deadline2;
    orig[1].arrival_time = orig[1].original_arrival_time;
    orig[0].deadline += deadline1;
    orig[1].deadline += deadline2;
    memcpy(&subset[2], &orig[0], sizeof(Process));
    memcpy(&subset[3], &orig[1], sizeof(Process));
    orig[0].pid += 2;
    orig[1].pid += 2;
    orig[0].original_arrival_time += deadline1;
    orig[0].arrival_time = orig[0].original_arrival_time;
    orig[1].original_arrival_time += deadline2;
    orig[1].arrival_time = orig[1].original_arrival_time;
    orig[0].deadline += deadline1;
    orig[1].deadline += deadline2;
    memcpy(&subset[4], &orig[0], sizeof(Process));
    memcpy(&subset[5], &orig[1], sizeof(Process));
    memcpy(subset2, subset, sizeof(Process)*6);
    printf("\n=== Real-Time Scheduling ===\n");
    printf("\n-- Rate Monotonic (RM) --\n");
    run_rm(subset, 6);
    printf("\n-- Earliest Deadline First (EDF) --\n");
    run_edf(subset2, 6);
    free(orig);
    return 0;
}
