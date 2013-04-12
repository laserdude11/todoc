/* linkedlist.c
 *
 * A basic linked list implementation.
 */ 
#include "stdio.h"
#include "stdlib.h"
#include "linkedlist.h"

LList* new_llist(){
    LList* this = (LList*)malloc(sizeof(LList));

    // Extra construction stuff....
    this->head = NULL;
    this->tail = NULL;
    return this;
}

int lladd(LList* l, void* item)
{
    struct element* elem = (struct element*) malloc(sizeof(struct element));
    if (elem == NULL){
        fprintf(stderr, "Could not create new linked list element.");
        goto error;
    }
    elem->data = item;
    elem->prev = NULL;
    elem->next = NULL;

    // No head of list, there is no list.
    if (l->head == NULL){
        l->head = elem;
        l->tail = elem;
        return 0;
    } else {
        l->tail->next = elem;
        elem->prev = l->tail;
        l->tail = elem;
    }
error:
    return -1;
}

// Destroys the linked list.
int destroy_llist(LList* list)
{
    if (!list) return;
    if (!(list->head)){free(list); return;}
    struct element* index = list->head;
    struct element* previous = NULL;
    while (index){
        previous = index;
        index = index->next;
        free(previous);
    }
    free(list);
}
