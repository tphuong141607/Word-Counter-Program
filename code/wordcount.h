#ifndef WORDCOUNT_H
#define WORDCOUNT_H

#include "wordcount.h"
#include <stdio.h>

#define SUCCESS 1
#define UNSUCCESS 0
#define MAX_INPUT_SIZE 300

int inputString(char *prompt, char *inputFileBuffer, size_t size);
struct countValue countChar(FILE *filePointer);
int checkASCII(FILE *filePointer);
void *super_malloc(size_t size);
void *super_realloc(void *ptr, size_t size);

struct countValue {
    int charCount, wordCount, lineCount;
};

#endif
