#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"

// gcc -Wall -Wextra ema_richnakova.c -o ema_richnakova && ./ema_richnakova

typedef char bool;
#define true 1
#define false 0

#define EMPTY_STR ""

typedef struct opt {
    bool flag; // if opt was enetered
    char* arg; // argument to the option
} OPT;

OPT setOptDefault() {
    return (OPT) {false, NULL};
}

OPT setOpt(bool b, char* str) {
    return (OPT) {b, str};
}

char* getOptArgument(int appOpt, int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == appOpt) {
            return argv[i+1];
        }
    }
    return NULL;
}

int getWarehouses(WAREHOUSE **db) {
    FILE *warehouseFp = fopen(WAREHOUSE_DB_FILE, "r");
    if (warehouseFp == NULL) { exit(1); }
    int lineLen = MAX_NAME+20;
    char line[lineLen];
    int j = 0, word = 0, k = 0;
    char lineWords[4][MAX_NAME];
    int dbNum = 0;

    while (fgets(line, lineLen, warehouseFp)) { // count warehouses
        dbNum++;
    }
    *db = malloc(dbNum*sizeof(WAREHOUSE));

    rewind(warehouseFp);

    while (fgets(line, lineLen, warehouseFp)) { // get data
        word = j = 0;

        for (int i = 0; i < (int)strlen(line); i++) {
            if (line[i] == ' ') {
                lineWords[word][j] = '\0';
                word++;
                j = 0;
            } else {
                lineWords[word][j] = line[i];
                j++;
            }
        }
        strcpy((*db)[k].name, lineWords[0]);
        (*db)[k].gps.lat = strtod(lineWords[1], NULL);
        (*db)[k].gps.lon = strtod(lineWords[2], NULL);
        (*db)[k].capacity = atoi(lineWords[3]);
        k++;
    }
    fclose(warehouseFp);

    return dbNum;
}

char *getFolder() {
    char *folder = malloc(9*sizeof(char));
    strcpy(folder, ".");
    strcat(folder, PATH_SEPARATOR);
    strcat(folder, ITEMS_FOLDER);
    strcat(folder, PATH_SEPARATOR);
    return folder;
}

void getItems(WAREHOUSE **db, int dbNum) {
    FILE *itemFile;
    char *folder = getFolder();
    char fileName[MAX_NAME+5] = "";
    strcpy(fileName, folder);

    for (int i = 0; i < dbNum; i++) {
        strcat(fileName, (*db)[i].name);
        strcat(fileName, ".txt");
        itemFile = fopen(fileName, "r");
        if (itemFile == NULL) { // if file doesn't exist
            fprintf(stderr, "%s %s\n", "FILE_ERROR", fileName);
            strcpy((*db)[i].name, EMPTY_STR);
        } else {
            fclose(itemFile);
        }
        strcpy(fileName, folder);
    }
}

void printWarehouses(WAREHOUSE *db, int dbNum) {
    for (int i = 0; i < dbNum; i++) {
      if (strcmp(db[i].name, EMPTY_STR) == 0) { continue; }
        printf("%s %.03lf %.03lf %d\n", db[i].name, db[i].gps.lat, db[i].gps.lon, db[i].capacity);
    }
}

int main(int argc, char *argv[]){
    int appOpt; // options from terminal
	char *optstring = ":w:n:e:ad";
    OPT optw = setOptDefault();
    OPT optn = setOptDefault();
    OPT opte = setOptDefault();
    bool opta = false;
    bool optd = false;
    WAREHOUSE *db;
    int dbNum = 0;

    while ((appOpt = getopt(argc, argv, optstring)) != -1) { // get options
        switch (appOpt) {
            // warehouse filter - default: all warehouses
            case 'w': // warehouse name arg (case-sensitive)
                optw = setOpt(true, getOptArgument(appOpt, argc, argv));
                break;
            case 'n': // warehouse lat(itude) arg
            	optn = setOpt(true, getOptArgument(appOpt, argc, argv));
            	break;
            case 'e': // warehouse lon(gitude) arg
                opte = setOpt(true, getOptArgument(appOpt, argc, argv));
                break;
            // warehouse sort - default: warehouse sort by name asc
            case 'a': // items sort by price asc
                opta = true;
                break;
            case 'd': // items sort by price desc
                optd = true;
                break;
            case '?':
                return 1; // error: invalid option
            case ':':
                return 2; // error: no parameter(arg) after option
            default:
                break;
        }
    }

    dbNum = getWarehouses(&db); // get warehouses data and number of them
    // error situation 1 - warehouse has no items txt file, no return, ignore missing warehouse;
    getItems(&db, dbNum);

    // error situation 2 - num of items in file > capacity, no return, ignore warehouse
    // TODO function fprintf(stderr, "CAPACITY_ERROR name.txt\n", MAX_NAME+23);

    // error situation 3 - wrong item file format, no return, ignore warehouse
    // TODO function fprintf(stderr, "FORMAT_ERROR name.txt\n", MAX_NAME+23);
    printWarehouses(db, dbNum);

    free(db);

    return 0; 
}