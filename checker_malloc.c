#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "checker_malloc.h"

void check_allocation(void * allocated_pointer){
    if (allocated_pointer == NULL){
        fprintf(stderr, "There was a problem allocating memory\n");
        fflush(stdout);
        exit(1);
    }
}