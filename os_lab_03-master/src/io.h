#pragma once

#include <stdlib.h>
#include <unistd.h>

int readInt();
float readFloat();
void writeInt(int num);
void writeFloat(float real, int afterpoint);
void writeChar(char c);
void writeString(char *str);
void error(char *str, int code);