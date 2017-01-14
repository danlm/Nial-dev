/**
 * Compute the safe bounds for floor functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main() {
    long lival, lrval,  lmval = 1L;
    double dval;
    int i;
    
    for (i = 0; i < 64; i++) {
        lmval *=2;
        lival = lmval - 1;
        dval = lival*1.0;
        lrval = floor(dval);
        printf("i: %d, lival: %ld, lrval: %ld, diff: %ld, dval: %f\n", i, lival, lrval, (lival-lrval), dval);
    }
    
    return 0;
}