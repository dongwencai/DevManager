/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  none
 *
 *        Version:  1.0
 *        Created:  2014年11月10日 14时45分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dongwencai
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <pthread.h>
#include <sys/msg.h>
#include "list.h"
#include "stringex.h"
#include "deviceManager.h"
#include "devmsg.h"
#include "config.h"
#include "event.h"

PLISTINFO   device_list = NULL;
static int g_msgid;
static int g_retmsgid = -1;         

static int load_config(const char *config,PLISTINFO pDevList);
static device_t register_device_ex(DEVCONTEXT *pContext);
static void *device_thread(void *pContext);
static int nameCompare(void *src,void *dst,int offset,int len);
static int unregister_device_ex(PLIST link);
static int  msg_init();
static int  msg_loop();
static void *dev_msg_loop(void *p);
static int dev_ioctl(PDEVMSG pMsg);
static int msg_release();

void print_all_divice()
{
    PLIST pHead = NULL;
    PDEVCONTEXT pContext = NULL;
    if(device_list)
    {
        pthread_mutex_lock(&device_list->lock);
        pHead = device_list->phead;
        while(pHead)
        {
            pContext =(PDEVCONTEXT) pHead->data;
            puts(pContext->so_name);
            pHead = pHead->next;
        }
        pthread_mutex_unlock(&device_list->lock);

    }
}
static int load_config(const char *config,PLISTINFO pDevList)
{
    FILE *fp = NULL;
    int cnt = 0;
    PLIST link = NULL;
    char so_name[256];
    fp = fopen(config,"r");
    if(!fp)     return -DEV_FAIL;
    while(1)
    {
       if(fgets(so_name,256,fp))
       {
          strip_illegal_char(so_name);
          if(so_name[0] == '#')
          {
              continue;
          }
          if(register_device(so_name) >= 0 && ++ cnt);
       }
       else
          break; 
    }
    fclose(fp);
    return cnt;
}

static int msg_release()
{
    int ret;
    if(g_msgid >=0)
        ret = msgctl(g_msgid,IPC_RMID,0);
    if(g_retmsgid >=0)
        ret |= msgctl(g_retmsgid,IPC_RMID,0);
    return ret;
}

static int  msg_init()
{
    key_t key ;
    key  = ftok(DEVICEMANAGER,IPCKEY);
    g_msgid = msgget(key,0666 | IPC_CREAT);
    key = ftok(DEVICERETMSG,IPCKEY);
    g_retmsgid = msgget(key,0666 | IPC_CREAT);
    return (g_msgid | g_retmsgid);
}

int main(int argc,char *argv[])
{
    char *p_config_name = DEVICECONFIG;
    if(argc > 1)
    {
        p_config_name = argv[1];
    }
    if(access(p_config_name,F_OK|R_OK) != 0)
    {
        perror("config file not exist or Limited access!\n");
        exit(-1);
    }
    device_list = create_list(sizeof(DEVCONTEXT),nameCompare);
    if(device_list && load_config(p_config_name,device_list) >= 0 && msg_init() > 0)
    {
        //pthread_create_detached(dev_msg_loop,NULL);
        msg_loop();
    }
    msg_release();
    perror("config file is invalid!\n");
    return 0;
}

device_t register_device(const char *name)
{
    PLIST link = NULL;
    DEVCONTEXT context;
    PDEVCONTEXT pContext;
    device_t devHdl = 0;
    link = lookup_node(device_list,(void *)name,MEMBER_OFF(DEVCONTEXT,so_name),strlen(name) + 1);
    if(!link)
    {
        strncpy(context.so_name,name,SONAMELEN);
        if((devHdl = register_device_ex(&context)) > 0)
        {
            if(context.device_open && context.device_open() >= 0)
            {
                link = list_add(device_list,&context);
            }
            if(link)
            {
                pContext = (PDEVCONTEXT)link->data;
                pthread_create(&pContext->threadHdl,NULL,device_thread,(void *)pContext);
                return devHdl;
            }
        }
    }
    return 0;
}

static device_t register_device_ex(PDEVCONTEXT pContext)
{
    void *sohdl = NULL;
    int ret = -1;
    sohdl = dlopen(pContext->so_name,RTLD_NOW); 
    if(sohdl)
    {
        pContext->sohdl = (device_t)sohdl;
        pContext->device_open = dlsym(sohdl,"device_open");
        pContext->device_getmsg = dlsym(sohdl,"device_getmsg");
        pContext->msg_transale = dlsym(sohdl,"msg_transale");
        pContext->device_close = dlsym(sohdl,"device_close");
        pContext->device_ctl = dlsym(sohdl,"device_ctl");
        return  (device_t)pContext->sohdl;
    }
    return 0;
}

int unregister_device(device_t hdl)
{
    PLIST plink = NULL;
    int ret = -1;
    plink = lookup_node(device_list,(void *)&hdl,MEMBER_OFF(DEVCONTEXT,sohdl),sizeof(hdl));
    if(plink)
    {
        ret = unregister_device_ex(plink);
    }
    return ret;
}

static int unregister_device_ex(PLIST link)
{
    PDEVCONTEXT pContext = NULL;
    int ret = 0;
    pContext = (PDEVCONTEXT)link->data;
    if(pContext->device_close)
        pContext->device_close();
    pthread_cancel(pContext->threadHdl);
    pthread_join(pContext->threadHdl,NULL);
    ret = dlclose((void *)(pContext->sohdl));
    ret |= list_del(device_list,link);
    return ret;
}

static void *device_thread(void *pContext)
{
    PDEVCONTEXT pDevContext = NULL;
    int ret = 0;
    void *p = NULL;
    msg_t msg;
    pDevContext = (DEVCONTEXT *)pContext;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    while(1)
    {
        p = pDevContext->device_getmsg();
        ret = pDevContext->msg_transale(p,&msg);
        if(ret >= 0)
        {
            //将消息传递出去
        }
    }
    return NULL;
}

static int nameCompare(void *src,void *dst,int offset,int len)
{
    PDEVCONTEXT pDst = (PDEVCONTEXT)dst;
    return memcmp(src,(void *)((unsigned int)pDst+offset),len);
}

static int  msg_loop()
{
    PDEVMSG pMsg;
    int ret;
    device_t hdl = 0;
    pMsg = (PDEVMSG)malloc(sizeof(DEVMSG)+512);
    while(pMsg)
    {
        ret = msgrcv(g_msgid,pMsg,MSGMAXSIZE,DEVMSGTYPE,0);
        if(ret > 0)
        {
            ret = -1;
            switch(pMsg->cmd)
            {
                case REG_DEV:
                    hdl = register_device((char *)pMsg->param);
                    break;
                case UNREG_DEV:
                    ret = unregister_device(pMsg->sohdl);
                    break;
                case DEV_CTRL:
                    ret =  dev_ioctl(pMsg);
                default:
                    break;
            }
        }
        if(pMsg->ret)
        {
            pMsg->ret = ret;
            pMsg->sohdl = hdl;
            pMsg->type = pMsg->timing;
            msgsnd(g_retmsgid,pMsg,MSGMAXSIZE,0);
        }
    }
    return DEV_SUC;
}
static int dev_ioctl(PDEVMSG pMsg)
{
    int ret = -1;
    PLIST plink = NULL; 
    PDEVCONTEXT pContext = NULL;
    plink = lookup_node(device_list,(void *)&pMsg->sohdl,MEMBER_OFF(DEVCONTEXT,sohdl),sizeof(pMsg->sohdl));
    if(plink)
    {
        pContext = (PDEVCONTEXT)plink->data;
        if(pContext->device_ctl)
            ret = pContext->device_ctl(pMsg->cmd,pMsg->param);
    }
    return ret;
}
