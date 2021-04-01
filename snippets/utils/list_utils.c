//
// Created by bem22 on 01/04/2021.
//

#include "list_utils.h"
#include <stddef.h>
#include <stdlib.h>

// These functions serve the custom linked list
Node *pop(list *list, unsigned int index)
{
    struct Node *head = list->node;

    // Cut down index until reaching the desired position
    while (head)
    {
        if (index)
        {
            head = head->next;
            index--;
        }
        else
        {
            return head;
        }
    }

    // If you are here, the node does not exist
    return NULL;
}

int push(list *list, Node *n)
{
    if (!n)
    {
        return -1;
    }

    // Do we have a node in the list?
    if (list->node)
    {
        n->next = list->node; // head:tail
    }
    else
    {
        n->next = NULL; // head:null
    }

    // Now make the list point to the head
    list->node = n;
    list->length++;

    return 0;
}

int remove_element(list *list, unsigned int index)
{
    struct Node *previous;
    struct Node *head;

    previous = NULL;
    head = list->node;

    // Swap head until index
    while (head)
    {
        // Is the index !0 and we have more nodes?
        if (index)
        {
            previous = head;
            head = head->next;
            index--;
        }
        else
        {
            if (previous)
            {
                previous->next = head->next;
            }
            else
            {
                list->node = head->next;
            }
            // Free memory, assign NULL pointer
            free(head->string);
            head->string = NULL;
            free(head);
            head = NULL;

            list->length--;

            // Return success
            return 0;
        }
    }

    // No head? No problem!
    return -1;
}

void init_list(list *list)
{
    list->node = NULL;
    list->length = 0;
}

void destroy_node(Node *n) {
    if(n) {
        destroy_node(n->next);
        free(n->string);
        n->string = NULL;
        free(n);
        n = NULL;
    }
}
void destroy_list(list *list) {
    if(list) { destroy_node(list->node); }
    list = NULL;
}