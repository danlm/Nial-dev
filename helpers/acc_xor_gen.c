/**
 * Generate tables for an accumulating xor function
 */
 
 #include <stdio.h>
 #include <stdlib.h>
 

#define NUM_BITS 8
#define TBL_SIZE (1 << NUM_BITS)

void build_acc_xor_table(unsigned int ibit, unsigned int *tbits) {
    unsigned int i;
    
    /* Build accumulating xor table for all bit patterns */
    for (i = 0; i < TBL_SIZE; i++) {
        unsigned int j, w = i;
        unsigned int mbit = (1 << (NUM_BITS-1));
        unsigned int cbit = ((ibit&1) << (NUM_BITS-1));

        for (j = 0; j < NUM_BITS; j++) {
            w ^= cbit;          /* Change one bit appropriately */
            cbit = w & mbit;    /* copy that bit                */
            cbit >>= 1;
            mbit >>= 1;
        }
        
        tbits[i] = w;
    }
    
    return;     
 }
 
 
 int main() {
    unsigned int i, ibit, tbits[TBL_SIZE];
    
    for (ibit = 0; ibit < 2; ibit++) {
        build_acc_xor_table(ibit, tbits);
        printf("/* Accumulating xor table for ibit %d */\n", ibit); 
        for (i = 0; i < TBL_SIZE; i++) {
            if ((i%8) == 0)
                printf("   /* %02x */ ", i);
            printf(" 0x%02x,", tbits[i]);
            if ((i %8) == 7)
                printf("\n");
        }
        printf("\n\n");
    
     }
     
     return 0;
 }
 