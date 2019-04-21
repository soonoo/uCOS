#include "includes.h"
#include <time.h>

#define TASK_STK_SIZE   512

// number of client/server task
#define N_CLIENT        20
#define N_SERVER        10

// number of messages in message queue
#define N_MSG           1000

OS_STK ServerTaskStk[N_CLIENT][TASK_STK_SIZE];
OS_STK ClientTaskStk[N_SERVER][TASK_STK_SIZE];

// requests from ClientTask
OS_EVENT *requestQueue;
void *requestMessages[N_MSG];

OS_EVENT *cpuCountSem;
int cpuCount = 0;

OS_EVENT *cpuTimeSem;

int requestCount = 0;

void InitQueue(void *pdata);
void CreateTasks(void *pdata);
void ServerTask(void *pdata);
void ClientTask(void *pdata);

int main(void)
{
    char buffer[100];
    OSInit();
    InitQueue(0);
    CreateTasks(0);

    cpuCountSem = OSSemCreate(N_SERVER);
    cpuTimeSem = OSSemCreate(N_SERVER);

    PC_DispClrScr(DISP_BGND_BLACK);
    PC_DispStr(2, 2, "Client Status", DISP_FGND_WHITE);
    PC_DispStr(29, 2, "System Info", DISP_FGND_WHITE);
    PC_DispStr(56, 2, "Server Status", DISP_FGND_WHITE);
    PC_DispStr(0, 3, "----------------------------------------------------------------------------", DISP_FGND_WHITE);

    sprintf(buffer, "Total ServerTask: %d", N_SERVER);
    PC_DispStr(29, 4, buffer, DISP_FGND_GREEN);

    sprintf(buffer, "Total ClientTask: %d", N_CLIENT);
    PC_DispStr(29, 5, buffer, DISP_FGND_GREEN);

    OSStart();
    return 0;
}

void InitQueue(void *pdata)
{
    requestQueue = OSQCreate(requestMessages, (INT16U)N_MSG);
}

void CreateTasks(void *pdata)
{
    int i = 0;

    // create ServerTask
    for(; i < N_SERVER; i++) {
        OSTaskCreate(ServerTask, (void *)i, &ServerTaskStk[i][TASK_STK_SIZE - 1], (INT8U)(i + 1));
    }

    // create ClientTask
    for(i = 0; i < N_CLIENT; i++) {
        OSTaskCreate(ClientTask, (void *)0, &ClientTaskStk[i][TASK_STK_SIZE - 1], (INT8U)(N_SERVER + i + 1));
    }
}

void ServerTask(void *pdata)
{
    void *msg;
    char *responseMessage = "success";
    INT8U err;
    int serverId = (int)pdata;
    char buffer[100];

    srand((unsigned)time(NULL) + OSTCBCur->OSTCBPrio);

    while(1) {
        msg = OSQPend(requestQueue, 0, &err);

        if(!msg) continue;

        OSSemPend(cpuCountSem, 0, &err);
        cpuCount++;
        OSSemPost(cpuCountSem);

        sprintf(buffer, "server#%d processing", serverId);
        PC_DispStr(56, 4 + serverId, buffer, DISP_FGND_RED);

        OSTimeDly(rand() % 30 + 15);
        sprintf(buffer, "server#%d idle      ", serverId);
        PC_DispStr(56, 4 + serverId, buffer, DISP_FGND_GREEN);

        PC_DispStr(29, 7, "Request processing", DISP_FGND_GREEN);
        sprintf(buffer, "#%s     ", (char *)msg);
        PC_DispStr(29, 8, buffer, DISP_FGND_GREEN);

        sprintf(buffer, "Server load: %3.2f %c", ((double)cpuCount/(double)N_SERVER) * 100.0, '%');
        PC_DispStr(29, 10, buffer, DISP_FGND_GREEN);

        // 20
        sprintf(buffer, "[                    ]");
        PC_DispStr(29, 11, buffer, DISP_FGND_GREEN);

        int dashCount = 20.0 * (double)cpuCount / (double)(N_SERVER);
        for(int j = 1; j < 21; j++) {
            char sep = dashCount >= j ? '|' : ' ';
            sprintf(buffer + j, "%c", sep);
        }
        PC_DispStr(29, 11, buffer, DISP_FGND_GREEN);

        OSSemPend(cpuCountSem, 0, &err);
        cpuCount--;
        OSSemPost(cpuCountSem);
    }
}

void ClientTask(void *pdata)
{
    INT8U err;
    char msg[200];
    int clientId = (OSTCBCur -> OSTCBPrio) - N_SERVER - 1;

    srand((unsigned)time(NULL) + clientId);

    while(1) {
        OSTimeDly(rand() % 10 + 5);

        sprintf(msg, "client#%02d request#%04d", clientId, requestCount++);
        PC_DispStr(2, 4 + clientId, msg, DISP_FGND_BLUE);
        OSTimeDly(20);
        sprintf(msg, "client#%02d             ", clientId);
        PC_DispStr(2, 4 + clientId, msg, DISP_FGND_WHITE);

        sprintf(msg, "%d", requestCount);
        err = OSQPost(requestQueue, msg);
        OSTimeDly(rand() % 70 + 40);
    }
}
