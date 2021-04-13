//
// Created by bem22 on 01/04/2021.
//

#include "list_utils.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Node *get_from_MAC(list *list, char* mac_addr) {
    struct Node *head = list->node;

    // Cut down index until reaching the desired position
    while (head)
    {
        if (strcmp(head->mac_addr, mac_addr) != 0)
        {
            head = head->next;
        } else if(strcmp(head->mac_addr, mac_addr) == 0){
            return head;
        }
    }

    // If you are here, the node does not exist
    return NULL;
}

// These functions serve the custom linked list
Node *get_from_index(list *list, unsigned int index)
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
            free(head->mac_addr);
            head->mac_addr = NULL;
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

int is_in_list(list* list, char* object_path) {
    struct Node *previous;
    struct Node *head;

    previous = NULL;
    head = list->node;

    while(head) {
        if(strcmp(head->mac_addr, object_path) == 0) {
            printf("hello!\n");
            return 1;
        }
    }

    return 0;

}

void init_list(list *list)
{
    list->node = NULL;
    list->length = 0;
}

void destroy_node(Node *n) {
    if(n) {
        destroy_node(n->next);
        free(n->mac_addr);
        n->mac_addr = NULL;
        free(n);
        n = NULL;
    }
}
void destroy_list(list *list) {
    if(list) { destroy_node(list->node); }
    list = NULL;
}