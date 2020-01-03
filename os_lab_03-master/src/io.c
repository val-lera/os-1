#include "io.h"
#define BS 256

void fill(char *buf) //read single word from stream
{
    int i = 0; //index

    do { read(STDIN_FILENO, buf, sizeof(char)); } //read blanks
    while ((buf[i] == '\0' || buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\n') && i < BS - 1);

    do { read(STDIN_FILENO, buf + ++i, sizeof(char)); } //read next char
    while (buf[i] != '\0' && buf[i] != ' ' && buf[i] != '\t' && buf[i] != '\n' && i < BS - 1); //stop if blank symbol or overflow
    buf[i] = '\0'; //place zero as last char
}
int readInt() { char buf[BS]; fill(buf); return atoi(buf); }
float readFloat() { char buf[BS]; fill(buf); return atof(buf); }
void writeInt(int num)
{
    if (num < 0) { writeChar('-'); num = -num; }
    char buf[BS], t;
    int len = 0;
    do
    {
        buf[len++] = '0' + num % 10;
        num /= 10;
    }
    while (num > 0);

    for (int i = 0; i < len / 2; i++)
    {
        t = buf[i];
        buf[i] = buf[len - i - 1];
        buf[len - i - 1] = t;
    }
    write(STDOUT_FILENO, buf, len * sizeof(char));
}
void writeFloat(float real, int afterpoint)
{
    if (real < 0) { writeChar('-'); real = -real; }
    char buf[BS], t;
    int len = 0, num = real;
    do
    {
        buf[len++] = '0' + num % 10;
        num /= 10;
    }
    while (num > 0);

    for (int i = 0; i < len / 2; i++)
    {
        t = buf[i];
        buf[i] = buf[len - i - 1];
        buf[len - i - 1] = t;
    }

    buf[len++] = '.';
    for (int i = 0; i < afterpoint; i++) buf[len++] = '0' + (int)(real *= 10) % 10;

    write(STDOUT_FILENO, buf, len * sizeof(char));
}
void writeChar(char c) { write(STDOUT_FILENO, &c, sizeof(char)); }
void writeString(char *str)
{
    int len = 0;
    while (str[len++]);
    write(STDOUT_FILENO, str, len * sizeof(char));
}
void error(char *str, int code)
{
    int len = 0;
    while (str[len++]);
    write(STDERR_FILENO, "Error: ", 8 * sizeof(char));
    write(STDERR_FILENO, str, len * sizeof(char));
    writeChar('\n');
    exit(code);
}