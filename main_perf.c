#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "magic.h"

// Number of operations for performance test
#define N 1000000  

int main()
{
    MAGIC m = MAGICinit();
    clock_t start, end;
    double cpu_time;

    // === TEST: MAGICadd ===
    start = clock();
    for (int i = 0; i < N; ++i) 
    {
        MAGICadd(m, i, 1);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("MAGICadd #%d: %.3f sec\n", N, cpu_time);

    // === TEST: MAGICremove ===
    start = clock();
    for (int i = 0; i < N; ++i) 
    {
        MAGICremove(m, i, 1);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("MAGICremove #%d: %.3f sec\n", N, cpu_time);

    // === TEST: MAGICmap IN → OUT ===
    start = clock();
    for (int i = 0; i < N; ++i) 
    {
        (void)MAGICmap(m, STREAM_IN_OUT, i);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("MAGICmap(IN->OUT) #%d: %.3f sec\n", N, cpu_time);

    // === TEST: MAGICmap OUT → IN ===
    start = clock();
    for (int i = 0; i < N; ++i) 
    {
        (void)MAGICmap(m, STREAM_OUT_IN, i);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("MAGICmap(OUT->IN) #%d: %.3f sec\n", N, cpu_time);

    MAGICdestroy(m);
    return 0;
}
