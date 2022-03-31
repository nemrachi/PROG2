#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRLEN 1001

bool isLetterOrSpace(char c) {
    return (isalpha(c) || c == ' ') ? true : false;
}

// if dFlag == true, keep digits; if pFlag == true, keep punctations
// default - keep letters and spaces
void removeChars(char *str, bool dFlag, bool pFlag) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (!isLetterOrSpace(str[i])) {
            if ((dFlag && isdigit(str[i])) || (pFlag && ispunct(str[i]))) { 
                continue;
            }
            memmove(&str[i], &str[i+1], strlen(str) + 1);
            len--;
            i--;
        }
    }
}

void uFunction(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
}

void lFunction(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
}

// chek if in input string is words from non option arguments from terminal
bool isMatch(char *str, int i, char nonOptArgs[][100], int nonOptArgNum, bool caseSensitive) {
    if (!nonOptArgNum) { return true; }
    bool match;
    for (int k = 0; k < nonOptArgNum; k++) {
        for (int j = 0; j < strlen(nonOptArgs[k]); j++) {
            if (caseSensitive) {
                if (str[i+j] != nonOptArgs[k][j]) {
                    match = false;
                    break;
                }
            } else {
                if (tolower(str[i+j]) != tolower(nonOptArgs[k][j])) {
                    match = false;
                    break;
                }
            }
            match = true;
        }
        if (match) { return match; }        
    }
    return match;
}

// replace nonOptArgs match with replaceStr
// if no nonOptArgs, replace all alphanumeric characters with replaceStr
void rFunction(char *str, char *replaceStr, char nonOptArgs[][100], int nonOptArgNum, bool caseSensitive) {
    for (int i = 0; i < strlen(str); i++) {
        if (isalnum(str[i]) && isMatch(str, i, nonOptArgs, nonOptArgNum, caseSensitive)) {
            if (i != 0 && isalnum(str[i-1])) { continue; }
            int j = 0;
            while (1) {
                if (j < strlen(replaceStr) && isalnum(str[i+j])) {
                    if (caseSensitive) {
                        str[i+j] = replaceStr[j];
                    } else {
                        if (isdigit(str[i+j])) {
                            str[i+j] = replaceStr[j];
                        } else if (islower(str[i+j])) {
                            str[i+j] = tolower(replaceStr[j]);
                        } else {
                            str[i+j] = toupper(replaceStr[j]);
                        }
                    }
                } else if (!isalnum(str[i+j])){
                    i += j;
                    j = 0;
                    break;
                }
                j++;
            }
        }
    }
}

int main(int argc, char *argv[]) {
	int opt; // options from terminal
	char *optstring = ":dpulr:R:";
    bool dFlag = false, pFlag = false;
    bool uFlaf = false, lFlag = false;
    bool rFlag = false, RFlag = false;
    char str[MAX_STRLEN]; // string to process
    char *strPtr;
    char replaceStr[MAX_STRLEN]; // parameter after -r/-R
    
    while ((opt = getopt(argc, argv, optstring)) != -1) { // get options
        switch (opt) {
            case 'd': // with numbers
                dFlag = true;
                break;
            case 'p': // with punctuation
                pFlag = true;
                break;
            case 'u': // to upper
            	uFlaf = true;
            	break;
            case 'l': // to lower
                lFlag = true;
                break;
            case 'r': // case-sensitive replace
                rFlag = true;
                break;
            case 'R': // case-insensitive replace
            	RFlag = true;
            	break;
            case '?':
                return 1; // situation 1 - invalid option
            case ':':
                return 2; // situation 2 - no parameter after -r/-R
            default:
                printf("Some error -> %c\n", opt);
                return 3;
        }
    }

    // args for -r/-R
    int nonOptArgNum = argc - optind;
    char nonOptArgs[nonOptArgNum][100];
    if(nonOptArgNum) {
        int j = 0;
    	for(int i = optind; i < argc; i++){
            if (argv[i][0] == '-') { return 1; }
            strcpy(nonOptArgs[j++], argv[i]);
    	}
    }
    if (rFlag || RFlag) {
        strcpy(replaceStr, argv[optind-1]);
    }

    while (fgets(str, MAX_STRLEN, stdin) != NULL) {
        if (!strcmp(str, "\n")) { return 0; }
        strPtr = str;

        // default, -d or -p input functionality
        removeChars(strPtr, dFlag, pFlag);

        if (uFlaf) {
            uFunction(strPtr);
        } else if (lFlag) {
            lFunction(strPtr);
        }

        if (rFlag) {
            rFunction(strPtr, replaceStr, nonOptArgs, nonOptArgNum, true);
        } else if (RFlag) {
            rFunction(strPtr, replaceStr, nonOptArgs, nonOptArgNum, false);
        }

        printf("%s\n", str);
    }

	return 0;
}