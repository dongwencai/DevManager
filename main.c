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
#include <list.h>
#include <pthread.h>
#define DEVICECONFIG    "/etc/deviceList.conf"
#define MSGTYPE     int
typedef struct {
    char so_name[256];
    void *pHdl;
    pthread_t threadHdl;
    int (*device_open)();
    void *(*device_listen)();
    MSGTYPE (*msg_transale)(void *context);
    int (*device_close)();
}DEVCONTEXT;

int register_device_ex(DEVCONTEXT *pContext);
static void *device_thread(void *pContext);
static int load_config(const char *config,PLIST *pphead)
{
    FILE *fp = NULL;
    int cnt = 0;
    DEVCONTEXT *p_context = NULL;
    fp = fopen(config,"r");
    if(!fp)     return -1;
    while(1)
    {
       p_context = (DEVCONTEXT *)malloc(sizeof(DEVCONTEXT));
       if(!p_context)   return -1;
       memset(p_context,0,sizeof(DEVCONTEXT));

       if(fgets(p_context->so_name,256,fp))
       {
          if(p_context->so_name[0] == '#')
          {
              free(p_context);
              continue;
          }
          if(register_device_ex(p_context) < 0)
          {
              free(p_context);
              p_context = NULL;
          }
          else
          {
              list_add(pphead,p_context);
              p_context = NULL;
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
    PLIST device_list = NULL;
    if(argc > 1)
    {
        p_config_name = argv[1];
    }
    if(access(p_config_name,F_OK|R_OK) != 0)
    {
        perror("config file not exist or Limited access!\n");
        exit(-1);
    }
    if(load_config(p_config_name,&device_list) <= 0)
    {
        perror("config file is invalid!\n");
        exit(-1);
    }

   return 0;
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
        return -1;
    }
    if(pContext->device_open)
        pContext->device_open();
    pthread_create(&pContext->threadHdl,NULL,device_thread,(void *)pContext);
    return 0;
}
static void *device_thread(void *pContext)
{
    DEVCONTEXT *pDevContext = NULL;
    pDevContext = (DEVCONTEXT *)pContext;
    if(pDevContext->device_open)
        pthread_attr_t thread_attr;
}
