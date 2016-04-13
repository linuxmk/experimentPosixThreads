#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>

static int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void outputError(int useErr, int err, int flushStdout,const char *format, va_list ap)
{
#define BUF_SIZE 500
char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];
vsnprintf(userMsg, BUF_SIZE, format, ap);
if (useErr)
snprintf(errText, BUF_SIZE, " [%s %s]",(err > 0 && err <= 1024) ? "dfdfsd" : "?UNKNOWN?", strerror(err));
else
snprintf(errText, BUF_SIZE, ":");
snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);
if (flushStdout)
fflush(stdout);
fputs(buf, stderr);
fflush(stderr);
/* Flush any pending stdout */
/* In case stderr is not line-buffered */
}

void
errExitEN(int errnum, const char *format, ...)
{
    va_list argList;
    va_start(argList, format);
    outputError(1, errnum, 1, format, argList);
    va_end(argList);
   // terminate(1);
}

static void *threadFunc(void *arg)
{

    int loops = *((int*)arg);
    int loc, j,s;

    for(j = 0 ; j < loops ; j++)
    {
        s = pthread_mutex_lock(&mtx);
        if(s!=0)
           errExitEN(s, "pthread_mutex_lock");
        loc = glob;
        loc++;
        glob = loc;
        //glob++;
        s = pthread_mutex_unlock(&mtx);
        if(s!=0)
           errExitEN(s, "pthread_mutex_unlock");
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

    s = pthread_join(t1, NULL);

    s = pthread_join(t2, NULL);


    fprintf(stderr, "Glob = %d\n", glob);
    return 0;
}
