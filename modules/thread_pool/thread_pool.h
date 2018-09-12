#pragma once
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct worker
{
    void *(*process) (void *arg);   //thread process
    void *arg;                      // thread process arguments
    struct worker *next;
}Thread_worker;

typedef struct
{
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;

    Thread_worker *queue_head;      //all thread saved in a linear list
    int shutdown;                   // destroy thread pool or not flag
    pthread_t *thread_id;
    int max_thread_num;             // the number of max process that have access to work
    int cur_queue_size;             // current queue number
}Thread_pool;

int pool_init(int max_thread_num);

int pool_add_worker(void *(*process)(void *arg), void *arg);

int pool_destroy();

void *thread_routine (void *arg);

#ifdef __cplusplus
}
#endif
