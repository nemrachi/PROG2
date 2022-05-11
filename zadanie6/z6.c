#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/*
Dear tester,
my game was tested in these terminals:
    - VS Code Code Runner
    - VS Code cmd
    - Windows 11 Terminal cmd
    - www.replit.com
If you will use some other terminals, it is not guaranteed it will display it correctly.

My recommendation is to use www.replit.com
*/

typedef char bool;
#define true 1
#define false 0
// terminal color codes
#define RESET ""
#define GREY ";90"
#define GREEN ";32"
#define YELLOW ";33"
#define PURPLE ";35"
#define CYAN ";36"
// some constants
#define WORDS_NUM 10 // number of all words
#define WORD_LEN 5
#define MAX_ATTEMPTS 6

const char WORDS_DB[WORDS_NUM][WORD_LEN] = {"GECKO", "MANGO", "WOODS", "APPLE", "GRASS",
                                            "SNACK", "WINGS", "BACON", "WATER", "EARTH"};

char *getRandWord() {
    time_t t;
    srand((unsigned) time(&t)); // init random (time) seed for rand() func
    int dbI = rand()%WORDS_NUM; // get random index for word from WORDS_DB array
    char *randWord = malloc(WORD_LEN+1);

    memcpy(randWord, WORDS_DB[dbI], WORD_LEN);
    randWord[5] = '\0';

    return randWord;
}

void setConsoleColor(char *color) {
    printf("\033[0%sm", color);
}

void clearWord(char *str) {
    memset(str, '\0', WORD_LEN+1);
}

bool isWord(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (!isalpha(str[i])) { // if letter is NOT from [a-z] or [A-Z]
            return false;
        }
    }
    return true;
}

void strToUpper(char *str) { // all letters in word turns to upper
    for (int i = 0; i < WORD_LEN; i++) {
        str[i] = toupper(str[i]);
    }
}

int getCharIndex(char *str, char c) { // get index of first found char in word
    int i = -1;
    int len = strlen(str);
    while (len > i) {
        if (str[++i] == c) { return i; }
    }
    return -1;
}

bool isMatch(char *inWord, char *finalWord) {
    int countMatch = 0; // count matched letters
    char *tmpWord = malloc(WORD_LEN+1);
    strcpy(tmpWord, finalWord);

    for (int i = 0; i < WORD_LEN; i++) {
        // if some letter occure, delete it, so it will not count for next occurence of the same letter
        if (tmpWord[i] == inWord[i]) { // if letter is on right position
            tmpWord[i] = ' ';
            setConsoleColor(GREEN);
            countMatch++;
        } else if (strchr(tmpWord, inWord[i]) != NULL) { // if wrong position
            tmpWord[getCharIndex(tmpWord, inWord[i])] = ' ';
            setConsoleColor(YELLOW);
        } else { // if there is no such letter
            setConsoleColor(GREY);
        }
        printf("%c", inWord[i]);
        setConsoleColor(RESET);
    }

    free(tmpWord);
    return (countMatch == WORD_LEN) ? true : false;
}

int main() {
    bool match = false; // flag if user matched/guessed whole word
    int attempts = 0; // number of tries
    char *finalWord = getRandWord();
    char *inputWord = malloc(WORD_LEN+1);
    clearWord(inputWord);

    printf("--- RULES ---\n"
            "Guess the 5 letter word in 6 tries.\n"
            "Hit the Enter button to submit.\n"
            "After each guess, the color of the letters will change to show how close your guess was to the word.\n");
    setConsoleColor(GREEN);
    printf("GREEN");
    setConsoleColor(RESET);
    printf(" - indicates, the letter is in the correct spot.\n");
    setConsoleColor(YELLOW);
    printf("YELLOW");
    setConsoleColor(RESET);
    printf(" - indicates, the letter is in the word, but in the wrong spot.\n");
    setConsoleColor(GREY);
    printf("GREY");
    setConsoleColor(RESET);
    printf(" - indicates, the letter is not in the word.\n\n");

    while(attempts < MAX_ATTEMPTS && !match) {
        // player's input
        while (strlen(inputWord) < WORD_LEN) {
            scanf("%s", inputWord);
            getchar();
            printf("\033[1F"); // move to the start of the previous line
            printf("\033[2K"); // delete it
        }
        // check non alpha chars in word
        if (!isWord(inputWord)) {
            clearWord(inputWord);
            continue; 
        }
        // transform
        strToUpper(inputWord);
        // check matching
        match = isMatch(inputWord, finalWord);

        printf("\033[1E"); // new line
        clearWord(inputWord);
        attempts++;
    }
    
    if (match) {
        setConsoleColor(CYAN);
        printf("\n~~~ VICTORY ~~~\n");
    } else {
        setConsoleColor(PURPLE);
        printf("\n~~~ TRY NEXT TIME ~~~\n");
    }
    setConsoleColor(RESET);
    
    free(inputWord);
    free(finalWord);
    return 0;
}