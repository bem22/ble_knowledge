//
// Created by bem22 on 01/04/2021.
//

#ifndef SNIPPETS_LIST_H
#define SNIPPETS_LIST_H

#include <gio/gio.h>

// --- Data structures ---

typedef struct Node
{
    unsigned int string_length;
    int paired;
    char *mac_addr;
    GDBusProxy *device_proxy;

    struct Node *next;
} Node;


typedef struct list
{
    struct Node *node;
    int length;
} list;


static list l;

// --- List functions ---
Node *get_from_index(list *list, unsigned int index);
int push(list *list, Node *n);
int remove_element(list *list, unsigned int index);
void init_list(list *list);
void destroy_list(list *list);
int is_in_list(list* list, char* string);
#endif //SNIPPETS_LIST_H
