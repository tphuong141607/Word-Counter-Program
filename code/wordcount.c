
/* Program description: Write a program that shall count the number of lines,
 * words, and characters in a file and write the number into another file.
 *
 * Author: Thao Phuong
 * Date created: May 9, 2019
 */

#include "wordcount.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>


/****** GET INPUT FROM USER (stdin) ******/
int inputString(char *prompt, char *inputFileBuffer, size_t size) {
    // print prompt
    if (prompt != NULL) {
        printf("%s", prompt);
        fflush(stdout);
    }
    
    // read and store the input from user to buffer
    int bufferSize, validEntry;
    do {
        if (fgets(inputFileBuffer, size, stdin) == NULL) {
            return UNSUCCESS;
        };
        
        // handle empty input (the inputFileBuffer has only newline character)
        bufferSize = strlen(inputFileBuffer);
        if (bufferSize == 1) {
            printf("Empty entry, please provide a valid file name: ");
        }
        
        // handle input with the size greater than MAX_INPUT_SIZE
        if (strchr(inputFileBuffer, '\n') == NULL) {
            validEntry = 0;
            char ch;
            while(((ch = fgetc(stdin)) != '\n') && (ch != EOF));
            printf("File name is way too long, please provide a valid file name: ");
        } else validEntry = 1;
        
    } while ((bufferSize <= 1) || (validEntry == 0));
    
    // remove newline and give string back to caller.
    inputFileBuffer[bufferSize-1] = '\0';
    return SUCCESS;
}

/****** CHECK ASCII ******
 * The checkASCII(FILE *filePointer) function tests if a given file is
 * represented as a valid 7–bit US-ASCII character. The function returns
 * 0 if the file contains non-ASCII, and 1 otherwise.
 */
int checkASCII(FILE *filePointer) {
    int c;
    while ((c = getc(filePointer)) != EOF) {
        if (c < 0 || c > 127) { return UNSUCCESS; }
    }
    return SUCCESS;
}

/****** COUNTING CHARACTER, LINE, WORD ******
 * Line: count all lines that end with a newline character
 * Word: count all words that contain at least an alphabet or a digit.
 *       Example: "..." or "   " is not counted as a word
 * Character: count all characters that are valid and visible (tap and space are counted)
 */
struct countValue countChar(FILE *filePointer) {
    int charCount = 0, wordCount = 0, lineCount = 0;
    size_t memorySpace = 20;
    char *tempWord = (char*) super_malloc(memorySpace);
    
    char c;
    if((c = getc(filePointer))) {
        if (c != EOF) {
            lineCount++; // the file always have 1 line if the first character is not EOF
        }
        
        do {
            // Counting lines
            if (c == '\n') { lineCount++; }
            
            // Counting words
            // If the current char is digit or alphabet, append it to the temporary string
            if((isdigit(c)) || (isalpha(c))) {
                if (strlen(tempWord) == 20) {
                    tempWord = super_realloc(tempWord, memorySpace * 2);
                }
                strcat(tempWord, &c);
            }
            
            // When we encounter a space, newline, tap or EOF, we will check if the word is "valid"
            if (c == ' ' || c == '\n' || c == '\t' || c == EOF) {
                // tempWord != 0 means that the word has either digit or alphabet
                if (strlen(tempWord) != 0) {
                    wordCount++;
                }
                strcpy(tempWord, ""); // reset the tempWord
            }
            
            // Counting characters
            // We don't count EOF and white space character (except regular space) as a character.
            if((!isspace(c) && (c != EOF)) || (c == ' ') || (c == '\t')) { charCount++; }
            
            // break the loop if the current the current char is an EOF
            if (c == EOF) { break; }
            
        } while((c = getc(filePointer)));
    }
    free(tempWord);
    struct countValue result = {charCount, wordCount, lineCount};
    return result;
}

/****** SAFE MALLOC CALL ******/
void *super_malloc(size_t size) {
    void *newPointerMalloc = malloc(size);
    if (newPointerMalloc == NULL) {
        perror("malloc");
        return UNSUCCESS;
    }
    return newPointerMalloc;
}

/****** SAFE REALLOC CALL ******/
void *super_realloc(void *ptr, size_t size) {
    void *newPointerRealloc = realloc(ptr, size);
    if (newPointerRealloc == NULL) {
        perror("malloc");
        return UNSUCCESS;
    }
    return newPointerRealloc;
}


/****** MAIN ******/
int main() {
    char readFile[MAX_INPUT_SIZE];
    char writeFile[MAX_INPUT_SIZE];
    struct countValue count;
    
    /*** HANDLING THE READ FILE ***/
    char *promptRead = "Please enter your file's name for processing: ";
    if(inputString(promptRead, readFile, MAX_INPUT_SIZE) == UNSUCCESS) {
        printf("fgets: an error or an end-of-file condition");
        return UNSUCCESS;
    };

    FILE *readfilePointer;
    
    // Check if file exists
    if(access(readFile, F_OK) != -1 ) {
        readfilePointer = fopen(readFile, "r");
        
        // Check if file can be opened for reading
        if (readfilePointer != NULL) {
            
            // Check if file has non-ascii
             if (checkASCII(readfilePointer) == SUCCESS) {
                rewind(readfilePointer);
                count = countChar(readfilePointer);
            } else {
                printf("Sorry, we can't proceed because the file contains non-ASCII characters.");
                return UNSUCCESS;
            }
            
            // Close the file
            fclose(readfilePointer);
        } else {perror(readFile);}
       
    } else {
        perror(readFile);
        return(UNSUCCESS);
    }
    
    
    /*** HANDLING THE WRITE FILE ***/
    char *promptWrite = "Please enter your file's name for storing result: ";
    if(inputString(promptWrite, writeFile, MAX_INPUT_SIZE) == UNSUCCESS) {
        printf("fgets: an error or an end-of-file condition");
        return UNSUCCESS;
    };
    
    FILE *writefilePointer;
    
    // Open the file if it exists, or create a new one if it doesn't exist
    writefilePointer = fopen(writeFile, "w");

    // Check if file can be opened for writing
    if (writefilePointer != NULL) {
        fprintf(writefilePointer, "Lines: %d%s Words: %d%s Characters: %d%s", count.lineCount,
                "\t", count.wordCount, "\t", count.charCount, "\n");

        // Close the file
        fclose(writefilePointer);
        
    } else {
        perror(writeFile);
        return UNSUCCESS;
    }
    return SUCCESS;
}




