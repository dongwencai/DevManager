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
#define DEVICECONFIG    "/etc/deviceList.conf"
typedef struct {
    char so_name[256];
    int (*device_open)();
    void *(*device_listen)();
    MSG (*msg_transale)(void *context);
    int (*device_close)();
}DEVCONTEXT;
int load_device(PLIST pDevList);

static int load_config(const char *config,PLIST *pphead)
{
    FILE *fp = NULL;
    int cnt = 0;
    DEVCONTEXT *p_context = NULL;
    fp = fopen(config,"r");
    if(!fp)     return -1;
    while(1)
    {
       if(!p_context) 
       {
           p_context = (DEVCONTEXT *)malloc(sizeof(DEVCONTEXT));
           if(!p_context)   return -1;
           memset(p_context,0,sizeof(DEVCONTEXT));
       }

       if(fgets(p_context->so_name,256,fp))
       {
          if(p_context->so_name[0] == '#')
          {
              memset(p_context,0,sizeof(DEVCONTEXT));
              continue;
          }
          if(access(p_context->so_name,R_OK|F_OK) != 0)
          {
              memset(p_context,0,sizeof(DEVCONTEXT));
              continue;
          }
          list_add(pphead,p_context);
          cnt ++;
       }
    }
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
   return load_device(device_list);
}

int load_device(PLIST pDevList)
{
   DEVCONTEXT *p_devcontext = NULL; 

}
