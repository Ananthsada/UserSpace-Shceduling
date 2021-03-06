#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include "../inc/thread_lib.h"

#define MAX_THREADS 255
#define DEFAULT_BURST_TIME 20 /* in milliseconds */

extern void scheduler(int sig, siginfo_t *info, void *ucontext);

static struct_thread_lib_t thread_bhv;
static timer_t schedule_timer;

thread_error_t init_lib()
{
	struct sigevent sev;
	sigset_t mask;
	struct sigaction sa;
	
	if(thread_bhv.hymen_broken)
		return THREAD_LIB_ALREADY_INITIALIZED;
		
	thread_bhv.max_th = MAX_THREADS;
	thread_bhv.burst_time = DEFAULT_BURST_TIME;
	thread_bhv.head = NULL;
	thread_bhv.no_th = 0;
	thread_bhv.hymen_broken = 1;
	
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = scheduler;
	sigemptyset(&sa.sa_mask);
	if(sigaction(SIGRTMIN, &sa, NULL) == -1)
	{
		printf("Sigaction failed %s\n",strerror(errno));
		return THREAD_LIB_SYS_ERR;
	}
/*	
	sigemptyset(&mask);
	sigaddset(&mask, SIGRTMIN);
	if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	{
		printf("Mask Signal failed %s\n",strerror(errno));
		return THREAD_LIB_SYS_ERR;
	}
*/	
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = &schedule_timer;
	
	if(timer_create(CLOCK_MONOTONIC, &sev, &schedule_timer) != 0)
		return THREAD_LIB_RESOURCE_ERR;
	
	return THREAD_LIB_OK;
}

unsigned char get_thread_cnt(void)
{
	return thread_bhv.no_th;
}

void scheduler(int sig, siginfo_t *info, void *ucontext)
{
	thread_data_struct_t* start = thread_bhv.head;
		
	printf("Inside Scheduler\n");
	
	do
	{
		while(start)
		{
			if( start->thread_node->isrun == 1 )
			{
				printf("Scheduling thread %d\n",start->thread_node->id);
				start->thread_node->func(start->thread_node->arg);
				start->thread_node->isrun = 0;
			}
			start = start->next_node;
			if(start)
			{
				start->thread_node->isrun = 1;
			}
			printf("Moved to another thread \n");
		}
		start = thread_bhv.head;
		start->thread_node->isrun = 1;
	}while(1);
	
	printf("Exiting Scheduler\n");
}

thread_error_t add_thread_node(struct_thread_t* arg)
{
	thread_data_struct_t* start = thread_bhv.head;
	thread_data_struct_t* node_l;
	node_l = malloc(sizeof(thread_data_struct_t));
	node_l->thread_node = arg;
	node_l->next_node = NULL;
	
	printf("Adding New thread %s\n", arg->name);
	if(!start)
	{
		struct itimerspec timer_info;

		start = node_l;
		start->thread_node->isrun = 1;

		timer_info.it_value.tv_nsec = 0;
		timer_info.it_value.tv_sec = 2;
		timer_info.it_interval.tv_nsec = 0 * 1000 * 1000;
		timer_info.it_interval.tv_sec = 0;
		
		if(timer_settime(schedule_timer, 0, &timer_info, NULL) != 0)
			return THREAD_LIB_RESOURCE_ERR;
		printf("Scheduler Thread Created\n");
		
		thread_bhv.head = start;
	}
	else
	{
		while(start->next_node)
			start = start->next_node;
		start->next_node = node_l;
	}
	thread_bhv.no_th += 1;
	return THREAD_LIB_OK;
}

thread_error_t create_thread( void* (*func_arg)(void*), struct_thread_t arg, void* thread_arg)
{
	struct_thread_t* thread_node = NULL;
	
	if(!func_arg)
		return THREAD_LIB_NULL_PTR;
		
	thread_node = (struct_thread_t*)malloc(sizeof(struct_thread_t) );
	memset((unsigned char*)thread_node, 0, sizeof(struct_thread_t));
	
	if(arg.name)
	{
		unsigned char len = strlen(arg.name);
		len = (len > 16)?16:len;
		memcpy(&thread_node->name[0], &arg.name[0], len);
	}
	else
	{
		memcpy(&thread_node->name[0], "NewThread", 9);
	}
	thread_node->id = get_thread_cnt() + 1;
	thread_node->func = func_arg;
	thread_node->arg = thread_arg;
	thread_node->isrun = 0;
	
	return add_thread_node(thread_node);
	
}
