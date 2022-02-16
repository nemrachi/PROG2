#include <stdio.h>
#include <stdbool.h>

bool isPrimeNum(int num) {
    for (int i = 2; i < num; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

void primeFactors(int a, int m) {
    if (!(a > 1 && m >= 0)) {
        printf("ERROR");
        return;
    }

    int count = 0;
    for (int i = 2; i <= a; i++) {
        if (count == m) {
            return;
        }
        if (a % i == 0) {
            if (isPrimeNum(i)) {
                printf("%d\n", i);
                count += 1;
            }
        }      
    }
}

int main() {
    int a, m;
    scanf("%d %d", &a, &m);

    primeFactors(a, m);

    return 0;
}