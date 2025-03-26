#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "magic.h"

#define TEST_SIZE 1000000

long long current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000;
}

int main() {
    srand(time(NULL));

    long long start = current_time_ms();

    MAGIC m = MAGICinit();

    printf("Performing %d random add/remove operations...\n", TEST_SIZE);

    // Mélange d'ajouts et suppressions aléatoires
    for (int i = 0; i < TEST_SIZE; i++) {
        int pos = rand() % (TEST_SIZE / 2);
        int len = (rand() % 5) + 1;
        if (rand() % 2 == 0) {
            MAGICadd(m, pos, len);
        } else {
            MAGICremove(m, pos, len);
        }
    }

    printf("Mapping input → output for selected positions:\n");
    for (int i = 0; i <= 100; i += 10) {
        int out = MAGICmap(m, STREAM_IN_OUT, i);
        printf("IN %3d to OUT %3d\n", i, out);
    }

    printf("\nMapping output → input for selected positions:\n");
    for (int i = 0; i <= 100; i += 10) {
        int in = MAGICmap(m, STREAM_OUT_IN, i);
        printf("OUT %3d to IN %3d\n", i, in);
    }

    MAGICdestroy(m);

    long long end = current_time_ms();
    printf("Execution time: %lld ms\n", end - start);

    return 0;
}
