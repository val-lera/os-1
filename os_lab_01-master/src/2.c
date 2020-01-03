#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE "\n\n=================================\n\n"

int main()
{
    write(STDOUT_FILENO, LINE, sizeof(LINE) / sizeof(char));
    void *a = malloc(10);
    write(STDOUT_FILENO, LINE, sizeof(LINE) / sizeof(char));
    free(a);
    write(STDOUT_FILENO, LINE, sizeof(LINE) / sizeof(char));
    return 0;
}
