#include <stdio.h>

char swap(char c)
{
    if (c >= 'a' && c <= 'z') putchar(c + 'A' - 'a');
    else if (c >= 'A' && c <= 'Z') putchar(c + 'a' - 'A');
    else if (c != ' ' && c != '\n' && c != '\t') printf("\nchar:%i\n", (int)c);
    else putchar(c);
}

int main(int argc, char *argv[])
{
    char c;
    while ((c = getchar()) != EOF) swap(c);
    putchar('\n');
    return 0;
}