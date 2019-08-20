#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utilities.h"

void print_string(FILE *file, char *string) {
    int counter = 0;
    while(string[counter] != '\0') {
        fprintf(file, "%c", string[counter++]);
    }
    printf("\n");
}

int max_array(int array[], int array_length) {
    int max = array[0];
    for(int i = 1; i < array_length; i++) {
        if(array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

void check_allocation(void * pointer) {
    if(pointer == NULL) {
        fprintf(stderr, "There was a problem allocating memory.\n"); ff;
        exit(1);
    }
}


void log_on_file(int packet_size, int rtt, char path[]){
    FILE * write_file = fopen(path, "a");
    if (write_file == NULL){
        fprintf(stderr, "There was a problem opening the file.\n");
    }
    fprintf(write_file, "%d %d\n", packet_size, rtt);
    fclose(write_file);
}