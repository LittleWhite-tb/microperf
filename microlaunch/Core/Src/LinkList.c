/*
Copyright (C) 2011 Exascale Research Center

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "LinkList.h"

SLinkList *linkList_create (void (*print) (void*),
                            void (*destroy) (void*))
{
    SLinkList *res = malloc (sizeof (*res));
    assert (res);
    memset (res, 0, sizeof (*res));

    res->print = print;
    res->destroy = destroy;
    return res;
}

void linkList_destroy (SLinkList *list)
{
    if (list != NULL)
    {
        SLinkListNode *node = list->start,
                      *tmp;
        
        while (node != NULL)
        {
            if (list->destroy)
                list->destroy (node->data), node->data = NULL;

            tmp = node->next;

            free (node), node = NULL;

            node = tmp;
        }
        free(list), list = NULL;
    }
}

void linkList_addHead (SLinkList *list, void *elem)
{
    if (list != NULL)
    {
        SLinkListNode *node = malloc (sizeof (*node));
        assert (node != NULL);
        memset (node, 0, sizeof (*node));

        node->data = elem;
        node->next = list->start;
        list->start = node;
    }
}

void linkList_removeHead (SLinkList *list)
{
    if (list != NULL)
    {
        SLinkListNode *node = list->start;

        if (node != NULL)
        {
            list->start = list->start->next;
        }

        if (list->destroy)
            list->destroy (node->data), node->data = NULL;
        free (node), node = NULL;
    }
}

void* linkList_getHead (SLinkList *list)
{
    if (list == NULL)
        return NULL;

    return list->start;
}

int linkList_isEmpty (SLinkList *list)
{
    if (list == NULL)
        return 1;
    if (list->start == NULL)
        return 1;
    return 0;
}
