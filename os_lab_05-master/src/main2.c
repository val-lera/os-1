#include <stdio.h>
#include <dlfcn.h>
//list itself
struct List;
typedef struct List List;

//iterator
struct Iterator;
typedef struct Iterator Iterator;

//list functions
List* (*list_create)();
void (*list_free)(List *l);
//size
int (*list_size)(List *l);
char (*list_empty)(List *l);
//iterators
Iterator* (*list_begin)(List *l);
Iterator* (*list_end)(List *l);
//insert/erase elements
void (*list_clear)(List *l);
void (*list_insert)(List *l, Iterator *i, double val);
void (*list_erase)(List *l, Iterator *i);

//iterator moving
void (*iterator_left)(Iterator *i);
void (*iterator_right)(Iterator *i);
//getting/seting values
double (*iterator_get)(Iterator *i);
void (*iterator_set)(Iterator *i, double val);
//remove iterator
void (*iterator_free)(Iterator *i);

void print_menu()
{
    printf(
        "1 - show menu\n"
        "2 - show list contents\n"
        "3 - insert element\n"
        "4 - erase element\n"
        "5 - clear list\n"
        "6 - set value\n"
        "0 - exit\n"
        );
}

int main()
{
    void *lib = dlopen("./libmyspisok.so", RTLD_LAZY);
    if (!lib)
    {
        printf("Cannot open lib because: %s\n", dlerror());
        return 1;
    }
    //list functions
    list_create = dlsym(lib, "list_create");
    list_free = dlsym(lib, "list_free");
    //size
    list_size = dlsym(lib, "list_size");
    list_empty = dlsym(lib, "list_empty");
    //iterators
    list_begin = dlsym(lib, "list_begin");
    list_end = dlsym(lib, "list_end");
    //insert/erase elements
    list_clear = dlsym(lib, "list_clear");
    list_insert = dlsym(lib, "list_insert");
    list_erase = dlsym(lib, "list_erase");

    //iterator moving
    iterator_left = dlsym(lib, "iterator_left");
    iterator_right = dlsym(lib, "iterator_right");
    //getting/seting values
    iterator_get = dlsym(lib, "iterator_get");
    iterator_set = dlsym(lib, "iterator_set");
    //remove iterator
    iterator_free = dlsym(lib, "iterator_free");

    List *l = list_create(); //create list

    char loop = 1;
    int com;

    print_menu(); //show menu at start

    while (loop)
    {
        printf("> ");
        scanf("%i", &com); //get user command
        switch (com)
        {
        case 0: //exit
            loop = 0;
            break;

        case 1: //show menu
            print_menu();
            break;

        case 2: //show list
            if (list_empty(l)) printf("*list is empty*\n");
            else
            {
                int n = list_size(l), sz = n;
                Iterator *i = list_begin(l);

                while (n--) //go through all list elements
                {
                    printf("%f ", iterator_get(i));
                    iterator_right(i);
                }
                printf("(size: %i)\n", sz); //show size at the end
                iterator_free(i);
            }
            break;
        
        case 3: //insert element
        {
            int pos; double val; //get position and value
            printf("Position: "); scanf("%i", &pos);
            printf("Value: "); scanf("%lf", &val);

            Iterator *i = list_begin(l);

            while (pos--) iterator_right(i); //go to the pos
            list_insert(l, i, val);

            iterator_free(i);
            break;
        }
        
        case 4: //erase element
        {
            int pos; //get position and value
            printf("Position: "); scanf("%i", &pos);

            Iterator *i = list_begin(l);

            while (pos--) iterator_right(i); //go to the pos
            list_erase(l, i);

            iterator_free(i);
            break;
        }

        case 5: //clear list
            list_clear(l);
            break;

        case 6: //set value
        {
            int pos; double val; //get position and value
            printf("Position: "); scanf("%i", &pos);
            printf("Value: "); scanf("%lf", &val);

            Iterator *i = list_begin(l);

            while (pos--) iterator_right(i); //go to the pos
            iterator_set(i, val);

            iterator_free(i);
            break;
        }
        }
    }

    list_free(l); //free memory

    dlclose(lib);
    return 0;
}