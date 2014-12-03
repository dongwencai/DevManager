#include "devmsg.h"
#include "stdio.h"
//#include "../deviceManager"

int main()
{
    unsigned int hdl;
    hdl = register_dev("./device/mouse.so");
    printf("%s\t%d\n",__func__,hdl);
    getchar();
    unregister_dev(hdl);
}
