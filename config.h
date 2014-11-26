#ifndef __CONFIG_H__
#define __CONFIG_H__

//不可修改
#define IPCKEY 0x1
#define DEVICEMANAGER "/ipc/msg/DevManager"     
#define DEVICERETMSG  "/ipc/msg/DeviceRetMsg"
#define DEVICECONFIG    "./deviceList.conf"
#define SYSMSGTYPE 1
#define DEVMSGTYPE 2
#define DEVMSGRET  3
#define MSGMAXSIZE 512

typedef struct{
    long type;
    long timing;                        //当前时间s和us组合值，为了当前唯一
    struct{
        int cmd;
        int ret;
        char param[0];
    }devmsg;
}__attribute__((packed, aligned(1))) DEVMSG,*PDEVMSG;

typedef struct{
    long type;
    long timing;
    struct{
        char so_name[256];
        int ret;
        int cmd;
        char param[0];
    }somsg;
}__attribute__((packed, aligned(1))) SOMSG,*PSOMSG;

#endif

