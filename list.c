#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void node_del(PLIST *ppHead,PLIST pNode);

PLISTINFO create_list(int nsize,int (*cbcompare)(void *,void *))
{
    PLISTINFO pListInfo = NULL;
    if(pListInfo = malloc(sizeof(LISTINFO)))
    {
        pListInfo->phead = NULL;
        pListInfo->data_size = nsize;
        pthread_mutex_init(&pListInfo->lock,NULL);
        pListInfo->compare = cbcompare;
    }
    return pListInfo;
}

LIST_STATUS list_add(PLISTINFO pListInfo, void *pData)
{
    PLIST pNewNode = NULL;
    if(pListInfo)
    {
        pNewNode = (PLIST)malloc(sizeof(LIST) + pListInfo->data_size);
        if(pNewNode)
        {
            memcpy(pNewNode->data,pData,pListInfo->data_size);
            pthread_mutex_lock(&pListInfo->lock);
            pNewNode->next = pListInfo->phead;
            pListInfo->phead = pNewNode;
            pthread_mutex_unlock(&pListInfo->lock);
        }
    }
    else
        return LIST_FAIL;
    return LIST_SUC;
}
//compare 查找对比函数 相等返回0
PLIST lookup_node(PLISTINFO pListInfo,void *pkey)
{
    PLIST pNode = NULL;
    if(!pListInfo)
    {
        return NULL;
    }
    pthread_mutex_lock(&pListInfo->lock);
    if(pListInfo->compare)
    {
        pthread_mutex_unlock(&pListInfo->lock);
        return NULL;
    }
    pNode = pListInfo->phead;
    while(pNode)
    {
        if(!pListInfo->compare(pkey,pNode->data))
            break;
        pNode = pNode->next;
    }
    pthread_mutex_unlock(&pListInfo->lock);
    return pNode;
}

LIST_STATUS list_del(PLISTINFO pListInfo,PLIST pNode)
{
    PLIST pHead = NULL;
    if(!pListInfo)
    {
        return LIST_FAIL;
    }
    pthread_mutex_lock(&pListInfo->lock);
    pHead = pListInfo->phead;
    node_del(&pHead,pNode);
    pthread_mutex_unlock(&pListInfo->lock);
    return LIST_SUC;
}

static void node_del(PLIST *ppHead,PLIST pNode)
{
    if(*ppHead == pNode)
    {
        free(ppHead);
        *ppHead = NULL;
    }
    while(*ppHead)
    {
        if((*ppHead)->next == pNode)
        {
            (*ppHead)->next = pNode->next;
            free(pNode);
        }
    }

}

static void *list_del_all(void *p)
{
    PLIST *ppHead = p,temp = NULL;
    while(*ppHead)
    {
       temp = *ppHead; 
       free(temp);
       *ppHead = (*ppHead)->next;
    }
    return NULL;
}

LIST_STATUS empty_list(PLISTINFO pListInfo)
{
    pthread_t thid;
    pthread_attr_t thAttr;
    PLIST pHead = NULL;
    if(!pListInfo)
        return LIST_SUC;
    pthread_attr_init(&thAttr);
    pthread_attr_setdetachstate(&thAttr,PTHREAD_CREATE_DETACHED);
    pthread_mutex_lock(&pListInfo->lock);
    pHead = pListInfo->phead;
    pListInfo->phead = NULL;
    pthread_mutex_unlock(&pListInfo->lock);
    pthread_create(&thid,&thAttr,list_del_all,&pHead);
    return LIST_SUC;
}

LIST_STATUS release_list(PLISTINFO pListInfo)
{
    if(!pListInfo)
        return LIST_SUC;
    empty_list(pListInfo);
    pthread_mutex_destroy(&pListInfo->lock);
    free(pListInfo);
    return LIST_SUC;
}
