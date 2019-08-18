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
#include "message_sizes.h"
#include "client_executor.h"
#include "utilities.h"

#define RTT 1
#define THROUGHPUT 2

int execute_request(int operation, int sizes[], int sizes_length, int n_measurements_for_size);
char * get_string_by_length(int length);

char ip_address[30];
char port[30];

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong parameter count:\n");
        printf("\n%s <server IP (dotted notation)> <server port>\n", argv[0]);
        exit(1);
    }
    strcpy(ip_address, argv[1]);
    strcpy(port, argv[2]);

    int mode;
    printf("Select the operation you want to perform:\n- 1: rtt measurement\n- 2: throughput measurement\n");
    scanf("%d", &mode);
    if(mode == RTT) {
        int sizes[6] = {1, 100, 200, 400, 800, 1000};
        int n_measurements_for_size = 20;
        execute_rtt(argv[1], argv[2]);
        //execute_request(RTT, sizes, 6, n_measurements_for_size);
    } else if(mode == THROUGHPUT) {
        int measurements[5] = {1000, 2000, 4000, 16000, 32000};
        int n_measurements_for_size = 20;
        // execute_throughput(argv[1], argv[2]);
        execute_throughput(argv[1], argv[2]);
    } else { 
        fprintf(stderr, "Invalid mode");
        return 1;
    }
    return 0;
}

//int execute_request(int operation, int sizes[], int sizes_length, int n_measurements_for_size) {
//    char *hello_op = NULL;
//    if(operation == RTT) {
//        hello_op = "rtt";
//    } else if(operation == THROUGHPUT) {
//        hello_op = "thput";
//    } else {
//        return 1;
//    }
//    ssize_t recv_message_size;
//
//    struct timespec start, stop;
//
//    double total_measurements[sizes_length * n_measurements_for_size];
//    int max_size = max_array(sizes, sizes_length);
//    char *string = get_string_by_length(max_size);
//
//    char message_recv[max_size];
//
//    for(int i = 0; i < sizes_length; i++) {
//        int sfd = socket(AF_INET, SOCK_STREAM, 0);
//        if (sfd < 0){
//            fprintf(stderr, "Socket creation failed: errno value is %d\n", errno);
//            return 2;
//        }
//        struct sockaddr_in server_addr;
//        server_addr.sin_family = AF_INET;
//        server_addr.sin_addr.s_addr = inet_addr(ip_address);
//        server_addr.sin_port = htons(atoi(port));
//
//        int cr = connect(sfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
//        if (cr == -1) {
//            fprintf(stderr, "Connect operation failed: errno value is %d\n", errno);
//            return 2;
//        }
//
//        //Hello phase
//        char message_send[max_size];
//        sprintf(message_send, "h %s %d %d 0", hello_op, n_measurements_for_size, sizes[i]);
//        send(sfd, message_send, strlen(message_send), 0);
//        recv_message_size = recv(sfd, message_recv, INVALID_HELLO_SIZE, 0);
//        printf("Hello return message from the server: "); ff;
//        print_string(message_recv); ff;
//        if(strcmp(message_recv, "200 OK - Ready") != 0) {
//            fprintf(stderr, "Invalid Hello Message");
//            return 1;
//        }
//        int size = sizes[i];
//
//        //Measurements phase
//        for(int j = 1; j <= 20; j++) {
//            char data[1024];
//            sprintf(data, "m %d %.*s", j, size, string);
//            printf("Message: %s\n", data);
//            printf("String of length %d going to be sent to server. Data lenght: %lu\n", size, strlen(data)); ff;
//
//            clock_gettime(CLOCK_REALTIME, &start);
//            send(sfd, data, strlen(data), 0);
//
//            ssize_t recv_message_sizes = recv(sfd, message_recv, MAX_MESSAGE_SIZE, 0);
//            clock_gettime(CLOCK_REALTIME, &stop);
//            printf("Measurement return message from the server: "); ff;
//            print_string(message_recv); ff;
//            // if(recv_message_sizes != size) {
//            //     print_string(message_recv); ff;
//            //     exit(1);
//            // }
//            // if(operation == RTT) {
//            //     total_measurements[i * j + j] = ((double) (stop.tv_nsec - start.tv_nsec) / 1E6);
//            //     printf("Measured RTT: %lf ms.\n", total_measurements[i * j + j]); ff;
//            // } else {
//            //     total_measurements[i * j + j] = size / ((double) (stop.tv_nsec - start.tv_nsec) / 1E9);
//            //     printf("Measured throughput: %lf b/s.\n", total_measurements[i * j + j]); ff;
//            // }
//        }
//
//        //Bye phase
//        sprintf(message_send, "b");
//        send(sfd, message_send, 50000, 0);
//        close(sfd);
//    }
//
//    return 0;
//}

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
