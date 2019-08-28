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
#include "utilities.h"

#define MAX_MESSAGE_SIZE 33000

void manage_request(int socket_file_descriptor);

int main(int argc, const char * argv[]) {
    struct sockaddr_in server_address; // struct containing server address information
    socklen_t server_addr_length;
    ssize_t byteRecv;
    struct sockaddr_in client_addr; // struct containing client address information
    int sfd;
    char receivedData [MAX_MESSAGE_SIZE]; // Data to be received

    if (argc != 3) {
		printf("Wrong parameter count:\n");
		printf("%s <server IP (dotted notation)> <server port>\n", argv[0]);
		exit(1);
    }
    sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  
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
        printf("Accepted Client: IP Address %u and port %d", client_addr.sin_addr.s_addr, client_addr.sin_port);
        int pid = fork();
        if(pid < 0) {
            fprintf(stderr, "Cannot fork");
            exit(1);
        } else if(pid == 0) {
            printf("Child process created.\n");
            close(sfd);
            manage_request(opened_sfd);
            exit(0);
        } else {
            printf("Created process %d\n", pid);
            close(opened_sfd);
        }
    }
    
    return 0;
}

void manage_request(int socket_file_descriptor) {
    char *message = NULL;
    message = (char *) malloc(MAX_MESSAGE_SIZE); // please add free
    if (message == NULL){
        perror("Could not allocate memory");
        exit(1);
    }
    hello_message request;
    measurement_message measurement;
    measurement.payload = NULL;
    char *output_message;
    while(1) {
        message = memset(message, 0, MAX_MESSAGE_SIZE);
        printf("Waiting for messages...\n"); ff;
        ssize_t message_length = recv(socket_file_descriptor, message, MAX_MESSAGE_SIZE, 0);
        print_string(stdout, message); ff;
        if(message_length == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        if(is_hello_phase(message, message_length)) {
            int parse_hello_message_return_value = parse_hello_message(message, &request);
            if(parse_hello_message_return_value == 1) {
                output_message = "404 ERROR - Invalid Hello message";
                send(socket_file_descriptor, output_message, 100, 0);
            } else {
                output_message = "200 OK - Ready";
                send(socket_file_descriptor, output_message, 100, 0);
            }
        } else if(is_measurement_phase(message, message_length)) {
            if(is_request_uninitialized(request)) {
                printf("Invalid measurement: request uninitialized.\n"); ff;
                output_message = "404 ERROR - Invalid Measurement message";
                send(socket_file_descriptor, output_message, strlen(output_message), 0);
                close(socket_file_descriptor);
                exit(1);
            }
            int parse_and_check_measurement_message_return_value = parse_and_check_measurement_message(message, request, &measurement);
            if(parse_and_check_measurement_message_return_value == 1) {
                printf("Invalid measurement: invalid request.\n"); ff;
                output_message = "404 ERROR - Invalid Measurement message";
                send(socket_file_descriptor, output_message, 100, 0);
                close(socket_file_descriptor);
                exit(1);
            } else {
                while(strlen(measurement.payload) != request.msg_size) {
                    ssize_t message_length = recv(socket_file_descriptor, message, MAX_MESSAGE_SIZE, 0);
                    char *new_payload;
                    new_payload = malloc(strlen(measurement.payload) + message_length + 1);
                    check_allocation(new_payload);
                    new_payload[0] = '\0';  // ensures the memory is an empty string
                    strcat(new_payload, measurement.payload);
                    strcat(new_payload, message);
                    measurement.payload = new_payload;
                }
                sleep(request.server_delay);
                request.probes_counted++;
                send(socket_file_descriptor, measurement.payload, strlen(measurement.payload), 0);
            }
        } else if(is_bye_phase(message, message_length)) {
            output_message = "200 OK - Closing";
            send(socket_file_descriptor, output_message, 100, 0);
            close(socket_file_descriptor);
            exit(1);
        } else {
            printf("Error"); ff;
            output_message = "404 ERROR - No operation found";
            send(socket_file_descriptor, output_message, 100, 0);
            close(socket_file_descriptor);
            exit(1);
        }
    }
}
