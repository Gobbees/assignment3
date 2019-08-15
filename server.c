#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "check_phase.h"

#define ff fflush(stdout);

void manage_request(int socket_file_descriptor);

int main(int argc, const char * argv[]) {
    struct sockaddr_in server_address; // struct containing server address information
    socklen_t server_addr_length;
    ssize_t byteRecv;
    struct sockaddr_in client_addr; // struct containing client address information
    int sfd;
    char receivedData [1024]; // Data to be received

    if (argc != 3) {
		printf("Wrong parameter count:\n");
		printf("%s <server IP (dotted notation)> <server port>\n", argv[0]);
		exit(1);
    }
    sfd = socket(AF_INET, SOCK_STREAM, 0);
  
    if (sfd < 0){
        perror("socket opening failed"); // Print error message
        exit(EXIT_FAILURE);
    }
  
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));


    int bind_return_value = bind(sfd, (struct sockaddr *) &server_address, sizeof(server_address));
    if(bind_return_value == -1) {
        perror("Bind operation failed");
        exit(EXIT_FAILURE);
    }

    int listen_return_value = listen(sfd, 20); //TODO: define MAX_CONNECTIONS 20
    if(listen_return_value == -1) {
        perror("Listen operation failed");
        exit(EXIT_FAILURE);
    }
    socklen_t size = sizeof(client_addr);
    while(1) {
        int opened_sfd = accept(sfd, (struct sockaddr *) &client_addr, &size);
        int pid = fork();
        if(pid < 0) {
            fprintf(stderr, "Cannot fork");
            exit(1);
        } else if(pid == 0) {
            close(sfd);
            manage_request(opened_sfd);
            exit(0);
        } else {
            close(opened_sfd);
        }
    }
    
    return 0;
}

void manage_request(int socket_file_descriptor) {
    char *message;
    hello_message request;
    measurement_message measurement;
    char output_message[100];
    while(1) {
        int message_length = recv(socket_file_descriptor, message, 1024, 0);
        if(message_length == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        if(is_hello_phase(message, message_length)) {
            int parse_hello_message_return_value = parse_hello_message(message, &request);
            if(parse_hello_message_return_value == 1) {
                strcpy(output_message, "404 ERROR - Invalid Hello message");
                send(socket_file_descriptor, output_message, 100, 0);
            } else {
                strcpy(output_message, "200 OK - Ready");
                send(socket_file_descriptor, output_message, 100, 0);
            }
        } else if(is_measurement_phase(message, message_length)) {
            if(is_request_uninitialized(request)) {
                strcpy(output_message, "404 ERROR - Invalid Measurement message");
                send(socket_file_descriptor, output_message, 100, 0);
                close(socket_file_descriptor);
                exit(1);
            }
            int parse_and_check_measurement_message_return_value = parse_and_check_measurement_message(message, request, &measurement);
            if(parse_and_check_measurement_message_return_value == 1) {
                strcpy(output_message, "404 ERROR - Invalid Measurement message");
                send(socket_file_descriptor, output_message, 100, 0);
                close(socket_file_descriptor);
                exit(1);
            } else {
                request.probes_counted++;
                strcpy(output_message, measurement.payload);
                send(socket_file_descriptor, output_message, 100, 0);
            }
        } else if(is_bye_phase(message, message_length)) {
            strcpy(output_message, "200 OK - Closing");
            send(socket_file_descriptor, output_message, 100, 0);
            close(socket_file_descriptor);
            exit(1);
        } else {
            strcpy(output_message, "404 ERROR - No operation found");
            send(socket_file_descriptor, output_message, 100, 0);
            close(socket_file_descriptor);
            exit(1);
        }
    }
}