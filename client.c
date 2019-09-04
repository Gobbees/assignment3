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

typedef struct{
    int code;
    char * as_string;
} operation;

/**
 * 
 */
int execute_request(operation operation, int sizes[], int sizes_length, int n_measurements_for_size, int server_delay);
/**
 * 
 */
char * get_string_by_length(int length);
/**
 * 
 */
double get_average_by_operation(int operation, long rtts[], int start, int end, int sizes[], int n_measurements_for_size);

/**
 * Returns the current time in milliseconds.
 */
long long get_time_in_milliseconds();

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
    if(mode != RTT && mode != THROUGHPUT) {
        fprintf(stderr, "Invalid mode");
        return 1;
    }
    
    int server_delay;
    printf("Insert the server delay: ");
    scanf("%d", &server_delay);
    if(server_delay < 0) {
        fprintf(stderr, "Invalid server delay: it must be a positive number");
        return 1;
    }

    if(mode == RTT) {
        int sizes[] = {1, 100, 200, 400, 800, 1000};
        int n_measurements_for_size = 20;
        operation op;
        op.code = RTT;
        op.as_string = "RTT";
        execute_request(op, sizes, sizeof(sizes)/sizeof(int), n_measurements_for_size, server_delay);
    } else {
        int sizes[] = {1000,2000,4000,16000,32000};
        int n_measurements_for_size = 1;
        operation op;
        op.code = THROUGHPUT;
        op.as_string = "Throughput";
        execute_request(op, sizes, sizeof(sizes)/sizeof(int), n_measurements_for_size, server_delay);
    }

    return 0;
}

int execute_request(operation operation, int sizes[], int sizes_length, int n_measurements_for_size, int server_delay) {
    int number_of_measurements = n_measurements_for_size * sizes_length;
    long rtts[number_of_measurements];

    char *hello_operation = NULL;
    if(operation.code == RTT) {
        hello_operation = "rtt";
    } else {
        hello_operation = "thput";
    }
    int max_size = max_array(sizes, sizes_length);
    // + 50 since the messages are longer than the max_size since they contain also the message fields
    char *rcv_buffer = (char *) malloc(max_size + 50); 
    check_allocation(rcv_buffer);
    char *send_buffer = (char *) malloc(max_size + 50);
    long long start, end;
    ssize_t recv_message_size;
    
    char *string = get_string_by_length(max_size);

    for(int i = 0; i < sizes_length; i++) {
        int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sfd == -1) {
            fprintf(stderr, "Socket initialization failed");
            return 1;
        }
        
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip_address);
        server_addr.sin_port = htons(atoi(port));
        
        int connect_return_value = connect(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (connect_return_value == -1) {
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
            start = get_time_in_milliseconds();
            send(sfd, send_buffer, strlen(send_buffer), 0);
            int tmp_recv_message_size = 0;
            recv_message_size = 0;
            while(recv_message_size != size && tmp_recv_message_size != -1) {
                tmp_recv_message_size = recv(sfd, rcv_buffer, max_size, 0);
                recv_message_size += tmp_recv_message_size;
            }
            end = get_time_in_milliseconds();
            
            if(recv_message_size != size) {
                fprintf(stderr, "Error: recv returned wrong string: "); ff;
                print_string(stderr, rcv_buffer);
                exit(1);
            }
            rtts[i * n_measurements_for_size + j - 1] = end - start;
            printf("Measured RTT for message of size %d: %ld ms.\n", size, rtts[i * n_measurements_for_size + j - 1]);
        }
        printf("Average %s for message of size %d: %lf\n", operation.as_string, size, get_average_by_operation(RTT, rtts, i * n_measurements_for_size, (i + 1) * n_measurements_for_size, sizes, n_measurements_for_size));
        //Bye phase
        sprintf(send_buffer, "b");
        send(sfd, send_buffer, max_size, 0);
        recv_message_size = recv(sfd, rcv_buffer, max_size, 0);
        printf("%zd ", recv_message_size);
        print_string(stdout, rcv_buffer);
        if(recv_message_size == -1 || strcmp(rcv_buffer, "200 OK - Closing") != 0) {
            fprintf(stderr, "Error: recv returned wrong string: "); ff;
            print_string(stderr, rcv_buffer);
        }
        close(sfd);
    }

    printf("Total Average %s: %lf", operation.as_string, get_average_by_operation(operation.code, rtts, 0, number_of_measurements, sizes, n_measurements_for_size));
    
    return 0;
}

double get_average_by_operation(int operation_code, long rtts[], int start, int end, int sizes[], int n_measurements_for_size) {
    if(operation_code == RTT) {
        long total_rtts = 0;
        for(int i = start; i < end; i++) {
            total_rtts += rtts[i];
        }
        return (double) (total_rtts / (end - start));
    } else if(operation_code == THROUGHPUT) {
        double total_throughputs = 0;
        for(int i = start; i < end; i++) {
            int size = sizes[i / n_measurements_for_size];
            total_throughputs += (double) ((size * 1E6) / (rtts[i] / 1E3));
        }
        return total_throughputs / (end - start);
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

long long get_time_in_milliseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);   
}