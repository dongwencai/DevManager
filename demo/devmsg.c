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
    pMsg =(PDEVMSG) malloc(sizeof(DEVMSG) + 260);
    gettimeofday(&tv,NULL);
    ts = TIMESTAMP(tv.tv_sec,tv.tv_usec);
    pMsg->timing = ts;
    pMsg->devmsg.ret = 1;
    pMsg->type = SYSMSGTYPE;
    pMsg->devmsg.cmd = REG_DEV;
    strcpy(pMsg->devmsg.param,so_name);
    msgsnd(g_devMsgid,pMsg,512,IPC_NOWAIT);
    msgrcv(g_retMsgid,pMsg,512,ts,0);
    return pMsg->devmsg.ret;
}

int unregister_dev(char *so_name)
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
    pMsg->devmsg.ret = 1;
    pMsg->type = SYSMSGTYPE;
    pMsg->devmsg.cmd = UNREG_DEV;
    strcpy(pMsg->devmsg.param,so_name);
    msgsnd(g_devMsgid,pMsg,512,0);
    msgrcv(g_retMsgid,pMsg,512,ts,0);
    printf("%s\t%d\t%d\n",__func__,__LINE__,pMsg->devmsg.ret);
    return pMsg->devmsg.ret;
}

int devctl(char *so_name,int cmd,void *param)
{
    PSOMSG pMsg;
    long ts;
    struct timeval tv;
    if(g_devMsgid < 0 || g_retMsgid < 0)
    {
        if(msgid_init() < 0)
            return -1;
    }
    pMsg =(PSOMSG) malloc(sizeof(PSOMSG ) + 260);
    gettimeofday(&tv,NULL);
    ts = TIMESTAMP(tv.tv_sec,tv.tv_usec);
    pMsg->timing = ts;
    pMsg->somsg.ret = 1;
    pMsg->type = SYSMSGTYPE;
    pMsg->somsg.cmd = DEV_CTRL;
    strcpy(pMsg->somsg.so_name,so_name);
    msgsnd(g_devMsgid,pMsg,512,0);
    msgrcv(g_retMsgid,pMsg,512,ts,0);
    return pMsg->somsg.ret;
}
