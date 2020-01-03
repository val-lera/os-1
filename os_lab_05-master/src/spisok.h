#pragma once

//list itself
struct List;
typedef struct List List;

//iterator
struct Iterator;
typedef struct Iterator Iterator;

//list functions
List* list_create();
void list_free(List *l);
//size
int list_size(List *l);
char list_empty(List *l);
//iterators
Iterator* list_begin(List *l);
Iterator* list_end(List *l);
//insert/erase elements
void list_clear(List *l);
void list_insert(List *l, Iterator *i, double val);
void list_erase(List *l, Iterator *i);

//iterator moving
void iterator_left(Iterator *i);
void iterator_right(Iterator *i);
//getting/seting values
double iterator_get(Iterator *i);
void iterator_set(Iterator *i, double val);
//remove iterator
void iterator_free(Iterator *i);