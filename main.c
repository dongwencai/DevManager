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
#include "list.h"
#include <pthread.h>
#define DEVICECONFIG    "/etc/deviceList.conf"
#define MSG     int
#define SONAMELEN 256

typedef struct {
    char so_name[SONAMELEN];
    void *pHdl;
    pthread_t threadHdl;
    int (*device_open)();
    int (*device_ctl)(int cmd,void *p);
    void *(*device_listen)();
    int (*msg_transale)(void *context,MSG *Msg);
    int (*device_close)();
}DEVCONTEXT,*PDEVCONTEXT;

typedef enum{
    DEV_SUC = 1,
    DEV_FAIL,
    DEV_NOT_EXIST,
}DEV_STATUS;

PLISTINFO device_list = NULL;

int register_device_ex(DEVCONTEXT *pContext);
DEV_STATUS register_device(const char *name);
static void *device_thread(void *pContext);
static int nameCompare(void *src,void *dst);
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
        while(1);
    }

    perror("config file is invalid!\n");
    return 0;
}

DEV_STATUS register_device(const char *name)
{
    DEV_STATUS ret = DEV_SUC;
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
int register_device_ex(DEVCONTEXT *pContext)
{
    pContext->pHdl = dlopen(pContext->so_name,RTLD_NOW); 
    if(pContext->pHdl)
    {
        pContext->device_open = dlsym(pContext->pHdl,"device_open");
        pContext->device_listen = dlsym(pContext->pHdl,"device_listen");
        pContext->msg_transale = dlsym(pContext->pHdl,"msg_transale");
        pContext->device_close = dlsym(pContext->pHdl,"device_close");
    }
    else
    {
        return -DEV_FAIL;
    }
    pthread_create(&pContext->threadHdl,NULL,device_thread,(void *)pContext);
    return DEV_SUC;
}
int unregister_device_ex(PLIST link)
{
    PDEVCONTEXT pContext = NULL;
    if(!link)   return -DEV_NOT_EXIST;
    pthread_cancel(pContext->threadHdl);
    pthread_join(pContext->threadHdl,NULL);
    if(pContext->device_close)
        pContext->device_close();
    list_del(device_list,link);
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
        }
    }

}

static int nameCompare(void *src,void *dst)
{
    PDEVCONTEXT pSrc = (PDEVCONTEXT)src;
    PDEVCONTEXT pDst = (PDEVCONTEXT)dst;
    return strncmp(pSrc->so_name,pDst->so_name,SONAMELEN);
}
