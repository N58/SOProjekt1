#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <dirent.h>

typedef struct node{
    char fileName[NAME_MAX + 1];
    struct node *next;
} node;

node* add(node* head, char* fileName);
node* pop(node* head);
int search(node* head, char* fileName);
node* removeNode(node* head, char* fileName);

#endif
