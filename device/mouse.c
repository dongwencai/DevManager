#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "mouse.h"
#include <fcntl.h>
#include <sys/poll.h>
#include <pthread.h>
#include "queue.h"
#define MSG     int


int device_open();
int device_ctl(int cmd,void *p);
void *device_getmsg();
int msg_transale(void *context,MSG *Msg);
int device_close();

static int gMousefd = -1;
static void pthread_create_detached(void *(*thread_func)(void *),void *param);
static msg_queue_t *gPmsgHead = NULL;

static void *device_listen(void *p)
{
    struct pollfd stPoll[1] = {0};
    u8  buf[4] = {0};
    stPoll[0].fd = gMousefd;
    stPoll[0].events = POLLIN;
    while(1)
    {
        if(poll(stPoll,1,1) > 0)
        {
           read(stPoll[0].fd,buf,sizeof(buf)); 
           SendMsg(gPmsgHead,buf);
        }
    }
}
static void pthread_create_detached(void *(*thread_func)(void *),void *param)
{
    pthread_t tid;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
    pthread_create(&tid,&tattr,thread_func,param);
    pthread_attr_destroy(&tattr);
}

int mouse_init()
{
    int ret = -1;
    int old_flags;
    u8  imps2_param [] = {243,200,243,100,243,80}; 
    u8  buf[4];
    old_flags = fcntl(gMousefd,F_GETFL,0);
    printf("old_flags:%d\n",old_flags);
    ret = fcntl(gMousefd,F_SETFL,O_NONBLOCK);
    if(write(gMousefd,imps2_param,sizeof(imps2_param) == sizeof(imps2_param)))
    {
        //while(read(gMousefd,buf,sizeof(buf)));
    }
    ret |= fcntl(gMousefd,F_SETFL,old_flags);
    printf("ret:%d\n",ret);
    return ret;
}

int device_open()
{
    gMousefd = open(DEV_MOUSE,O_RDWR);
    printf("mouse fd:%d\n",gMousefd);
    if(gMousefd >= 0 && mouse_init() >= 0 && (gPmsgHead = CreateMsgQueue(100,4)))
    {
        pthread_create_detached(device_listen,gPmsgHead);
    }
    return gMousefd;
}

void *device_getmsg()
{
    static u8 buf[4];
    if(RecvMsg(gPmsgHead,buf,1) < 0)
        return NULL;
    return buf;
}

int msg_transale(void *context,MSG *Msg)
{
    u8 *buf = context;
    printf("buf:%d\t%d\t%d\t%d\n",buf[0],buf[1],buf[2],buf[3]);
    return 1;
}

int device_close()
{
    close(gMousefd);
    gMousefd = -1;
    return 1;
}

