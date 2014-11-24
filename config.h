#ifndef __CONFIG_H__
#define __CONFIG_H__

//不可修改
#define IPCKEY 0x1
#define DEVICEMANAGER "/ipc/msg/DevManager"     
#define DEVICERETMSG  "/ipc/msg/DeviceRetMsg"
#define DEVICECONFIG    "/etc/deviceList.conf"
#define SYSMSGTYPE 1
#define DEVMSGTYPE 2
#define MSGMAXSIZE 512

typedef struct{
    long type;
    struct{
        int cmd;
        char param[0];
    }devmsg;
}__attribute__((packed, aligned(1))) DEVMSG,*PDEVMSG;

typedef struct{
    long type;
    struct{
        char so_name[256];
        int cmd;
        char param[0];
    }somsg;
}__attribute__((packed, aligned(1))) SOMSG,*PSOMAG;

#endif

