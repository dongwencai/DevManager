#include "devmsg.h"
#include "stdio.h"
//#include "../deviceManager"

int main()
{
    register_dev("./device/mouse.so");
    getchar();
    unregister_dev("./device/testDev1.so");
}
