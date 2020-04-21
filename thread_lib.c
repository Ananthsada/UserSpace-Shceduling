#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

static struct_thread_lib_t thread_bhv;
static timer_t schedule_timer;

thread_error_t init_lib(struct_thread_lib_t* arg)
{
	if(!arg)
		return THREAD_LIB_NULL_PTR;

	thread_bhv.max_th = arg->max_th;
	thread_bhv.burst_time = arg->burst_time;
	thread_bhv.head = NULL;
	thread_bhv.no_th = 0;
	
	if(timer_create(CLOCK_MONOTONIC, SIGEV_SIGNAL, &schedule_timer) != 0)
		return THREAD_LIB_RESOURCE_ERR;
	
	return THREAD_LIB_OK;
}

unsigned char get_thread_cnt(void)
{
	return thread_bhv.no_th;
}

void scheduler()
{
	
}

thread_error_t add_thread_node(struct_thread_t* arg)
{
	struct_thread_t* start = head;
	
	if(!start)
	{
		struct itimerspec timer_info;

		start = arg;
		start->next = NULL;

		timer_info.it_value.tv_nsec = 20 * 1000 * 1000;
		timer_info.it_value.tv_sec = 0;
		timer_info.it_interval.tv_nsec = 20 * 1000 * 1000;
		timer_info.it_interval.tv_sec = 0;
		
		if(timer_settime(schedule_timer, 0, timer_info, NULL) != 0)
			return THREAD_LIB_RESOURCE_ERR;
	}
	else
	{
		while(start->next)
			start = start->next;
		start->next = arg;
		arg->next = NULL;
	}
	return THREAD_LIB_OK;
}

thread_error_t create_thread( void* (*func_arg)(void*), struct_thread_t arg, void* thread_arg)
{
	struct_thread_t* thread_node = NULL;
	
	if(!func_arg)
		return THREAD_LIB_NULL_PTR;
		
	thread_node = (struct_thread_t*)malloc(sizeof(struct_thread_t) );
	memset((unsigned char*)thread_node, 0, sizeof(struct_thread_t));
	
	if(arg->name)
	{
		unsigned char len = strlen(arg->name);
		len = (len > 16)?16:len;
		memcpy(&thread_node->name[0], &arg->name[0], len);
	}
	else
	{
		memcpy(&thread_node->name[0], "NewThread", 9);
	}
	thread_node->id = get_thread_cnt();
	thread_node->func = func_arg;
	thread_node->arg = thread_arg;
	thread_node->isrun = 0;
	
	return add_thread_node(thread_node);
	
}
