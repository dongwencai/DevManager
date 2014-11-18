#include "list.h"
#include <stdio.h>
#include <stdlib.h>
void list_add(PLIST *pphead, PLIST link)
{
    if(link)
    {
        if(*pphead)
        {
            link->next = (*pphead);
            *pphead = link;
        }
        else
        {
            *pphead = link;
            link->next = NULL;
        }

    }
}

void list_del(PLIST *pphead,PLIST link)
{
    if(*pphead == link)
    {
        release_node(*pphead);
        *pphead = NULL;
    }
    while(*pphead)
    {
        if((*pphead)->next == link)
        {
            (*pphead)->next = link->next;
            release_node(link);
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
