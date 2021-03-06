#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"
#include "utils.h"

node* add(node* head, char* fileName)
{
    node* new = (node *)malloc(sizeof(node));
    if(new == NULL)
        checkErrors(-1, "Error allocating memory.");

    strcpy(new->fileName, fileName);
    new->next = NULL;
    if(!head)
    {
        return new;
    }
    else
    {
        node* tmp = head;
        while(tmp->next)
        {
            tmp = tmp->next;
        }
        tmp->next = new;
    }
    return head;
}

node* pop(node* head)
{
    if(head)
    {
        node* tmp = head->next;
        free(head);
        return tmp;    
    }
    else return NULL;
}

int search(node* head, char* fileName)
{
    node *tmp = head;
    while(tmp)
    {
        if(strcmp(fileName, tmp->fileName) == 0) return 1;
        tmp = tmp->next;
    }
    return 0;
}

node* removeNode(node* head, char* fileName)
{
    node *tmp = head;
    if(tmp && strcmp(fileName, tmp->fileName) == 0)
    {
        head = head->next;
        free(tmp);
        return head;
    }
    while(tmp && tmp->next)
    {
        if(strcmp(fileName, tmp->next->fileName) == 0)
        {
            node* tmp1 = tmp->next;
            tmp->next = tmp->next->next;
            free(tmp1);
            return head;
        }
        tmp = tmp->next;
    }
    return head;
}
