#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "utilities.h"

#define RTT 1
#define THROUGHPUT 2

/**
 * 
 */
int execute_request(int operation, int sizes[], int sizes_length, int n_measurements_for_size);
/**
 * 
 */
char * get_string_by_length(int length);
/**
 * 
 */
double get_measurement_by_operation(int operation, int size, struct timespec start, struct timespec end);
/**
 * 
 */
void print_average_by_operation(int operation, double measurements[], int measurements_size);

char *ip_address;
char *port;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong parameter count:\n");
        printf("\n%s <server IP (dotted notation)> <server port>\n", argv[0]);
        exit(1);
    }
    ip_address = argv[1];
    port = argv[2];

    int mode;
    printf("Select the operation you want to perform:\n- 1: rtt measurement\n- 2: throughput measurement\n");
    scanf("%d", &mode);
    if(mode == RTT) {
        int sizes[6] = {1, 100, 200, 400, 800, 1000};
        int n_measurements_for_size = 20;
        execute_request(RTT, sizes, 6, n_measurements_for_size);
    } else if(mode == THROUGHPUT) {
        int sizes[5] = {1000, 2000, 4000, 16000, 32000};
        int n_measurements_for_size = 20;
        execute_request(THROUGHPUT, sizes, 5, n_measurements_for_size);
    } else { 
        fprintf(stderr, "Invalid mode");
        return 1;
    }
    return 0;
}

int execute_request(int operation, int sizes[], int sizes_length, int n_measurements_for_size) {

    int number_of_measurements = n_measurements_for_size * sizes_length;
    double measurements[number_of_measurements]; //TODO fix rtt double precision problem

    char *hello_operation = NULL;
    if(operation == RTT) {
        hello_operation = "rtt";
    } else {
        hello_operation = "thput";
    }
    int max_size = max_array(sizes, sizes_length) + 100;
    char rcv_buffer[max_size];
    char send_buffer[max_size];
    struct timespec start, end;
    ssize_t recv_message_size;
    
    char *string = get_string_by_length(max_size);
    for(int i = 0; i < sizes_length; i++) {
        int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sfd < 0){
            perror("socket"); // Print error message
            exit(1);
        }
        
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip_address);
        server_addr.sin_port = htons(atoi(port));
        int cr = connect(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (cr == -1) {
            perror("connect operation failed");
            exit(EXIT_FAILURE);
        }
        
        //Hello phase
        sprintf(send_buffer, "h %s %d %d 0", hello_operation, n_measurements_for_size, sizes[i]);
        send(sfd, send_buffer, strlen(send_buffer), 0);
        recv_message_size = recv(sfd, rcv_buffer, max_size, 0);
        if(recv_message_size == -1 || strcmp(rcv_buffer, "200 OK - Ready") != 0) {
            fprintf(stderr, "Error: recv returned wrong string: "); ff;
            print_string(stderr, rcv_buffer);
        }

        int size = sizes[i];
        for(int j = 1; j <= n_measurements_for_size; j++) {
            sprintf(send_buffer, "m %d %.*s", j, size, string);
            clock_gettime(CLOCK_REALTIME, &start);
            send(sfd, send_buffer, strlen(send_buffer), 0);
            recv_message_size = recv(sfd, rcv_buffer, max_size, 0);
            clock_gettime(CLOCK_REALTIME, &end);
            if(recv_message_size != size) {
                fprintf(stderr,"Error: recv returned wrong string: "); ff;
                print_string(stderr, rcv_buffer);
                exit(1);
            }
            
            measurements[i * j + j] = get_measurement_by_operation(operation, size, start, end);
        }
        sprintf(send_buffer, "b");
        send(sfd, send_buffer, max_size, 0);
        close(sfd);
    }

    print_average_by_operation(operation, measurements, number_of_measurements);
    
    return 0;
}

char * get_string_by_length(int length) {
    char *string = NULL;
    string = (char *) malloc(length);
    if(string == NULL) {
        fprintf(stderr, "Malloc returned NULL");
        exit(1);
    }

    for(int i = 0; i < length; i++) {
        string[i] = 'a';
    }
    return string;
}

double get_measurement_by_operation(int operation, int size, struct timespec start, struct timespec end) {
    if(operation == RTT) {
        return end.tv_nsec - start.tv_nsec;
    } else if(operation == THROUGHPUT) {
        return size / ((end.tv_nsec - start.tv_nsec) / 1E9);
    } else {
        fprintf(stderr, "Unsupported operation.");
        exit(1);
    }
}

void print_average_by_operation(int operation, double measurements[], int measurements_size) {
    long total = 0;
    for(int i=0; i < measurements_size; i++) {
            total += measurements[i];
    }
    double average = (double) total / measurements_size;
    if(operation == RTT) {
        printf("Average RTT: %lf ms.\n", (average / 1E6));
    } else if(operation == THROUGHPUT) { 
        printf("Average Throughput: %lf b/s.\n", average);
    } else {
        fprintf(stderr, "Unsupported operation.");
        exit(1);
    }
}
