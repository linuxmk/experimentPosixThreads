#include <stdio.h>
#include <pthread.h>
#include <time.h>

typedef struct alarm_tag {
    struct alarm_tag *link;
    int                             seconds;
    time_t                      time;
    char                          message[64];
}alarm_t;

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
alarm_t *alarm_list = NULL;

#define DEBUG

void * alarm_thread(void *arg)
{
            alarm_t *alarm;
            int sleep_time;
            time_t now;
            int status;

            while(1)
            {
                    status = pthread_mutex_lock(&alarm_mutex);
                    if(status != 0)
                        return -1;

                     alarm = alarm_list;
                     if(!alarm)
                         sleep_time = 1;
                     else
                     {
                         alarm_list = alarm_list->link;
                         now = time(NULL);
                         if(alarm->time <= now)
                         {
                             sleep_time = 0;
                         }
                         else
                             sleep_time = alarm->time - now;

                         fprintf(stderr, "[waiting: %d(%d)\"%s\"]\m", alarm->time, sleep_time, alarm->message);
                    }
                    status = pthread_mutex_unlock(&alarm_mutex);
                    if(status != 0)
                        return -1;

                    if(sleep_time > 0)
                        sleep(sleep_time);
                    else
                            sched_yield();

                    if(alarm)
                    {
                        fprintf(stderr, "(%d) %s\n", alarm->seconds, alarm->message);
                        free(alarm);
                    }
            }
}

int main(int argc, char *argv[])
{

        int status;
        char line[128];
        alarm_t *alarm, **last, *next;
        pthread_t thread;

        status = pthread_create(&thread,NULL, alarm_thread, NULL);
        if(status != 0)
        {
            return 0;
        }

        while(1)
        {
            printf("alarm>");
            if(fgets(line, sizeof(line), stdin) == NULL)
                    exit(0);
            if(strlen(line) <= 1)
                continue;

            alarm = (alarm_t *)malloc(sizeof(alarm_t));
            if(!alarm)
                exit(1);

            if(sscanf(line, "%d %64[^\n]",&alarm->seconds, alarm->message) < 2)
            {
                fprintf(stderr, "Error command\n");
                free(alarm);
            }
            else
            {
                status  = pthread_mutex_lock(&alarm_mutex);
                if(status != 0)
                    return -1;
                alarm->time = time(NULL) + alarm->seconds;

                last = &alarm_list;
                next = *last;
                while(next != NULL)
                {
                    if(next->time >= alarm->time)
                    {
                        alarm->link = next;
                        *last = alarm;
                        break;
                    }
                    last = &next->link;
                    next = next->link;
                }

                if(next == NULL)
                {
                    *last = alarm;
                    alarm->link = NULL;
                }

#ifdef DEBUG
                    fprintf(stderr, "[list: ");
                    for(next = alarm_list ; next != NULL ; next = next->link)
                        fprintf(stderr, "%d(%d)[\"%s\"]\n", next->time, next->time - time(NULL), next->message);
                    fprintf(stderr, "]\n");

#endif


                status = pthread_mutex_unlock(&alarm_mutex);
                if(status != 0)
                    return -1;

            }
        }


return 0;
}

