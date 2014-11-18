#include "list.h"
#include <stdio.h>
#include <stdlib.h>
void list_add(PLIST *pphead, void *date)
{
    struct list *newNode;
    newNode = (struct list *)malloc(sizeof(struct list));
    if(newNode)
    {
        newNode->date = date;
        if(*pphead)
        {
            newNode->next = (*pphead);
            *pphead = newNode;
        }
        else
        {
            *pphead = newNode;
            newNode->next = NULL;
        }

    }
    else
    {
        printf("alloc mem failt\n");
    }
}

void list_del(PLIST *pphead,PLIST node)
{
    if(*pphead == node)
    {
        release_node(*pphead);
        *pphead = NULL;
    }
    while(*pphead)
    {
        if((*pphead)->next == node)
        {
            (*pphead)->next = node->next;
            release_node(node);
        }
    }
}

void release_node(PLIST node)
{
    if(node)
    {
        if(node->date)
        {
            free(node->date);
        }
        free(node);
    }
    return ;
}

void release_list(PLIST head)
{
    PLIST nxt = NULL;
    while(head)
    {
        nxt = head->next;
        release_node(head);
        head = nxt;
    }
}
