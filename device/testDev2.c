#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MSG     int


int device_open();
int device_ctl(int cmd,void *p);
void *device_getmsg();
int msg_transale(void *context,MSG *Msg);
int device_close();



int device_open()
{
    printf("testDev2 open\n");
    return 0;
}

void *device_getmsg()
{
    int rd;
    static char *str = "abcdefg";
    rd = rand()%7;
    sleep(1);
    printf("%c\n",str[rd]);
    return &str[rd];
}

int msg_transale(void *context,MSG *Msg)
{
    char randsum = *(char *)context;
    *Msg = randsum;
    return 1;
}

int device_close()
{
    printf("testDev2 close\n");
    return 1;
}

