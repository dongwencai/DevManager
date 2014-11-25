#include "config.h"
#include "devmsg.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int register_dev(char *so_name)
{
    key_t key;
    int msgid;
    PDEVMSG pMsg = NULL;
    key = ftok(DEVICEMANAGER,IPCKEY);
    msgid = msgget(key,0666 | IPC_CREAT);
    pMsg =(PDEVMSG) malloc(sizeof(DEVMSG) + 260);
    pMsg->type = SYSMSGTYPE;
    pMsg->devmsg.cmd = REG_DEV;
    strcpy(pMsg->devmsg.param,so_name);
    if(msgid >= 0)
    {
        msgsnd(msgid,pMsg,512,0);
        msgrcv(msgid,pMsg,512,DEVMSGRET,0);
        puts(pMsg->devmsg.param);
        return 0;
    }
    return -1;
}
