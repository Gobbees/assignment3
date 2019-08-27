#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "utilities.h"

#define RTT 1
#define THROUGHPUT 2

/**
 * 
 */
int execute_request(int operation, int sizes[], int sizes_length, int n_measurements_for_size, int server_delay);
/**
 * 
 */
char * get_string_by_length(int length);
/**
 * 
 */
double get_average_by_operation(int operation, long rtts[], int start, int end, int sizes[], int n_measurements_for_size);
/**
 * 
 */
void print_average_by_operation(int operation, double measurements[], int measurements_size);

/**
 * 
 */
double timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

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
    int server_delay;
    printf("Insert the server delay: ");
    scanf("%d", &server_delay);

    if(mode == RTT) {
        int sizes[6] = {1, 100, 200, 400, 800, 1000};
        int n_measurements_for_size = 20;
        execute_request(RTT, sizes, 6, n_measurements_for_size, server_delay);
    } else if(mode == THROUGHPUT) {
        int sizes[5] = {1000, 2000, 4000, 16000, 32000};
        int n_measurements_for_size = 20;
        execute_request(THROUGHPUT, sizes, 5, n_measurements_for_size, server_delay);
    } else { 
        fprintf(stderr, "Invalid mode");
        return 1;
    }
    return 0;
}

int execute_request(int operation, int sizes[], int sizes_length, int n_measurements_for_size, int server_delay) {

    int number_of_measurements = n_measurements_for_size * sizes_length;
    long rtts[number_of_measurements];

    char *hello_operation = NULL;
    if(operation == RTT) {
        hello_operation = "rtt";
    } else {
        hello_operation = "thput";
    }
    int max_size = max_array(sizes, sizes_length) + 100;
    char rcv_buffer[max_size];
    char send_buffer[max_size];
    struct timeval start, end;
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
        sprintf(send_buffer, "h %s %d %d %d", hello_operation, n_measurements_for_size, sizes[i], server_delay);
        send(sfd, send_buffer, strlen(send_buffer), 0);
        recv_message_size = recv(sfd, rcv_buffer, max_size, 0);
        if(recv_message_size == -1 || strcmp(rcv_buffer, "200 OK - Ready") != 0) {
            fprintf(stderr, "Error: recv returned wrong string: "); ff;
            print_string(stderr, rcv_buffer);
        }

        int size = sizes[i];
        //Measurement phase
        for(int j = 1; j <= n_measurements_for_size; j++) {
            sprintf(send_buffer, "m %d %.*s", j, size, string);
            gettimeofday(&start, NULL);
            send(sfd, send_buffer, strlen(send_buffer), 0);
            recv_message_size = recv(sfd, rcv_buffer, max_size, 0);
            gettimeofday(&end, NULL);
            if(recv_message_size != size) {
                fprintf(stderr,"Error: recv returned wrong string: "); ff;
                print_string(stderr, rcv_buffer);
                exit(1);
            }
            rtts[i * j + j] = timedifference_msec(start, end)   ;
            printf("Measured RTT for message of size %d: %ld ms.\n", size, rtts[i * j + j]);
        }
        printf("Average RTT for message of size %d: %lf\n", size, get_average_by_operation(operation, rtts, i * n_measurements_for_size, (i + 1) * n_measurements_for_size, sizes, n_measurements_for_size));
        //Bye phase
        sprintf(send_buffer, "b");
        send(sfd, send_buffer, max_size, 0);
        close(sfd);
    }
    
    return 0;
}

double get_average_by_operation(int operation, long rtts[], int start, int end, int sizes[], int n_measurements_for_size) {
    if(operation == RTT) {
        long total_rtts = 0;
        for(int i = start; i < end; i++) {
            total_rtts += rtts[i];
        }
        return ((double) total_rtts / (end - start)) / 1E6;
    } else if(operation == THROUGHPUT) {
        double total_throughputs = 0;
        for(int i = start; i < end; i++) {
            int size = sizes[i / n_measurements_for_size];
            total_throughputs += (double) (size * 1E3) / (rtts[i] / 1E9);
        }
        return (double) total_throughputs / (end - start);
    } else {
        fprintf(stderr, "Unsupported operation.");
        exit(1);
    }
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
