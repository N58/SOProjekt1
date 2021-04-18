#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct node{
    char fileName[256];
    struct node *next;
} node;

node* add(node* head, char* fileName);
node* pop(node* head);
int search(node* head, char* fileName);
node* removeNode(node* head, char* fileName);
void printNodes(node* head);

#endif
