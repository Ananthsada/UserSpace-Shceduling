#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "thread_lib.h"

void* func(void* arg)
{
	printf("Inside thread\n");
}

int main()
{
	struct_thread_lib_t thread_lib;
	thread_error_t Error;
	struct_thread_t thread_info = {"Mythread", 0, 0, 0, NULL, NULL};
	
	thread_lib.max_th = 2;
	thread_lib.burst_time = 20;
	
	Error = init_lib(&thread_lib);
	if( Error != THREAD_LIB_OK )
	{
		exit(1);
	}
	
	for( int i = 0; i < 5 ; i++ )
	{
		Error = create_thread(func, thread_info, NULL);
		if(Error != THREAD_LIB_OK )
		{
			printf("Not able to create thread %d\n",Error);
			exit(1);
		}
	}
	
	while( 1 )
	{
		usleep( 1000 * 1000 );
	}
}
