#ifndef __DEVMSG_H__
#define __DEVMSG_H__
#define     REG_DEV     0x1000
#define     UNREG_DEV   0x1001
#define     DEV_CTRL    0x1002

extern int register_dev(char *so_name);

extern int unregister_dev(unsigned int hdl);

extern int devctl(unsigned int hdl,int cmd,void *param,int len);
#endif
