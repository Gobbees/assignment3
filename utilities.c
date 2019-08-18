#include <stdio.h>
#include <unistd.h>
#include "utilities.h"

void print_string(char *string, ssize_t string_length) {
    int counter = 0;
    while(string[counter] != '\0') {
        printf("%c", string[counter++]);
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