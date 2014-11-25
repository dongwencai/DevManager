#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define MSG     int


int device_open();
int device_ctl(int cmd,void *p);
void *device_listen();
int msg_transale(void *context,MSG *Msg);
int device_close();



int device_open()
{
    printf("%s\t%x\n",__func__,(unsigned int)device_open);
    return 0;
}

void *device_listen()
{
    static int rd ;
    rd = rand();
    sleep(1);
    printf("%s\trd:%d\n",__func__,rd);
    return &rd;
}

int msg_transale(void *context,MSG *Msg)
{
    int randsum = *(int *)context;
    *Msg = randsum + 1;
    return 1;
}

int device_close()
{
    printf("%s\t%x\n",__func__,(unsigned int)device_close);
    return 1;
}

