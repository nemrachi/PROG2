#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_STRLEN 1001
// isprint(c)
// isalpha(c)
// tolower(c)
// toupper(c)
// getchar(), putchar()
// %s %c
// strcpy, strncpy, memmove
// strcmp
// sscanf

// argv[0] = path to the program
// argv[1..argc-1] = arguments

// options (prepinace) -x (option) abc
// non-option argument = after everything other

bool isLetterOrSpace(char c) {
    return (('A' <= c && c <= 'Z') || (c == ' ') ||
            ('a' <= c && c <= 'z')) ? true : false;
}

void removeLetters(char *str, bool dFlag, bool pFlag) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (!isLetterOrSpace(str[i])) {
            if ((dFlag && isdigit(str[i])) ||
            (pFlag && ispunct(str[i]))) { continue; }
            strcpy(&str[i], &str[i+1]);
            len--;
            i--;
        }
    }
}

void uFunction(char *str) {

}

void lFunction() {

}

void rFunction() {

}

void RFunction() {

}

int main(int argc, char *argv[]) {
	// Poznamky ku 'getopt':

	// Retazec 'optstring' specifikuje platne prepinace programu.

	// Ak funkcia 'getopt' narazi na neplatny prepinac, vrati znak '?'.
	// Ak 'optstring' zacina znakom ':', vtedy 'getopt' vrati znak ':'
	// v pripade chybajuceho povinneho parametra prepinaca.

	// Ak je v 'optstringu' za pismenom prepinaca znak ':', znamena to,
	// ze dany prepinac ma povinny parameter.

	// Povinny parameter aktualneho prepinaca vieme ziskat cez globalny
	// smernik 'optarg'.

	// Ked su rozpoznane vsetky prepinace, funkcia 'getopt' vrati -1.
	// Ak zostali nejake non-option argumenty, tak prvy non-option argument
	// ma index dany globalnou premennou 'optind'.

	int opt;
	char* optstring = ":dpulr:R:";
    bool dFlag = 0, pFlag = 0; // can combine
    bool uFlaf = 0, lFlag = 0; // cannot combine
    bool rFlag = 0, RFlag = 0; // cannot combine
    char str[MAX_STRLEN];

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'd':
                dFlag = 1;
                break;
            case 'p':
                pFlag = 1;
                break;
            case 'u': // to upper
            	uFlaf = 1;
            	break;
            case 'l': // to lower
                lFlag = 1;
                break;
            case 'r': // case-sensitive replace
                rFlag = 1;
                break;
            case 'R': // case-insensitive replace
            	RFlag = 1;
            	break;
            case '?':
                return 1; // situation 1
            case ':':
                return 2; // situation 2
            default:
                printf("Some another error\n");
                return 3;
        }
    }

    if(optind < argc){
    	for(int i=optind;i<argc;i++){
            if (argv[i][0] == '-') {
                return 1;
            }
    		printf("Non-option CMD argument: %s\n",argv[i]);
    	}
    }

    while (1) {
        fgets(str, MAX_STRLEN, stdin);
        if (!strcmp(str, "\n")) {
            return 0;
        }

        // default, -d or -f input functionality
        removeLetters(&str, dFlag, pFlag);

        if (uFlaf) {

        } else if (lFlag) {

        }

        if (rFlag) {

        } else if (RFlag) {

        }

        printf("%s", str);
    }

	return 0;
}