#include "devmsg.h"
#include "stdio.h"
//#include "../deviceManager"

int main()
{
    unsigned int hdl1,hdl2;
    int ret = 0;
    hdl1 = register_dev("./device/mouse.so");
    printf("%s\t%d\n",__func__,hdl1);
    getchar();
    /*
    hdl2 = register_dev("./device/testDev1.so");
    printf("%s\t%d\n",__func__,hdl2);
    getchar();
    */
    ret = unregister_dev(hdl1);
    printf("%s\t%d\n",__func__,ret);
}
