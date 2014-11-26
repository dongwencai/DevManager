#ifndef __DEVMSG_H__
#define __DEVMSG_H__
#define     REG_DEV     0x1000
#define     UNREG_DEV   0x1001
#define     DEV_CTRL    0x1002

int register_dev(char *so_name);

int unregister_dev(char *so_name);

int devctl(char *so_name,int cmd,void *param);
#endif
