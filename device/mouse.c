#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "mouse.h"
#include <fcntl.h>
#include <sys/poll.h>
#include <pthread.h>
#include <stdint.h>
#include "queue.h"
#define MSG     int
static struct mouse_config{
    int fd;
    pthread_t th;
    uint32_t  dbclicktimes;
    uint16_t posx,posy;
    msg_queue_t *msg;
}mouseConf;

int device_open();
int device_ctl(int cmd,void *p);
void *device_getmsg();
int msg_transale(void *context,MSG *Msg);
int device_close();

static void *device_listen(void *p)
{
    struct pollfd stPoll[1] = {0};
    u8  buf[4] = {0};
    stPoll[0].fd = mouseConf.fd;
    stPoll[0].events = POLLIN;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    while(mouseConf.fd > 0)
    {
        if(poll(stPoll,1,1) > 0)
        {
           read(stPoll[0].fd,buf,sizeof(buf)); 
           SendMsg(mouseConf.msg,buf);
        }
    }
}

int mouse_init()
{
    int ret = -1;
    int old_flags;
    u8  imps2_param [] = {243,200,243,100,243,80}; 
    u8  buf[4];
    old_flags = fcntl(mouseConf.fd,F_GETFL,0);
    printf("old_flags:%d\n",old_flags);
    ret = fcntl(mouseConf.fd,F_SETFL,O_NONBLOCK);
    if(write(mouseConf.fd,imps2_param,sizeof(imps2_param) == sizeof(imps2_param)))
    {
        //while(read(mouseConf.fd,buf,sizeof(buf)));
    }
    ret |= fcntl(mouseConf.fd,F_SETFL,old_flags);
    printf("ret:%d\n",ret);
    return ret;
}

int device_open()
{
    mouseConf.fd = open(DEV_MOUSE,O_RDWR);
    printf("mouse fd:%d\n",mouseConf.fd);
    if(mouseConf.fd >= 0 && mouse_init() >= 0 && (mouseConf.msg = CreateMsgQueue(100,4)))
    {
        pthread_create(&mouseConf.th,NULL,device_listen,NULL);
    }
    return mouseConf.fd;
}

void *device_getmsg()
{
    static u8 buf[4];
    if(RecvMsg(mouseConf.msg,buf,1) < 0)
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
    close(mouseConf.fd);
    mouseConf.fd = -1;
    DestoryMsgQueue(mouseConf.msg); 
    pthread_cancel(mouseConf.th);
    pthread_join(mouseConf.th,NULL);
    return 1;
}

