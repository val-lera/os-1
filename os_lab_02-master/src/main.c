#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define READ_END 0
#define WRITE_END 1
#define BUF_SZ 100

char rid(int from, char where[])
{
    char c;
    for (int i = 0; i < BUF_SZ - 1; i++)
    {
        read(from, &c, sizeof(c));
        if (c == '\n') { where[i] = '\0'; return c; }
        where[i] = c;
    }
    return 0;
}

int main()
{
    char prog[BUF_SZ], fname[BUF_SZ];
    prog[BUF_SZ - 1] = fname[BUF_SZ - 1] = '\0';

    rid(STDIN_FILENO, prog); //read prog name
    rid(STDIN_FILENO, fname); //read file name

    if (fname[0] == '\0')
    {
        execlp(prog, prog, NULL);
        return 0;
    }
    int file = open(fname, O_RDONLY);
    if (file == -1) return -1; //cannot open file
    if (dup2(file, STDIN_FILENO) == -1) { close(file); return -2; } //dup2 had been failing

    pid_t pid = fork();
    if (pid < 0) { close(file); return -3; } //fork was not sharp enough
    if (pid)
    {
        int s;
        waitpid(pid, &s, 0); //wait untill child is end
        close(file);
    }
    else execlp(prog, prog, NULL); //do the job
    return 0;
}
