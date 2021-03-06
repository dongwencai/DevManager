#ifndef __LIST_H__
#define __LIST_H__

#include <pthread.h>
typedef struct list{
    struct list *next;
    char data[0];
}LIST,*PLIST;

typedef struct{
    PLIST phead;
    pthread_mutex_t lock;
    int data_size;
    int (*compare)(void *src,void *dst,int offset,int compare_len);
}LISTINFO,*PLISTINFO;

typedef enum{
    LIST_SUC = 1,
    LIST_FAIL,
}LIST_STATUS;

extern PLISTINFO create_list(int data_size,int (*compare)(void *,void *,int ,int ));

extern PLIST list_add(PLISTINFO pListInfo, void *pData);

extern LIST_STATUS list_del(PLISTINFO pListInfo,PLIST plink);

extern LIST_STATUS release_list(PLISTINFO pListInfo);

extern LIST_STATUS empty_list(PLISTINFO pListInfo);

extern PLIST lookup_node(PLISTINFO pListInfo,void *pkey,int offset,int len);
#endif
