#include <stdio.h>
const int N_MIN = 10;
const int N_MAX = 100;

//////////////////     NEMENIT !!!      /////////////////////
// source: https://prog2.dev/tester/xrichnakova/zadanie-2/#prng
//    Vo vasom rieseni vyuzite tieto tieto funkcie na
//    generovanie pseudo-nahodnych cisiel.
//
//    Funkcia srnd(seed):
//       * Zavolajte 1-krat na zaciatku programu na
//       inicializaciu generovanej postupnosti cisiel.
//
//    Funkcia rnd(from, to):
//       * Sluzi na vygenerovanie dalsieho nahodneho
//       cisla z intervalu <from,to>.

#define R_MAX 2147483647 // vsetky generovane cisla su mensie ako R_MAX
static long long unsigned int SEED = 0x1; // seed generatora
void srnd(int seed) { SEED = seed; }

int rnd(int from, int to) {
    SEED = SEED * 16807 % R_MAX;
    return from + (int) SEED % (to - from + 1);
}
/////////////////////////////////////////////////////////////

int isInputValid(int s, int t, int n) {
    return (s > 0 && (N_MIN <= n && n <= N_MAX) && (t*2) <= (int)(n/2));
}

int isEnterExitValid(int enter, int exit, int n) {
    return (enter > 0 && exit > 0 && enter < (n-1) && exit < (n-1) && enter != exit);
}

int isEnterExitDuplicate(int tunnels[][2], int new) {
    for (int i = 0; i < new; i++) {
        if (tunnels[i][0] == tunnels[new][0] || tunnels[i][1] == tunnels[new][1] ||
            tunnels[i][0] == tunnels[new][1] || tunnels[i][1] == tunnels[new][0]) { 
                return 1;
            }
    }
    return 0;
}

int throwDice() {
    return rnd(1, 6);
}

int max(int a, int b) {
    return (a >= b) ? a : b;
}

int isTunnelsEnter(int tunnels[][2], int t, int pos) {
    for (int i = 0; i < t; i++) {
       if (pos == tunnels[i][0]) {
           return tunnels[i][1]; // return position of exit
       }
    }
    return 0;
}

void swap(int *a, int *b) {
    int temp = *a;
    a = b;
    b = &temp;
}

void sortTunnels(int tunnels[][2], int t) {
    // bubble sort
    for (int i = 0; i < t; i++) {
        for (int j = i+1; j < t; j++) {
            if (tunnels[i][0] > tunnels[j][0]) {
                int tmp = tunnels[i][0];
                tunnels[i][0] = tunnels[j][0];
                tunnels[j][0] = tmp;
                tmp = tunnels[i][1];
                tunnels[i][1] = tunnels[j][1];
                tunnels[j][1] = tmp;
            }
        }
    }
}

int main() {
    int s; // init value for rand num generator
    int t; // num of tunnels
    int n; // size of racing track

    scanf("%d %d %d", &s, &t, &n);
    if (!isInputValid(s, t, n)) { return 1; }
    srnd(s); // init randomizer

    int racingTrack[n]; // attendance counter
    for (int i = 0; i < n; i++) { racingTrack[i]=0; }
    
    int tunnels[t][2];

    for (int i = 0; i < t; i++) {
        scanf("%d %d", &tunnels[i][0], &tunnels[i][1]);
        if (!isEnterExitValid(tunnels[i][0], tunnels[i][1], n) || 
            isEnterExitDuplicate(tunnels, i)) { return 2; }
    }
    sortTunnels(tunnels, t);
    printf("TUNNELS: ");
    for (int i = 0; i < t; i++) {
        printf("[%d,%d] ", tunnels[i][0], tunnels[i][1]);
    }
    printf("\n");

    int h = 1; // player that play this turn
    int r1, r2; // 2 dices
    int player[2] = {-1, -1}; // i=0->p1, i=1->p2 // current positions
    int playerBefore[2] = {-1, -1}; // positions before turn
    int tunnelExitPos; // position of exit of entered tunnel
    int round = 1;

    while(1) { // race!
        h = (h + 1) % 2; // get number of opposite player
        r1 = throwDice();
        r2 = throwDice();

        printf("[%d,%d] [%d] [%d,%d] ", round++, h+1, player[h], r1, r2);

        if (player[h] == -1) { // player not on field
            if (r1+r2 > 7) {
                player[h] = r1+r2-7;
            } else {
                printf("[%d] \n", player[h]);
                continue;
            }
        } else { // player on field
            // special 1, special 2
            if (((playerBefore[h] < playerBefore[(h+1)%2] && (r1 + r2) == 12) ||
                (playerBefore[h] > playerBefore[(h+1)%2] && (r1 + r2) == 2)) &&
                (playerBefore[h] != -1 && playerBefore[(h+1)%2] != -1)) {
                    int tmp = player[h];
                    player[h] = player[(h+1)%2];
                    player[(h+1)%2] = tmp;

                    printf("[%d] S \n", player[h]);

                    // attendace counter
                    racingTrack[player[h]] += 1;
                    racingTrack[player[(h+1)%2]] += 1;
                    continue;
            } else {
                // normal
                player[h] += max(r1, r2);
            }
        }
        // tunnels
        tunnelExitPos = isTunnelsEnter(tunnels, t, player[h]);
        player[h] = (tunnelExitPos) ? tunnelExitPos : player[h];

        printf("[%d] ", player[h]);

        if (tunnelExitPos) { printf("T "); }
        // elimination
        if (player[h] == player[(h+1)%2]) {
            player[(h+1)%2] = -1;
            playerBefore[(h+1)%2] = -1;
            printf("E ");
        }

        printf("\n");

        // finish
        if (player[h] >= n) {
            printf("WINNER: %d\n", h+1);
            break;
        }
        playerBefore[h] = player[h];
        // attendace counter
        racingTrack[player[h]] += 1;
    }

    printf("VISITS: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", racingTrack[i]);
    }    

    return 0;
}