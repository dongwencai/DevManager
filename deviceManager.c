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

PLISTINFO   device_list = NULL;
static int g_msgid = -1;
static int g_retmsgid = -1;         
static int load_config(const char *config,PLISTINFO pDevList);
static int register_device_ex(DEVCONTEXT *pContext);
static void *device_thread(void *pContext);
static int nameCompare(void *src,void *dst);
static int unregister_device_ex(PLIST link);
static int  msg_loop();
static void *dev_msg_loop(void *p);

int pthread_create_detached(void *(*thread_func)(void *),void *p)
{
    pthread_t thread_id;
    pthread_attr_t attr;
    int ret ;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&thread_id,&attr,thread_func,p);
    pthread_attr_destroy(&attr);
    return ret;
}
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
    DEVCONTEXT context;
    fp = fopen(config,"r");
    if(!fp)     return -DEV_FAIL;
    while(1)
    {
       if(fgets(context.so_name,256,fp))
       {
          strip_illegal_char(context.so_name);
          if(context.so_name[0] == '#')
          {
              continue;
          }
          if(register_device_ex(&context) > 0)
          {
              list_add(pDevList,&context);
              cnt ++;
          }
       }
    }
    fclose(fp);
    return cnt;
    
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
    if(device_list && load_config(p_config_name,device_list) > 0)
    {
        key_t key = ftok(DEVICEMANAGER,IPCKEY);
        g_msgid = msgget(key,0666 | IPC_CREAT);
        key = ftok(DEVICERETMSG,IPCKEY);
        g_retmsgid = msgget(key,0666 | IPC_CREAT);
        pthread_create_detached(dev_msg_loop,NULL);
        while(g_msgid >= 0)
        {
            msg_loop();
        }
    }

    perror("config file is invalid!\n");
    return 0;
}

DEV_STATUS register_device(const char *name)
{
    PLIST link = NULL;
    DEVCONTEXT context;
    link = lookup_node(device_list,(void *)name);
    if(!link)
    {
        strncpy(context.so_name,name,SONAMELEN);
        if(register_device_ex(&context) > 0)
        {
            list_add(device_list,&context);
            return DEV_SUC;
        }
    }
    return -DEV_FAIL;
}

static int register_device_ex(DEVCONTEXT *pContext)
{
    pContext->pHdl = dlopen(pContext->so_name,RTLD_NOW); 
    if(pContext->pHdl)
    {
        pContext->device_open = dlsym(pContext->pHdl,"device_open");
        pContext->device_listen = dlsym(pContext->pHdl,"device_listen");
        pContext->msg_transale = dlsym(pContext->pHdl,"msg_transale");
        pContext->device_close = dlsym(pContext->pHdl,"device_close");
        pContext->device_ctl = dlsym(pContext->pHdl,"device_ctl");
    }
    else
    {
        return -DEV_FAIL;
    }
    pthread_create(&pContext->threadHdl,NULL,device_thread,(void *)pContext);
    return DEV_SUC;
}

DEV_STATUS unregister_device(const char *name)
{
    PLIST plink = NULL;
    plink = lookup_node(device_list,(void *)name);
    if(plink)
    {
        unregister_device_ex(plink);
    }
    return DEV_SUC;
}

static int unregister_device_ex(PLIST link)
{
    PDEVCONTEXT pContext = NULL;
    if(!link)   return DEV_SUC;
    pthread_cancel(pContext->threadHdl);
    pthread_join(pContext->threadHdl,NULL);
    if(pContext->device_close)
        pContext->device_close();
    list_del(device_list,link);
    return DEV_SUC;
}

static void *device_thread(void *pContext)
{
    PDEVCONTEXT pDevContext = NULL;
    int ret = 0;
    void *p = NULL;
    MSG msg;
    pDevContext = (DEVCONTEXT *)pContext;
    if(pDevContext->device_open)
    {
        pDevContext->device_open();
    }
    while(1)
    {
        p = pDevContext->device_listen();
        ret = pDevContext->msg_transale(p,&msg);
        if(ret)
        {
            //将消息传递出去
            printf("msg : %d\n",msg);
        }
    }
    return NULL;
}

static int nameCompare(void *src,void *dst)
{
    PDEVCONTEXT pSrc = (PDEVCONTEXT)src;
    PDEVCONTEXT pDst = (PDEVCONTEXT)dst;
    return strncmp(pSrc->so_name,pDst->so_name,SONAMELEN);
}

int device_contrl(char *name,int cmd,void *p)
{
    PLIST plink = NULL;
    int ret = -1;
    plink = lookup_node(device_list,(void *)name);
    if(plink)
    {
       PDEVCONTEXT pContext =(PDEVCONTEXT) plink->data; 
       if(pContext->device_ctl)
           ret = pContext->device_ctl(cmd,p);
    }
    return ret;
}
static int  msg_loop()
{
    DEVMSG msg;
    int ret;
    ret = msgrcv(g_msgid,&msg,MSGMAXSIZE,SYSMSGTYPE,IPC_NOWAIT);
    if(ret == 0)
    {
        int cmd = msg.devmsg.cmd;  
        switch(cmd)
        {
            case REG_DEV:
                register_device((char *)msg.devmsg.param);
                break;
            case UNREG_DEV:
                unregister_device((char *)msg.devmsg.param);
                break;
            default:
                break;
        }
    }
    return DEV_SUC;
}
static void *dev_msg_loop(void *p)
{
    int ret;
    PLIST   pLink = NULL;
    PDEVCONTEXT pContext = NULL;
    SOMSG msg;
    while(1)
    {
        ret = msgrcv(g_msgid,&msg,MSGMAXSIZE,DEVMSGTYPE,IPC_NOWAIT);
        if(ret == 0)
        {
            pLink = lookup_node(device_list,msg.somsg.so_name);           
            if(pLink)
            {
                pContext = (PDEVCONTEXT)pLink->data;
                pContext->device_ctl(msg.somsg.cmd,msg.somsg.param);
                msg.type = DEVMSGTYPE;
                if(g_retmsgid >= 0)
                    msgsnd(g_retmsgid,&msg,MSGMAXSIZE,0);
            }

        }
    }
}
