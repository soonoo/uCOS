#include "includes.h"
#include <time.h>

#define TASK_STK_SIZE   512
#define N_TASKS         5
#define N_MSG           100

OS_STK TaskStk[N_TASKS][TASK_STK_SIZE];
OS_STK LogTaskStk[TASK_STK_SIZE];

OS_EVENT *msg_q;
void *msg_array[N_MSG];

void LogTask(void *data);
void Task(void *data);
void CreateTasks(void);

int main(void)
{
    OSInit();
    CreateTasks();
    msg_q = OSQCreate(msg_array, (INT16U)N_MSG);

    if(msg_q == 0) {
        return -1;
    }

    OSStart();
    return 0;
}

void CreateTasks(void)
{
    OSTaskCreate(LogTask, (void *)0, &LogTaskStk[TASK_STK_SIZE - 1], (INT8U)0);
    OSTaskCreate(Task, (void *)0, &TaskStk[0][TASK_STK_SIZE - 1], (INT8U)10);
    OSTaskCreate(Task, (void *)0, &TaskStk[1][TASK_STK_SIZE - 1], (INT8U)20);
}

void LogTask(void *pdata)
{
    FILE *log;
    void *msg;
    INT8U err;

    log = fopen("log.txt", "w");

    for(;;) {
        msg = OSQPend(msg_q, 0, &err);
        if(msg != 0) {
            fprintf(log, "%s", msg);
            fflush(log);
        }
    }
}

void Task(void *pdata)
{
    INT8U sleep, err;
    char msg[100];

    srand(time((unsigned int *)0) + (OSTCBCur->OSTCBPrio));

    for(;;) {
        sprintf(msg, "%4u: Task %u schedule \n", OSTimeGet(), OSTCBCur->OSTCBPrio);

        err - OSQPost(msg_q, msg);
        while(err != OS_NO_ERR) {
            err = OSQPost(msg_q, msg);
        }
        sleep = (rand() % 5) + 1;
        OSTimeDly(sleep);
    }
}