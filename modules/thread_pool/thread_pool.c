#include "thread_pool.h"

Thread_pool *pool = NULL;
static Thread_pool pool_1;

int pool_init(int max_thread_num)
{
    int i;
    pool = &pool_1;
    if(!pool)
        return -1;
    pthread_mutex_init(&(pool->queue_lock), NULL); //init lock
    pthread_cond_init(&(pool->queue_ready), NULL); //ready
    pool->queue_head = NULL;                        //clear work queue
    pool->max_thread_num = max_thread_num;          //number of active process
    pool->cur_queue_size = 0;                       //clear work queue
    pool->shutdown = 0;                             //thread close flag
    pool->thread_id = (pthread_t *)malloc(max_thread_num * sizeof(pthread_t));
    for(i = 0; i < max_thread_num; i++)
    {
        int ret = pthread_create(&(pool->thread_id[i]), NULL, thread_routine, NULL);
        if(ret != 0)
            return ret;
    }
    return 0;
}

int pool_add_worker (void *(*process) (void *arg), void *arg)
{
    if(process == NULL)
    	return -1;
    Thread_worker *new_worker = (Thread_worker*)malloc(sizeof(Thread_worker));
    new_worker->process = process;                   //add process for a thread
    new_worker->arg = arg;                           //arguments
    new_worker->next = NULL;                         //next work set NULL

    pthread_mutex_lock(&(pool->queue_lock));
    Thread_worker *member = pool->queue_head;
    /*add the new task at the end of the list*/
    if (member != NULL)
    {
        while (member->next != NULL)
            member = member->next;
        member->next = new_worker;
    }
    else
    {
        pool->queue_head = new_worker;
    }

    assert (pool->queue_head != NULL);              //assert

    pool->cur_queue_size++;
    pthread_mutex_unlock(&(pool->queue_lock));
    pthread_cond_signal(&(pool->queue_ready));    //wake up a thread who are blocked
    return 0;
}


/*销毁线程池，等待队列中的任务不会再被执行，但是正在运行的线程会一直
把任务运行完后再退出*/
int pool_destroy ()
{
    if (pool->shutdown)
        return -1;/*防止两次调用*/
    pool->shutdown = 1;
 
    /*唤醒所有等待线程，线程池要销毁了*/
    pthread_cond_broadcast (&(pool->queue_ready));
 
    /*阻塞等待线程退出，否则就成僵尸了*/
    int i;
    for (i = 0; i < pool->max_thread_num; i++)
        pthread_join (pool->thread_id[i], NULL);
    free (pool->thread_id);
 
    /*销毁等待队列*/
    Thread_worker *head = NULL;
    while (pool->queue_head != NULL)
    {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free (head);
    }
    /*条件变量和互斥量也别忘了销毁*/
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));
    
    free (pool);
    /*销毁后指针置空是个好习惯*/
    pool=NULL;
    return 0;
}


void *thread_routine(void *arg)
{
    //printf ("starting thread 0x%x\n", pthread_self ());
    while (1)
    {
        int ret = pthread_mutex_lock(&(pool->queue_lock));
        while(pool->cur_queue_size == 0 && !pool->shutdown)
        {
            //printf ("thread 0x%x is waiting\n", pthread_self ());
            pthread_cond_wait (&(pool->queue_ready), &(pool->queue_lock)); //cond_wait atom operation,
        }

        if(pool->shutdown)
        {
            pthread_mutex_unlock (&(pool->queue_lock));         // unlock first
            //LOG("thread 0x%x will exit\n", pthread_self ());
            pthread_exit(NULL);
        }
        //printf ("thread 0x%x is starting to work\n", pthread_self ());
        assert (pool->cur_queue_size != 0);
        assert (pool->queue_head != NULL);

        pool->cur_queue_size--;                 //work queue reduce 1 and delete list head member
        Thread_worker *worker = pool->queue_head;
        pool->queue_head = worker->next;
        pthread_mutex_unlock (&(pool->queue_lock));
        /*starting process*/
        (*(worker->process))(worker->arg);

        free(worker);
        worker = NULL;
    }
}
