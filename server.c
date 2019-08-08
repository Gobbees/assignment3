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
#include "parse_message.h"

#define ff fflush(stdout);

void manage_request(int socket_file_descriptor, struct sockaddr_in address);

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
    while(1) {
        socklen_t size = sizeof(client_addr);
        int opened_sfd = accept(sfd, (struct sockaddr *) &server_address, &size);
        manage_request(opened_sfd, server_address);    
    }
    

    close(sfd);
    
    return 0;
}

void manage_request(int socket_file_descriptor, struct sockaddr_in address) {
    char *message;
    hello_message request;
    measurement_message measurement;
    while(1) {
        int messasge_length = recv(socket_file_descriptor, message, 1024, 0);
        if(messasge_length == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        if(is_hello_phase(message)) {
            printf("ok");
            int parse_hello_message_return_value = parse_hello_message(message, &request);
            char output_message[100];
            strcpy(output_message, "404 ERROR - Invalid Hello message");
            if(parse_hello_message_return_value == -1) {
                send(socket_file_descriptor, output_message, 100, 0);
            }
            strcpy(output_message, "200 OK - Ready");
            send(socket_file_descriptor, output_message, 100, 0);

        } else if(is_measurement_phase(message)){
            int parse_measurement_message_return_value = parse_and_check_measurement_message(message, request, &measurement);
        }    
    }
    close(socket_file_descriptor);
}
