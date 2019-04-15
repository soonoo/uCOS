#include "includes.h"
#include <time.h>

#define TASK_STK_SIZE   512

// number of client/server task
#define N_CLIENT        10
#define N_SERVER        5

// number of messages in message queue
#define N_MSG           100

OS_STK ServerTaskStk[N_CLIENT][TASK_STK_SIZE];
OS_STK ClientTaskStk[N_SERVER][TASK_STK_SIZE];
OS_STK PrinterTaskStk[TASK_STK_SIZE];

void CreateTasks(void *pdata);
void ServerTask(void *pdata);
void ClientTask(void *pdata);
void PrinterTask(void *pdata);

int main(void)
{
    OSInit();
    CreateTasks(0);
    OSStart();
    return 0;
}

void CreateTasks(void *pdata)
{
    int i = 0;

    // create server tasks
    for(; i < N_SERVER; i++) {
        OSTaskCreate(ServerTask, (void *)0, &ServerTaskStk[i][TASK_STK_SIZE - 1], (INT8U)(i + 1));
    }

    // create client tasks
    for(i = 0; i < N_CLIENT; i++) {
        OSTaskCreate(ClientTask, (void *)0, &ClientTaskStk[i][TASK_STK_SIZE - 1], (INT8U)(N_SERVER + i + 1));
    }

    // create printer task
    OSTaskCreate(PrinterTask, (void *)0, &PrinterTaskStk[TASK_STK_SIZE - 1], (INT8U)(N_CLIENT + N_SERVER + 1));
}

void ServerTask(void *pdata)
{

}

void ClientTask(void *pdata)
{

}

void PrinterTask(void *pdata)
{

}