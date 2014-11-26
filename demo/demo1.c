#include "devmsg.h"
#include "stdio.h"
//#include "../deviceManager"

int main()
{
    register_dev("./device/testDev2.so");
    getchar();
    unregister_dev("./device/testDev1.so");
}
