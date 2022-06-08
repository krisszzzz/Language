
#if !defined TEXTLIB_INCLUDED

#define TEXTLIB_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define IGNORE_SPACES()                                  \
while(**expression == ' ' || **expression == '\t')       \
    ++*expression;

#define IGNORE_NEWLINES_AND_SPACES()                                                                \
while(**expression == '\n' || **expression == ' ' || **expression == '\t' || **expression == '\r')  \
    ++*expression;      

char* MakeBufferForFile(FILE* source, int* buffer_size);

int   InputString(char* buffer, int buffer_size);

int   WordLength(const char* expression);

#endif