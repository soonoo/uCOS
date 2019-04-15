#include "includes.h"
#include <time.h>

#define TASK_STK_SIZE 512
#define N_TASKS	5

OS_STK TaskStk[N_TASKS][TASK_STK_SIZE];
OS_EVENT *sem;

void Task(void *data);
void CreateTasks(void);

int main(void)
{
	OSInit();

	srand(time(NULL));
	sem = OSSemCreate(1);

	CreateTasks();

	OSStart();
	return 0;
}

void CreateTasks(void)
{
	INT8U i;
	for(i = 0; i < N_TASKS; i++) {
		OSTaskCreate(Task, (void *)0, &TaskStk[i][TASK_STK_SIZE - 1], (INT8U)(i + 1));
	}
}

void Task(void *data)
{
	FILE *out;
	INT8U sleep;
	INT8U err;

	for(;;) {
		OSSemPend(sem, 0, &err);
		out = fopen("result.txt", "a");

		fprintf(out, "%4u: Task %u, file open \n", OSTimeGet(), OSTCBCur->OSTCBPrio);

		fflush(out);
		sleep = (rand() % 3) + 1;
		OSTimeDly(sleep);
		fprintf(out, "%4u: Task %u, file close \n", OSTimeGet(), OSTCBCur->OSTCBPrio);

		OSSemPost(sem);
		fclose(out);
		sleep = (rand() % 3) + 1;
		OSTimeDly(sleep);
	}
}