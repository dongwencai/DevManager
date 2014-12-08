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
#include "event.h"
#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif
typedef struct{
    int msg_id;
    int event;
    uint32_t param1;
    uint32_t param2;
}dev_msg_t;

static struct mouse_config{
    int fd;
    pthread_t th;
    uint32_t  dbclicktimes;
    msg_queue_t *msg;
}mouseConf;

int device_open();
int device_ctl(int cmd,void *p);
void *device_getmsg();
int msg_transale(void *context,msg_t *Msg);
int device_close();

void mouse_clean(int fd)
{
   struct pollfd stPoll[1] = {fd,POLLIN};
   u8   buf[4];
   while(poll(stPoll,1,1) > 0)
   {
       read(stPoll[0].fd,buf,sizeof(buf));
   }
}
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
    int ret = 1;
    u8  imps2_param [] = {243,200,243,100,243,80}; 
    u8  buf[4];
    if(write(mouseConf.fd,imps2_param,sizeof(imps2_param) == sizeof(imps2_param)))
    {
        mouse_clean(mouseConf.fd);
    }
    else
    {
        ret = -1;
    }
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

int msg_transale(void *context,msg_t *pMsg)
{
    u8 *buf = context;
    static int lbtn_down = FALSE,rbtn_down = FALSE,mbtn_down = FALSE;
    //printf("buf:%d\t%d\t%d\t%d\n",buf[0],buf[1],buf[2],buf[3]);
    if(buf[0] & MOUSE_LBTN)
    {
        lbtn_down = TRUE;
        pMsg->events = MOUSE_LBTN_DOWN;
    }
    else
    {
        if(lbtn_down)
        {
            lbtn_down = FALSE;
            pMsg->events = MOUSE_LBTN_UP;
        }
    }
    if(buf[0] & MOUSE_RBTN)
    {
        rbtn_down = TRUE; 
        pMsg->events = MOUSE_RBTN_DOWN;
    }
    else
    {
        if(rbtn_down)
        {
            rbtn_down = FALSE;
            pMsg->events = MOUSE_RBTN_UP;
        }
    }
    if(buf[0] & MOUSE_MBTN)
    {
        mbtn_down = TRUE;
        pMsg->events = MOUSE_MBTN_DOWN;
    }
    else
    {
        if(mbtn_down)
        {
            mbtn_down = FALSE;
            pMsg->events = MOUSE_MBTN_UP;
        }
    }
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

