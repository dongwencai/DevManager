#ifndef __DEVICEMANAGER_H__
#define __DEVICEMANAGER_H__
#include <pthread.h>
#include "event.h"
#define SONAMELEN 256
typedef unsigned int device_t;

#define MEMBER_OFF(type,member)    ((unsigned int)(&(((type *)0)->member)))

typedef struct {
    char so_name[SONAMELEN];
    device_t sohdl;
    pthread_t threadHdl;
    int (*device_open)();
    int (*device_ctl)(int cmd,void *p);
    void *(*device_getmsg)();
    int (*msg_transale)(void *context,msg_t *Msg);
    int (*device_close)();
}DEVCONTEXT,*PDEVCONTEXT;

typedef enum{
    DEV_SUC = 1,
    DEV_FAIL,
    DEV_NOT_EXIST,
}DEV_STATUS;

extern int unregister_device(device_t hdl);

extern device_t register_device(const char *name);

extern int device_contrl(char *name,int cmd,void *p);

extern void print_all_divice();
#endif
