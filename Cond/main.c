#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

typedef struct my_struct_tag {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int             value;
}my_struct_t;


my_struct_t data = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0 };
int hibernation = 1;

void wait_thread(void *arg)
{

    int status;

    sleep(hibernation);

    status = pthread_mutex_lock(&data.mutex);
    if(status!=0)
    {
        fprintf(stderr, "Create wait thread");
    }

    data.value = 1;

    status = pthread_cond_signal(&data.cond);
    if(status!=0)
    {
        fprintf(stderr, "Create wait thread");
    }

    status = pthread_mutex_unlock(&data.mutex);if(status!=0)
    {
        fprintf(stderr, "Create wait thread");
    }

return NULL;


}


int main(int argc, char *argv[])
{
    int status;
    pthread_t wait_thread_id;
    struct timespec timeout;

    if(argc > 1)
        hibernation = atoi(argv[1]);

    status = pthread_create(&wait_thread_id, NULL, wait_thread, NULL);
    if(status!=0)
    {
        fprintf(stderr, "Create wait thread");
    }


    timeout.tv_sec = time(NULL)+3;
    timeout.tv_nsec = 0;

    status = pthread_mutex_lock(&data.mutex);
    if(status!=0)
    {
        fprintf(stderr, "mutex lock");
    }

    while(data.value == 0)
    {
        status = pthread_cond_timedwait(&data.cond, &data.mutex, &timeout);
        if(status == ETIMEDOUT)
        {
            timeout.tv_sec = time(NULL)+3;// renew timedwait
            fprintf(stderr, "condition wait timed out\n");
           // break;
        }
        else
            if(status!=0)
            {
                fprintf(stderr, "error:pthread_cond_timedwait");
                abort();
            }

    }
    if(data.value != 0)
        fprintf(stderr, "Condition was signaled\n");

    status = pthread_mutex_unlock(&data.mutex);
    if(status!=0)
    {
        fprintf(stderr, "mutex unlock");
    }



    return 0;
}
