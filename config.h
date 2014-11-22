#ifndef __CONFIG_H__
#define __CONFIG_H__

//不可修改
#define IPCKEY 0x1
#define DEVICEMANAGER "/ipc/msg/DevManager"     
#define DEVICECONFIG    "/etc/deviceList.conf"
#define SYSMSGTYPE 1
#define DEVMSGTYPE 2
typedef struct{
    long type;
    struct{
        int cmd;
        char context[0];
    }MSG;
}DEVMSG,*PDEVMSG;

#endif

