#ifndef THREAD_LIBRARY_H
#define THREAD_LIBRARY_H

typedef enum
{
	THREAD_LIB_OK = 0,
	THREAD_LIB_NULL_PTR = -255,
	THREAD_LIB_RESOURCE_ERR,
	THREAD_LIB_INVALID = 255,
}thread_error_t;

typedef struct
{
	unsigned char name[16];
	unsigned char zero; /* just to be safe */
	unsigned char id;
	unsigned char isrun;
	void* arg;
	void* (*func)(void*);
}struct_thread_t;


struct struct_thread_data
{
	struct_thread_t* thread_node;
	struct struct_thread_data* next_node;
};
typedef struct struct_thread_data thread_data_struct_t;

typedef struct
{
	thread_data_struct_t* head;
	unsigned int short burst_time;
	unsigned char max_th;
	unsigned char no_th;
	unsigned char curr_th;
}struct_thread_lib_t;

extern thread_error_t init_lib(struct_thread_lib_t* arg);
extern thread_error_t create_thread( void* (*func)(void*), struct_thread_t arg, void* thread_arg);
extern unsigned char get_thread_cnt(void);

#endif
