//
// Created by bem22 on 17/04/2021.
//

#include <stdio.h>
#include "debug.h"

clock_t tick() {
    printf("\t\t\tTick!!\n");
    return clock();
}
void tock(clock_t start) {
    printf("\t\t\t%s %lf ms\n", "Time elapsed: ", ((double)clock() - start)/CLOCKS_PER_SEC);
}