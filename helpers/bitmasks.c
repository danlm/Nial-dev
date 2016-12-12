#include <stdio.h>
#include <stdlib.h>

int main() {
    long mask64 = 1L;
    int mask32 = 1;
    int i;
    
    /* Write out the masks */
    printf("#ifdef INTS32\n");
    printf("static nialint nialint_masks[33] = {\n");
    printf("   0");
    for (i = 31;i  >= 0; i--) {
        printf(",\n    0x%x", mask32<<i);
        mask32 <<= 1;
        mask32 |= 1;
    }
    printf("\n};\n");
    printf("#endif\n");
    
    printf("#ifdef INTS64\n");
    printf("static nialint nialint_masks[65] = {\n");
    printf("    0");
    for (i = 63; i >= 0; i--) {
        printf(",\n    0x%lxL", mask64<<i);
        mask64 <<= 1;
        mask64 |= 1;
    }
    printf("\n};\n");
    printf("#endif\n");
    
    exit(0);
}