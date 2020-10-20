#include <stdio.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/resource.h>

/*
 * Scheduling policies defined in sched.h
#define SCHED_NORMAL	0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_BATCH		3
 */

struct sched_param {
	int sched_priority;
};

struct sched_param s_param;

void setscheduler(void)
{
	int r;
	/* call sched_getparam() to initialize s_param struct */
	sched_getparam(0, &s_param);
	printf("Got priority %d\n" % s_param.sched_priority);
	/* set sched priority to 1 and schedule policy to FIFO */
	s_param.sched_priority = 1;
	r = sched_setscheduler(0, SCHED_FIFO, &s_param);
	printf("Scheduler set to SCHED_FIFO with priority %i...\n", s_param.sched_priority);
	
	/* check for errors */
	if (r) {
		printf("!!!Scheduler set to SCHED_FIFO with priority %i FAILED!!!\n", s_param.sched_priority);
	}
}

int main()
{
	setscheduler();

	while(1)
	{
		printf("PID %i sleeping..\n", getpid());
		switch(sched_getscheduler(getpid()))
		{
			case SCHED_NORMAL: printf("sched_getscheduler() = SCHED_NORMAL\n");
								break;
			case SCHED_FIFO: printf("sched_getscheduler() = SCHED_FIFO\n");
								break;
			case SCHED_RR: printf("sched_getscheduler() = SCHED_RR\n");
								break;
			case SCHED_BATCH: printf("sched_getscheduler() = SCHED_BATCH\n");
								break;
			default:
				break;
		}

		printf("Process priority %i...\n", getpriority(PRIO_PROCESS, 0));
		sleep(5);
	}
}
