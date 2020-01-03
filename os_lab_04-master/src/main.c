#include <string.h>
#include <stdio.h> 
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h> 

const char FNAME[] = "pipe.txt";
const size_t LEN = 80;

int main()
{
    //open temp file
    int fd = open(FNAME, O_RDWR | O_CREAT | O_TRUNC);
    if (fd < 0) //error
    {
        printf("ERROR: cannot create file %s\n", FNAME);
        remove(FNAME); return -1;
    }
    //set size
    if (lseek(fd, LEN * 2 - 1, SEEK_SET) == -1)
    {
        printf("ERROR: cannot set size file %s\n", FNAME);
        close(fd); remove(FNAME); return -2;
    }
    if (write(fd, "", 1) != 1)
    {
        printf("ERROR: cannot set size file %s\n", FNAME);
        close(fd); remove(FNAME); return -3;
    }
    //map file
    void *addr;
    if ((addr = mmap(NULL, LEN * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        printf("ERROR: cannot map file %s\n", FNAME);
        close(fd); remove(FNAME); return -4;
    }
    //UI
    printf("Write the name of programm:\n");
    read(STDIN_FILENO, addr, LEN);

    printf("Write the name of file:\n");
    read(STDIN_FILENO, addr + LEN, LEN);

    //fork
    pid_t pid = fork();
    if (pid < 0)
    {
        printf("ERROR: cannot fork\n");
        close(fd); remove(FNAME); return -5;
    }
    else if (pid > 0) //parent
    {
        int s;
        waitpid(pid, &s, 0); //wait untill child is end
        close(fd); remove(FNAME); //close everything
    }
    else //child
    {
        char prog_name[LEN], input_name[LEN];
        //read program name
        for (int i = 0; i < LEN; i++)
        {
            prog_name[i] = ((char*)addr)[i];
            if (prog_name[i] == '\n') { prog_name[i] = '\0'; break; }
        }
        //read file name
        for (int i = 0; i < LEN; i++)
        {
            input_name[i] = ((char*)addr)[i + LEN];
            if (input_name[i] == '\n') { input_name[i] = '\0'; break; }
        }
        printf("%s\n%s\n", prog_name, input_name);

        if (input_name[0] != '\0') //if has second argument, read from file
        {
            //open file
            int inp = open(input_name, O_RDONLY);
            if (inp < 0)
            {
                printf("ERROR: cannot open file %s\n", input_name);
                return -6;
            }
            //redirect input
            if (dup2(inp, STDIN_FILENO) == -1)
            if (inp < 0)
            {
                printf("ERROR: cannot open file %s\n", input_name);
                close(inp); return -6;
            }
        } //else read from console
        
        //execute program
        execlp(prog_name, prog_name, NULL);
    }

    return 0;
}