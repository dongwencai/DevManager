#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static PLIST node_del(PLIST pHead,PLIST pNode);

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
    pNewNode = (PLIST)malloc(sizeof(LIST) + pListInfo->data_size);
    if(pListInfo && pNewNode)
    {
        memcpy(pNewNode->data,pData,pListInfo->data_size);
        pthread_mutex_lock(&pListInfo->lock);
        pNewNode->next = pListInfo->phead;
        pListInfo->phead = pNewNode;
        pthread_mutex_unlock(&pListInfo->lock);
    }
    else
        return -LIST_FAIL;
    return LIST_SUC;
}
//compare 查找对比函数 相等返回0
PLIST lookup_node(PLISTINFO pListInfo,void *pkey)
{
    PLIST pNode = NULL;
    if(pListInfo && pListInfo->compare)
    {
        pthread_mutex_lock(&pListInfo->lock);
        pNode = pListInfo->phead;
        while(pNode)
        {
            if(!pListInfo->compare(pkey,pNode->data))
                break;
            pNode = pNode->next;
        }
        pthread_mutex_unlock(&pListInfo->lock);
    }
    return pNode;
}

LIST_STATUS list_del(PLISTINFO pListInfo,PLIST pNode)
{
    if(!pListInfo)
    {
        return -LIST_FAIL;
    }
    pthread_mutex_lock(&pListInfo->lock);
    pListInfo->phead = node_del(pListInfo->phead,pNode);
    pthread_mutex_unlock(&pListInfo->lock);
    return LIST_SUC;
}

static PLIST node_del(PLIST pHead,PLIST pNode)
{
    PLIST temp = NULL;
    if(pHead == pNode)
    {
        temp = pHead;
        pHead = pHead->next;
        free(temp);
    }
    else
    {
        temp = pHead;
        while(temp)
        {
            if((temp)->next == pNode)
            {
                temp->next = pNode->next;
                free(temp);
                break;
            }
            temp = temp->next;
        }
    }
    return pHead;
}

static void *list_del_all(void *p)
{
    PLIST pHead = p,temp = NULL;
    while(pHead)
    {
       temp = pHead; 
       pHead = pHead->next;
       free(temp);
    }
    return pHead;
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
    pthread_create(&thid,&thAttr,list_del_all,pHead);
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

#ifdef DEBUG
LIST_STATUS print_list(PLISTINFO pListInfo)
{
    PLIST pHead = NULL;
    char *name = NULL;
    pthread_mutex_lock(&pListInfo->lock);
    pHead = pListInfo->phead;
    while(pHead)
    {
        name = pHead->data;
        printf("%s\n",name);
        pHead = pHead->next;
    }
    pthread_mutex_unlock(&pListInfo->lock);
}
#endif
