#include <stdio.h>
#include <assert.h>
#include "magic.h"
#include <stdlib.h>
#include <time.h>

int main() 
{

    //===================================================
    //================= IN -> OUT TESTS =================
    //===================================================
    printf("==== IN -> OUT TESTS ====\n");

    // TEST 1 : ONLY REMOVE
    MAGIC m = MAGICinit();
    MAGICremove(m, 0, 2);
    assert(MAGICmap(m, STREAM_IN_OUT, 0) == -1);
    assert(MAGICmap(m, STREAM_IN_OUT, 1) == -1);
    assert(MAGICmap(m, STREAM_IN_OUT, 2) == 0);
    assert(MAGICmap(m, STREAM_IN_OUT, 3) == 1);
    assert(MAGICmap(m, STREAM_IN_OUT, 4) == 2);
    MAGICdestroy(m);
    printf("------Test 1 passed------\n");

    // TEST 2 : ONLY ADD
    m = MAGICinit();
    MAGICadd(m, 0, 2);
    MAGICadd(m, 10, 2);
    assert(MAGICmap(m, STREAM_IN_OUT, 0) == 2);
    assert(MAGICmap(m, STREAM_IN_OUT, 1) == 3);
    assert(MAGICmap(m, STREAM_IN_OUT, 2) == 4);
    assert(MAGICmap(m, STREAM_IN_OUT, 3) == 5);
    assert(MAGICmap(m, STREAM_IN_OUT, 4) == 6);
    assert(MAGICmap(m, STREAM_IN_OUT, 5) == 7);
    assert(MAGICmap(m, STREAM_IN_OUT, 6) == 8);
    assert(MAGICmap(m, STREAM_IN_OUT, 7) == 9);
    assert(MAGICmap(m, STREAM_IN_OUT, 8) == 12);
    assert(MAGICmap(m, STREAM_IN_OUT, 9) == 13);
    assert(MAGICmap(m, STREAM_IN_OUT, 10) == 14);
    assert(MAGICmap(m, STREAM_IN_OUT, 11) == 15);
    MAGICdestroy(m);
    printf("------Test 2 passed------\n");


    // TEST 3 : ADD AND REMOVE
    m = MAGICinit();
    MAGICadd(m, 0, 2);
    MAGICremove(m, 0, 2);
    assert(MAGICmap(m, STREAM_IN_OUT, 0) == 0);
    assert(MAGICmap(m, STREAM_IN_OUT, 1) == 1);
    MAGICdestroy(m);
    printf("------Test 3 passed------\n");

    // TEST 4 : Brief example
    m = MAGICinit();
    MAGICremove(m, 3, 2);
    MAGICremove(m, 4, 3);
    MAGICadd(m, 4, 2);
    MAGICadd(m, 9, 3);
    assert(MAGICmap(m, STREAM_IN_OUT, 0) == 0); // a
    assert(MAGICmap(m, STREAM_IN_OUT, 1) == 1); // b
    assert(MAGICmap(m, STREAM_IN_OUT, 2) == 2); // c
    printf("%d\n", MAGICmap(m, STREAM_IN_OUT, 3));
    assert(MAGICmap(m, STREAM_IN_OUT, 3) == -1); // d
    assert(MAGICmap(m, STREAM_IN_OUT, 4) == -1); // e
    assert(MAGICmap(m, STREAM_IN_OUT, 5) == 3); // f
    
    assert(MAGICmap(m, STREAM_IN_OUT, 6) == -1); // g
    assert(MAGICmap(m, STREAM_IN_OUT, 7) == -1); // h
    assert(MAGICmap(m, STREAM_IN_OUT, 8) == -1); // i
    assert(MAGICmap(m, STREAM_IN_OUT, 9) == 6); // j
    assert(MAGICmap(m, STREAM_IN_OUT, 10) == 7); // k
    assert(MAGICmap(m, STREAM_IN_OUT, 11) == 8); // l
    assert(MAGICmap(m, STREAM_IN_OUT, 12) == 12); // m
    assert(MAGICmap(m, STREAM_IN_OUT, 13) == 13); // n
    assert(MAGICmap(m, STREAM_IN_OUT, 14) == 14); // o
    assert(MAGICmap(m, STREAM_IN_OUT, 15) == 15); // p
    assert(MAGICmap(m, STREAM_IN_OUT, 16) == 16); // q
    MAGICdestroy(m);
    printf("------Test 4 passed------\n");

    // TEST 5 : Large test
    m = MAGICinit();
    for (int i = 0; i < 3; ++i) 
    {
        MAGICadd(m, i, 1);
        MAGICremove(m, i, 1);
    }
    for (int i = 0; i < 3; ++i) 
    {
        assert(MAGICmap(m, STREAM_IN_OUT, i) == i);
    }
    MAGICdestroy(m);
    printf("------Test 5 passed------\n");

    //===================================================
    //================= OUT -> IN TESTS =================
    //===================================================
    printf("\n==== OUT -> IN TESTS ====\n");

    // TEST A: ONLY ADD + OUT -> IN mapping
    m = MAGICinit();
    MAGICadd(m, 0, 2);
    MAGICadd(m, 5, 1);

    assert(MAGICmap(m, STREAM_OUT_IN, 0) == -1);
    assert(MAGICmap(m, STREAM_OUT_IN, 1) == -1);
    assert(MAGICmap(m, STREAM_OUT_IN, 2) == 0); 
    assert(MAGICmap(m, STREAM_OUT_IN, 3) == 1); 
    assert(MAGICmap(m, STREAM_OUT_IN, 4) == 2); 
    assert(MAGICmap(m, STREAM_OUT_IN, 5) == -1);
    assert(MAGICmap(m, STREAM_OUT_IN, 6) == 3);
    assert(MAGICmap(m, STREAM_OUT_IN, 7) == 4);

    MAGICdestroy(m);
    printf("------Test A passed------\n");

    // TEST B: ONLY REMOVE + OUT -> IN mapping
    m = MAGICinit();
    MAGICremove(m, 1, 2);
    assert(MAGICmap(m, STREAM_OUT_IN, 0) == 0);
    assert(MAGICmap(m, STREAM_OUT_IN, 1) == 3);
    assert(MAGICmap(m, STREAM_OUT_IN, 2) == 4);
    assert(MAGICmap(m, STREAM_OUT_IN, 3) == 5);
    assert(MAGICmap(m, STREAM_OUT_IN, 4) == 6);
    assert(MAGICmap(m, STREAM_OUT_IN, 5) == 7);

    MAGICdestroy(m);
    printf("------Test B passed------\n");

    // TEST C: ADD + REMOVE + OUT -> IN
    m = MAGICinit();
    MAGICadd(m, 0, 1);
    MAGICadd(m, 3, 1);
    MAGICremove(m, 1, 2);

    assert(MAGICmap(m, STREAM_OUT_IN, 0) == 1);
    assert(MAGICmap(m, STREAM_OUT_IN, 1) == -1);
    assert(MAGICmap(m, STREAM_OUT_IN, 2) == 2);
    assert(MAGICmap(m, STREAM_OUT_IN, 3) == 3);
    assert(MAGICmap(m, STREAM_OUT_IN, 4) == 4);
    MAGICdestroy(m);
    printf("------Test C passed------\n");

    return 0;
}
