#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include "config.h"

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
}

bool isStrNumber(double d, char* str) {
    return (d == 0 && !isdigit(str[0])) ? false : true;
}

int getGPSClosestIndex(char* strLat, char* strLon) {
    float lat = atof(strLat);
    float lon = atof(strLon);
    int closestIndex = 0;
    double closestDist = DBL_MAX;
    double possibleClosestDist;

    for (int i = 0; i < DB_NUM; i++) {
        possibleClosestDist = distance(db[i].gps, (GPS){lat, lon});
        if (possibleClosestDist  <  closestDist) {
            closestDist = possibleClosestDist;
            closestIndex = i;
        }
    }
    
    return closestIndex;
}

void printDB(int *filteredIndexes, int size, bool itemPrint, OPT optT, OPT optP) {
    int count = 1;
    if (itemPrint) {
        for (int i = 0; i < size; i++) {
            int fi = filteredIndexes[i];
            for (int j = 0; j < db[fi].n; j++) {
                if (optT.flag) {
                    if (!strcmp(db[fi].items[j].name, optT.arg)) {
                        printf("%d. %s %d : %s %.03lf %.03lf %d\n", count++, db[fi].items[j].name, db[fi].items[j].price, db[fi].name, db[fi].gps.lat, db[fi].gps.lon, db[fi].n);
                    }
                    continue;
                } else if (optP.flag) {
                    if (db[fi].items[j].price <= atoi(optP.arg)) {
                        printf("%d. %s %d : %s %.03lf %.03lf %d\n", count++, db[fi].items[j].name, db[fi].items[j].price, db[fi].name, db[fi].gps.lat, db[fi].gps.lon, db[fi].n);
                    }
                    continue;
                }
                printf("%d. %s %d : %s %.03lf %.03lf %d\n", count++, db[fi].items[j].name, db[fi].items[j].price, db[fi].name, db[fi].gps.lat, db[fi].gps.lon, db[fi].n);
            }
        }
    } else {
        for (int i = 0; i < size; i++) {
            count = 1;
            int fi = filteredIndexes[i];
            printf("%s %.03lf %.03lf %d :\n", db[fi].name, db[fi].gps.lat, db[fi].gps.lon, db[fi].n);
            for (int j = 0; j < db[fi].n; j++) {
                if (optT.flag) {
                    if (!strcmp(db[fi].items[j].name, optT.arg)) {
                        printf("%d. %s %d\n", count++, db[fi].items[j].name, db[fi].items[j].price);
                    }
                    continue;
                } else if (optP.flag) {
                    if (db[fi].items[j].price <= atoi(optP.arg)) {
                        printf("%d. %s %d\n", count++, db[fi].items[j].name, db[fi].items[j].price);
                    }
                    continue;
                }
                printf("%d. %s %d\n", count++, db[fi].items[j].name, db[fi].items[j].price);
            }
        }
    }
}

int main(int argc, char *argv[]) {
	int appOpt; // options from terminal
	char *optstring = ":w:i:n:e:t:p:W";
    OPT optSmallW = setOptDefault();
    OPT optI= setOptDefault();
    OPT optN = setOptDefault();
    OPT optE = setOptDefault();
    OPT optT = setOptDefault();
    OPT optP = setOptDefault();
    OPT optW = setOptDefault();
    int filteredIndexes[DB_NUM] = {0};
    int filteredNum = 0;

    while ((appOpt = getopt(argc, argv, optstring)) != -1) { // get options
        switch (appOpt) {
            case 'w': // warehouse name arg (case-sensitive)
                optSmallW = setOpt(true, getOptArgument(appOpt, argc, argv));
                break;
            case 'i': // warehouse item_name arg (case-sensitive)
                optI = setOpt(true, getOptArgument(appOpt, argc, argv));
                break;
            case 'n': // warehouse lat(itude)
            	optN = setOpt(true, getOptArgument(appOpt, argc, argv));
            	break;
            case 'e': // warehouse lon(gitude)
                optE = setOpt(true, getOptArgument(appOpt, argc, argv));
                break;
            case 't': // item_name
                optT = setOpt(true, getOptArgument(appOpt, argc, argv));
                break;
            case 'p': // item max_price
            	optP = setOpt(true, getOptArgument(appOpt, argc, argv));
            	break;
            case 'W': // sklado-centricky vypis
            	optW = setOpt(true, getOptArgument(appOpt, argc, argv));
            	break;
            case '?':
                return 1; // situation 1 - invalid option
            case ':':
                return 2; // situation 2 - no parameter(arg) after option
            default: // without opt - all warehouses // tovaro-centricky vypis
                break;
        }
    }
    // other error situations
    if ((optN.flag != optE.flag)) {
        return 3; // situation 3 - -n and -e must be entered together
    }
    if (optN.flag && optE.flag) {
        double lat = strtod(optN.arg, NULL);
        double lon = strtod(optE.arg, NULL);
        if (!(isStrNumber(lat, optN.arg) && isStrNumber(lon, optE.arg) &&
            (lat <= 90 || lat >= -90) && (lon <= 180 || lon >= -180))) {
            return 4; // situation 4 - latitude is not in range <-90,90> or longitude is not in range <-180,180> or they are not a number
        }
    }

    // filter warehouses by "" or -w or -i or (-n and -e)
    if (optSmallW.flag) {
        int j = 0;
        for (int i = 0; i < DB_NUM; i++) {
            if (!strcmp(db[i].name, optSmallW.arg)) {
                filteredIndexes[j++] = i;
            }
        }
        filteredNum = j;
    } else if (optI.flag) {
        int k = 0;
        for (int i = 0; i < DB_NUM; i++) {
            for (int j = 0; j < db[i].n; j++) {
                if (!strcmp(db[i].items[j].name, optI.arg)) {
                    filteredIndexes[k++] = i;
                    break;
                }
            }
        }
        filteredNum = k;
    } else if (optN.flag && optE.flag) {
        filteredIndexes[0] = getGPSClosestIndex(optN.arg, optE.arg);
        filteredNum = 1;
    } else {
        for (int i = 0; i < DB_NUM; i++) {
            filteredIndexes[i] = i;
        }
        filteredNum = DB_NUM;
    }

    printDB(filteredIndexes, filteredNum, !optW.flag, optT, optP);

	return 0;
}