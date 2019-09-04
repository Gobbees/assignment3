#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
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
        exit(EXIT_FAILURE);
    }
}

void check_send(int socket_file_descriptor, char *send_message, ssize_t send_return_value) {
    char output_error[100];
    if(send_return_value == -1) {
        sprintf(output_error, "404 ERROR - Send Failed. Errno variable set to %d", errno);
        fprintf(stderr, "%s\n", output_error); ff;
        send(socket_file_descriptor, output_error, strlen(output_error), 0); 
        exit(EXIT_FAILURE);
    } else {
        printf("Sent message: %s\n", send_message); ff;
    }
}

void check_recv(int socket_file_descriptor, char *recv_message, ssize_t recv_return_value) {
    char output_error[100];
    if(recv_return_value == -1) {
        sprintf(output_error, "404 ERROR - Recv Failed. Errno variable set to %d", errno);
        fprintf(stderr, "%s\n", output_error); ff;
        send(socket_file_descriptor, output_error, strlen(output_error), 0); 
        exit(EXIT_FAILURE);
    } else {
        printf("Received message: "); ff;
        print_string(stdout, recv_message); ff;
    }
}