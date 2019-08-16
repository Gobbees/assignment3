#include "utility.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void check_allocation(void * allocated_pointer){
    if (allocated_pointer == NULL){
        printf("There was a problem allocating memory\n");
        fflush(stdout);
        exit(1);
    }
}
