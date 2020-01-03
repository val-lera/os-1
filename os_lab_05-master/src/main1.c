#include "spisok.h"
#include <stdio.h>

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
    return 0;
}