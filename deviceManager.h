#ifndef __DEVICEMANAGER_H__
#define __DEVICEMANAGER_H__

#define MSG     int
#define SONAMELEN 256

typedef struct {
    char so_name[SONAMELEN];
    void *pHdl;
    pthread_t threadHdl;
    int (*device_open)();
    int (*device_ctl)(int cmd,void *p);
    void *(*device_getmsg)();
    int (*msg_transale)(void *context,MSG *Msg);
    int (*device_close)();
}DEVCONTEXT,*PDEVCONTEXT;

typedef enum{
    DEV_SUC = 1,
    DEV_FAIL,
    DEV_NOT_EXIST,
}DEV_STATUS;

extern DEV_STATUS unregister_device(const char *name);

extern DEV_STATUS register_device(const char *name);

extern int device_contrl(char *name,int cmd,void *p);

extern void print_all_divice();
#endif
