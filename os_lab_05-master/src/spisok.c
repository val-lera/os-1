#include "spisok.h"
#include <stdlib.h>

//list itself
struct Node
{
    double value;
    struct Node *next, *prev;
};
struct Iterator
{
    struct Node *pointer;
};
struct List
{
    struct Node *head, *tail;
};

//list functions
List* list_create()
{
    List *list = malloc(sizeof(List)); //create list
    list->head = list->tail = malloc(sizeof(struct Node)); //create terminator
    list->tail->next = list->tail->prev = NULL; //setup
    return list;
}
void list_free(List *l)
{
    list_clear(l); //remove all elements
    free(l->tail); //remove terminator
    free(l); //remove list
}
//size
int list_size(List *l)
{
    int sz = 0;
    struct Node *node = l->head;
    while (node != l->tail) //how many elements between head and terminator
    {
        node = node->next;
        sz++;
    }
    return sz;
}
char list_empty(List *l)
{
    return l->head == l->tail; //no elements except terminator
}
//iterators
Iterator* list_begin(List *l)
{
    Iterator *iter = malloc(sizeof(Iterator)); //create iterator
    iter->pointer = l->head; //setup
    return iter;
}
Iterator* list_end(List *l)
{
    Iterator *iter = malloc(sizeof(Iterator)); //create iterator
    iter->pointer = l->tail; //setup
    return iter;
}
//insert/erase elements
void list_clear(List *l)
{
    struct Node *node;
    while (l->head != l->tail)
    {
        node = l->head->next;
        free(l->head); //remove element
        l->head = node; //go to next one
    }
    l->head->prev = NULL;
}
void list_insert(List *l, Iterator *i, double val) //insert BEFORE iterator
{
    struct Node *node = i->pointer; //current element
    struct Node *memory = malloc(sizeof(struct Node)); //allocate memory
    memory->value = val;

    //rearrange pointers
    memory->next = node;
    memory->prev = node->prev;
    node->prev = memory;

    if (memory->prev) memory->prev->next = memory; //not first element
    else l->head = memory; //first element
}
void list_erase(List *l, Iterator *i)
{
    struct Node *node = i->pointer; //current element
    if (!node->next) return; //attempt to erase terminator

    //redirect pointers
    node->next->prev = node->prev;
    if (node->prev) node->prev->next = node->next; //not first element
    else l->head = node->next; //first element

    free(node); //deallocate memory
}

//iterator moving
void iterator_left(Iterator *i)
{
    if (i->pointer->prev) i->pointer = i->pointer->prev;
}
void iterator_right(Iterator *i)
{
    if (i->pointer->next) i->pointer = i->pointer->next;
}
//getting/seting values
double iterator_get(Iterator *i)
{
    return i->pointer->value;
}
void iterator_set(Iterator *i, double val)
{
    i->pointer->value = val;
}
//remove iterator
void iterator_free(Iterator *i)
{
    free(i);
}