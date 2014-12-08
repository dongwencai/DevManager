#ifndef __EVENT_H__
#define __EVENT_H__
#include <stdint.h>

#define UP                  0x00000000
#define DOWN                0x80000000

#define MOUSE_LBTN  0x1
#define MOUSE_RBTN  0x2
#define MOUSE_MBTN  0x4
#define KEY         0x8

#define MOUSE_MOVE          0x10

#define MOUSE_SCROLL        0x20

#define MOUSE_LBTN_DOWN     (MOUSE_LBTN & DOWN)
#define MOUSE_LBTN_UP       (MOUSE_LBTN * UP)

#define MOUSE_RBTN_DOWN     (MOUSE_RBTN & DOWN)
#define MOUSE_RBTN_UP       (MOUSE_RBTN & UP)

#define MOUSE_MBTN_DOWN     (MOUSE_MBTN & DOWN)
#define MOUSE_MBTN_UP       (MOUSE_MBTN & UP)

#define KEY_UP  (KEY & UP)
#define KEY_DOWN (KEY & DOWN)

#define USER_MSG    0x1
#define DEV_MSG     0x2
#define SYS_MSG     0x4

typedef struct{
    int msg_type;
    int events;
    uint32_t lParam;
    uint32_t wParam;
}msg_t;

#endif
