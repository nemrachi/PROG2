#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

typedef char bool;
#define true 1
#define false 0

// terminal colors
#define WHITE ""
#define RED ";31"
#define GREEN ";32"
#define BLUE ";34"
#define YELLOW ";33"
#define PURPLE ";35"

char wordsDb[10][5] = {"GECKO", "MANGO", "WOODS", "APPLE", "GRASS",
                        "SNACK", "WING", "BACON", "WATER", "EARTH"};

char *getRandWord() {
    time_t t;
    srand ((unsigned) time(&t));
    int dbI = rand()%10;
    char *randWord = malloc(6);

    memcpy(randWord, wordsDb[dbI], 5);
    randWord[5] = '\0';

    return randWord;
}

void setConsoleColor(char *color) {
    printf("\033[0%sm", color); // set color of console text
}

void toUpper(char *str) {
    for (int i = 0; i < 5; i++) {
        str[i] = toupper(str[i]);
    }
}

int getCharIndex(char *str, char c) {
    int i = -1;
    int len = strlen(str);
    while (len > i) {
        if (str[++i] == c) { return i; }
    }
    return -1;
}

bool isMatch(char *inWord, char *finalWord) {
    int countMatch = 0;
    char *tmpWord = malloc(6);
    strcpy(tmpWord, finalWord);

    for (int i = 0; i < 5; i++) {
        if (finalWord[i] == inWord[i]) {
            tmpWord[i] = ' ';
            setConsoleColor(GREEN);
            countMatch++;
        } else if (strchr(tmpWord, inWord[i]) != NULL) {
            tmpWord[getCharIndex(tmpWord, inWord[i])] = ' ';
            setConsoleColor(YELLOW);
        }
        printf("%c", inWord[i]);
        setConsoleColor(WHITE);
    }

    free(tmpWord);
    return (countMatch == 5) ? true : false;
}

int main() {
    bool match = false;
    int attempts = 0;
    char *finalWord = getRandWord();
    char *inputWord = malloc(6);
    memset(inputWord, '\0', 6);

    printf("%s\n", finalWord);

    while(attempts++ < 6 && !match) {
        while (strlen(inputWord) < 5) {
            scanf("%s", inputWord);
            getchar();
            printf("\033[1F");
            printf("\033[2K");
        }
        toUpper(inputWord);
        match = isMatch(inputWord, finalWord);
        printf("\033[1E");
        memset(inputWord, '\0', 6);
    }
    
    if (match) {
        printf("UwU VICTORY UwU");
    } else {
        printf("-w- TRY NEXT TIME -w-");
    }
    
    free(inputWord);
    free(finalWord);
    //system("cls");
    return 0;
}