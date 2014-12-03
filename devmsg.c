#include "config.h"
#include "devmsg.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#define TIMESTAMP(s,us)     ((s & 0xffff) | ((us & 0x7fff) << 16))
static int g_devMsgid = -1;
static int g_retMsgid = -1;
typedef unsigned int device_t ;
static int msgid_init()
{
    key_t key;
    key = ftok(DEVICEMANAGER,IPCKEY);
    g_devMsgid = msgget(key,0666 | IPC_CREAT);
    key = ftok(DEVICERETMSG,IPCKEY);
    g_retMsgid = msgget(key,0666 | IPC_CREAT);
    return (g_devMsgid | g_retMsgid);
}
int register_dev(char *so_name)
{
    PDEVMSG pMsg = NULL;
    long ts;
    struct timeval tv;
    if(g_devMsgid < 0 || g_retMsgid < 0)
    {
        if(msgid_init() < 0)
            return -1;
    }
    if(sizeof(so_name) > (512 - sizeof(DEVMSG)))
    {
        printf("param len is too long\n");
        return -1;
    }
    pMsg =(PDEVMSG) malloc(512);
    gettimeofday(&tv,NULL);
    ts = TIMESTAMP(tv.tv_sec,tv.tv_usec);
    pMsg->timing = ts;
    pMsg->ret = 1;
    pMsg->type = DEVMSGTYPE;
    pMsg->cmd = REG_DEV;
    strcpy(pMsg->param,so_name);
    msgsnd(g_devMsgid,pMsg,512,IPC_NOWAIT);
    msgrcv(g_retMsgid,pMsg,512,ts,0);
    return pMsg->ret;
}

int unregister_dev(device_t hdl)
{
    PDEVMSG pMsg = NULL;
    long ts;
    struct timeval tv;
    if(g_devMsgid < 0 || g_retMsgid < 0)
    {
        if(msgid_init() < 0)
            return -1;
    }
    pMsg =(PDEVMSG) malloc(sizeof(DEVMSG) + 260);
    gettimeofday(&tv,NULL);
    ts = TIMESTAMP(tv.tv_sec,tv.tv_usec);
    pMsg->timing = ts;
    pMsg->ret = 1;
    pMsg->type = DEVMSGTYPE;
    pMsg->cmd = UNREG_DEV;
    pMsg->sohdl = hdl;
    msgsnd(g_devMsgid,pMsg,512,0);
    msgrcv(g_retMsgid,pMsg,512,ts,0);
    printf("%s\t%d\t%d\n",__func__,__LINE__,pMsg->ret);
    return pMsg->ret;
}

int devctl(device_t hdl,int cmd,void *param,int len)
{
    PDEVMSG pMsg = NULL;
    long ts;
    struct timeval tv;
    if(g_devMsgid < 0 || g_retMsgid < 0)
    {
        if(msgid_init() < 0)
            return -1;
    }
    if(len > (512 - sizeof(DEVMSG)))
    {
        printf("param len is too long\n");
        return -1;
    }
    pMsg =(PDEVMSG) malloc(512);
    gettimeofday(&tv,NULL);
    ts = TIMESTAMP(tv.tv_sec,tv.tv_usec);
    pMsg->timing = ts;
    pMsg->ret = 1;
    pMsg->type = DEVMSGTYPE;
    pMsg->cmd = DEV_CTRL;
    pMsg->sohdl = hdl;
    memcpy(pMsg->param,param,len);
    msgsnd(g_devMsgid,pMsg,512,0);
    msgrcv(g_retMsgid,pMsg,512,ts,0);
    memcpy(param,pMsg->param,len);
    return pMsg->ret;
}
