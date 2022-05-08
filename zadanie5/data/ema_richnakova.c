#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <float.h>
#include "config.h"

/* 
  gcc -Wall -Wextra -lm functions.c ema_richnakova.c -o ema_richnakova && ./ema_richnakova > ./out.txt 2> ./err.txt
*/

typedef char bool;
#define true 1
#define false 0

char **splitStr(char *line, int n) { // split string to n words
    int j = 0, word = 0;
    char **lineWords = malloc(n*sizeof(char*));
    for (int i = 0; i < n; i++) {
      lineWords[i] = malloc(sizeof(char));
    }
  
    for (int i = 0; i < (int)strlen(line); i++) {
        if (line[i] == ' ') {
            lineWords[word++][j] = '\0';
            j = 0;
        } else {
            lineWords[word][j++] = line[i];
        }
    }

  return lineWords;
}

void getWarehouses(WAREHOUSE **db, int *dbNum) {
    FILE *warehouseFp = fopen(WAREHOUSE_DB_FILE, "r");
    if (warehouseFp == NULL) { exit(1); }
    int lineLen = MAX_NAME+20;
    char line[lineLen];
    int k = 0;
    char **lineWords = NULL;

    while (fgets(line, lineLen, warehouseFp)) { // count warehouses
        (*dbNum)++;
    }
    rewind(warehouseFp);
    
    *db = malloc((*dbNum)*sizeof(WAREHOUSE));

    while (fgets(line, lineLen, warehouseFp)) { // get data
        lineWords = splitStr(line, 4);

        strcpy((*db)[k].name, lineWords[0]);
        (*db)[k].gps.lat = strtod(lineWords[1], NULL);
        (*db)[k].gps.lon = strtod(lineWords[2], NULL);
        (*db)[k].capacity = atoi(lineWords[3]);
        k++;
    }

    fclose(warehouseFp);
}

char *getFolder() {
    char *folder = malloc(9*sizeof(char));
    sprintf(folder, ".%s%s%s", PATH_SEPARATOR, ITEMS_FOLDER, PATH_SEPARATOR);
    return folder;
}

void deleteElement(WAREHOUSE ***db, int **dbNum, int pos) {
    (**dbNum)--;
    for (int i = pos; i < (**dbNum); i++) {
        (**db)[i] = (**db)[i+1];
    }
}

bool isItemFormatCorrect(char **lineWords, char line[MAX_NAME]) {
    return (strchr(line, ' ') != NULL && atoi(lineWords[0]) == 0 && atoi(lineWords[1]) != 0) ? true : false;
}

void getItems(WAREHOUSE **db, int *dbNum) {
    FILE *itemsFile;
    char *folder = getFolder();
    char fileName[MAX_NAME+5] = "";
    char line[MAX_NAME];
    int k = 0;
    char **lineWords = NULL;
    int itemsNum;

    for (int i = 0; i < (*dbNum); i++) {
        itemsNum = k = 0;
        sprintf(fileName, "%s%s.txt", folder, (*db)[i].name);
      
        itemsFile = fopen(fileName, "r");
        if (itemsFile == NULL) { // if file doesn't exist
            // error situation 1 - warehouse has no items txt file
            fprintf(stderr, "%s %s.txt\n", "FILE_ERROR", (*db)[i].name);
            deleteElement(&db, &dbNum, i--);
            continue;
        }
      
        while (fgets(line, MAX_NAME, itemsFile)) { // count items
            itemsNum++;
        }

        if (itemsNum > (*db)[i].capacity) {
            // error situation 2 - num of items in file > warehouse capacity
            fprintf(stderr, "%s %s.txt\n", "CAPACITY_ERROR", (*db)[i].name);
            deleteElement(&db, &dbNum, i--);
            fclose(itemsFile);
            continue;
        }
      
        (*db)[i].n = itemsNum;
        (*db)[i].items = malloc(itemsNum*sizeof(ITEM));
      
        rewind(itemsFile);

        while (fgets(line, MAX_NAME, itemsFile)) { // get items data
          lineWords = splitStr(line, 2);
          // error situation 3 - wrong item file format
          if (!isItemFormatCorrect(lineWords, line)) {
              fprintf(stderr, "%s %s.txt\n", "FORMAT_ERROR", (*db)[i].name);
              free((*db)[i].items);
              deleteElement(&db, &dbNum, i--);
              break;
          }
          strcpy((*db)[i].items[k].name, lineWords[0]);
          (*db)[i].items[k].price = atoi(lineWords[1]);
          k++;
        }

        fclose(itemsFile);
    }
}

void filterByWarehouseName(WAREHOUSE **db, int *dbNum, char *name) {
    for (int i = 0; i < (*dbNum); i++) {
        if (!strcmp((*db)[i].name, name)) {
            (*dbNum) = 1;
            WAREHOUSE *tmp = malloc(sizeof(WAREHOUSE));
            *tmp = (*db)[i];
            free(*db);
            (*db) = tmp;
            break;
        }
    }
}

void filterByClosestGPS(WAREHOUSE **db, int *dbNum, char *strLat, char *strLon) {
    float lat = atof(strLat);
    float lon = atof(strLon);
    int closestIndex = 0;
    double closestDist = DBL_MAX;
    double possibleClosestDist;

    for (int i = 0; i < (*dbNum); i++) {
        possibleClosestDist = distance((*db)[i].gps, (GPS){lat, lon});
        if (possibleClosestDist  <  closestDist) {
            closestDist = possibleClosestDist;
            closestIndex = i;
        }
    }

    (*dbNum) = 1;
    WAREHOUSE *tmp = malloc(sizeof(WAREHOUSE));
    *tmp = (*db)[closestIndex];
    free(*db);
    (*db) = tmp;
}

void sortItems(WAREHOUSE **db, int dbNum, bool sortByName, bool sortByPriceAsc) {
    // source: https://www.geeksforgeeks.org/c-program-for-bubble-sort/
    int n;
    ITEM *temp = malloc(sizeof(ITEM));
    for (int k = 0; k < dbNum; k++) {
        n = (*db)[k].n;
        for (int i = 0; i < n-1; i++) {
            for (int j = 0; j < n-1-i; j++) {
                if ((sortByName && strcmp((*db)[k].items[j].name, (*db)[k].items[j+1].name) > 0) ||
                    (!sortByName && sortByPriceAsc && (*db)[k].items[j].price > (*db)[k].items[j+1].price) ||
                    (!sortByName && !sortByPriceAsc && (*db)[k].items[j].price < (*db)[k].items[j+1].price)) {
                    *temp = (*db)[k].items[j];
                    (*db)[k].items[j] = (*db)[k].items[j+1];
                    (*db)[k].items[j+1] = *temp;
                }
            }
        }
    }
    free(temp);
}

void print(WAREHOUSE *db, int dbNum, bool items) {
    for (int i = 0; i < dbNum; i++) {
      printf("%s %.03lf %.03lf %d", db[i].name, db[i].gps.lat, db[i].gps.lon, db[i].capacity);
      if (items) {
        printf(" :\n");
        for (int j = 0; j < db[i].n; j++) {
          printf("%d. %s %d\n", j+1, db[i].items[j].name, db[i].items[j].price);
        }
      } else { printf("\n"); }
    }
}

int main(int argc, char *argv[]){
    int appOpt; // options from terminal
	char *optstring = ":w:n:e:ad";
    char *lat = NULL, *lon = NULL;
    bool opta = false, optd = false;
    WAREHOUSE *db;
    int dbNum = 0;

    getWarehouses(&db, &dbNum); // count warehouses and get their data
    print(db, dbNum, false);

    while ((appOpt = getopt(argc, argv, optstring)) != -1) { // get options
        switch (appOpt) {
            // warehouse filter - default: all warehouses
            case 'w': // warehouse name arg (case-sensitive)
                filterByWarehouseName(&db, &dbNum, optarg);
                break;
            case 'n': // warehouse lat(itude) arg
                lat = optarg;
              	break;
            case 'e': // warehouse lon(gitude) arg
                lon = optarg;
                break;
            // warehouse sort - default: items sort by name asc
            case 'a': // items sort by price asc
                opta = true;
                break;
            case 'd': // items sort by price desc
                optd = true;
                break;
            case '?': // error: invalid option
                return 1;
            case ':': // error: no parameter(arg) after option
                return 2;
            default:
                break;
        }
    }
    if (lat != NULL && lon != NULL) {
        filterByClosestGPS(&db, &dbNum, lat, lon);
    }
  
    getItems(&db, &dbNum); // get items from item files
    if (opta) {
        sortItems(&db, dbNum, false, opta);
    } else if (optd) {
        sortItems(&db, dbNum, false, !optd);
    } else {
        sortItems(&db, dbNum, true, false);
    }
  
    print(db, dbNum, true);

    for (int i = 0; i < dbNum; i++) {
        free(db[i].items);
    }
    free(db);

    return 0; 
}