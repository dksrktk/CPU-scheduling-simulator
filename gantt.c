#include <stdio.h>
#include "process.h"

void print_gantt(GanttSlot* log, int len)
{
    printf("\n[Gantt Chart]\n");
    int segPid[2000], segTime[2000];
    int segCnt = 0;
    int lastSegPid = -1, prevTime = -1;
    for(int i = 0; i < len; i++)
    {
        int t  = log[i].time;
        int pid = log[i].pid;
        if(i == 0)
        {
            if(t > 0)
            {
                segPid[segCnt] = 0;
                segTime[segCnt] = 0;
                lastSegPid = 0;
                segCnt++;
            }
            segPid[segCnt] = pid;
            segTime[segCnt] = t;
            lastSegPid = pid;
            segCnt++;
        }
        else
        {
            if(t > prevTime + 1 && lastSegPid != 0)
            {
                segPid[segCnt] = 0;
                segTime[segCnt] = prevTime + 1;
                lastSegPid = 0;
                segCnt++;
            }
            if (pid != lastSegPid)
            {
                segPid[segCnt] = pid;
                segTime[segCnt] = t;
                lastSegPid = pid;
                segCnt++;
            }
        }
        prevTime = t;
    }
    segTime[segCnt] = prevTime + 1;

    for(int i = 0; i < segCnt; i++)
    {
        if(segPid[i] == 0)
            printf("|%5s", "Idle");
        else
            printf("|  P%2d", segPid[i]);
    }
    printf("|\n");

    for(int i = 0; i <= segCnt; i++)
    {
        if(i == 0)
            printf("%d", segTime[i]);
        else
            printf("%6d", segTime[i]);
    }
    printf("\n\n");
}
