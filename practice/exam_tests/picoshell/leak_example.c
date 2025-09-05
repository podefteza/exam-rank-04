#include <stdlib.h>
#include <stdio.h>

int main() {
    // This creates a REAL memory leak
    char *leaked_memory = malloc(100);
    printf("This program has a memory leak!\n");
    // Note: we don't free(leaked_memory) - this is a leak!
    return 0;
}
