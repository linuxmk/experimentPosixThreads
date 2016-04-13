#include <pthread.h>
#include <stdio.h>

static int glob = 0;

static void *threadFunc(void *arg)
{

    int loops = *((int*)arg);
    int loc, j;

    for(j = 0 ; j < loops ; j++)
    {
      //  loc = glob;
       // loc++;
        //glob = loc;
        glob++;
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    pthread_t t1,t2;
    int loops, s;


    loops = (argc > 1) ? atoi(argv[1]) : 1000;

    s =  pthread_create(&t1, NULL, threadFunc, &loops);

    s = pthread_create(&t2, NULL, threadFunc, &loops);

    s = pthread_join(&t1, NULL);

    s = pthread_join(&t2, NULL);

    fprintf(stderr, "Glob = %d\n", glob);
    return 0;
}
