#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include "check_phase.h"
#include "../common/utilities.h"

#define MAX_CONNECTIONS 20
#define MAX_MESSAGE_SIZE 33000

/**
 * This function is called whenever a SIGCHLD signal is thrown.
 * This permits to free the dead son process memory.
 */
void handle_signal (int sig);

/**
 * This function implement the logic involved in the computation of a client request.
 * - socket_file_descriptor: the socket on which communications will be transmitted
 */
void manage_request(int socket_file_descriptor);

int main(int argc, const char * argv[]) {
    struct sockaddr_in server_address;
    ssize_t byteRecv;
    struct sockaddr_in client_addr;
    int socket_file_descriptor;

    if (argc != 2) {
		printf("Wrong parameter count:\n");
		printf("%s <server port>\n", argv[0]);
		exit(EXIT_FAILURE);
    }

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_file_descriptor == -1) {
        perror("Socket opening failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(atoi(argv[1]));

    int bind_return_value = bind(socket_file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address));
    if(bind_return_value == -1) {
        perror("Bind operation failed");
        exit(EXIT_FAILURE);
    }

    int listen_return_value = listen(socket_file_descriptor, MAX_CONNECTIONS); 
    if(listen_return_value == -1) {
        perror("Listen opening failed");
        exit(EXIT_FAILURE);
    }
    signal(SIGCHLD, handle_signal);
    socklen_t client_addr_size;
    while(1) {
        int opened_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *) &client_addr, &client_addr_size);
        if(opened_socket_file_descriptor == -1) {
            perror("Accepted socket opening failed");
            exit(EXIT_FAILURE);
        }
        printf("Accepted Client: IP Address %s and port %d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port); ff;
        int pid = fork();
        if(pid < 0) {
            fprintf(stderr, "Cannot fork");
            exit(EXIT_FAILURE);
        } else if(pid == 0) {
            close(socket_file_descriptor);
            manage_request(opened_socket_file_descriptor);
            exit(0);
        } else {
            close(opened_socket_file_descriptor);
        }
    }
    return 0;
}

void handle_signal (int sig){
    switch (sig) {
        case SIGCHLD:
            return;
        default : printf ("Signal not known!\n");
            break;
    }
}

void manage_request(int socket_file_descriptor) {
    char *buffer = (char *) malloc(MAX_MESSAGE_SIZE);
    check_allocation(buffer);
    hello_message request = get_uninitialized_hello_message();
    measurement_message measurement;
    measurement.payload = NULL;
    char *output_message;

    while(1) {
        buffer = memset(buffer, '\0', MAX_MESSAGE_SIZE);
        ssize_t message_length = recv(socket_file_descriptor, buffer, MAX_MESSAGE_SIZE, 0);
        check_recv(socket_file_descriptor, buffer, message_length);
        if(is_hello_phase(buffer, message_length)) {
            int parse_hello_message_return_value = parse_hello_message(buffer, &request);
            if(parse_hello_message_return_value == 1) {
                output_message = "404 ERROR - Invalid Hello message";
                check_send(socket_file_descriptor, output_message, send(socket_file_descriptor, output_message, strlen(output_message), 0));
            } else {
                output_message = "200 OK - Ready";
                check_send(socket_file_descriptor, output_message, send(socket_file_descriptor, output_message, 100, 0));
            }
        } else if(is_measurement_phase(buffer, message_length)) {
            if(is_request_uninitialized(request)) { 
                printf("Invalid measurement: request uninitialized.\n"); ff;
                output_message = "404 ERROR - Invalid Measurement message";
                check_send(socket_file_descriptor, output_message, send(socket_file_descriptor, output_message, strlen(output_message), 0));
                close(socket_file_descriptor);
                exit(EXIT_FAILURE);
            }
            int parse_and_check_measurement_message_return_value = parse_and_check_measurement_message(buffer, request, &measurement);
            if(parse_and_check_measurement_message_return_value == 1) {
                printf("Invalid measurement: invalid request.\n"); ff;
                output_message = "404 ERROR - Invalid Measurement message";
                check_send(socket_file_descriptor, output_message, send(socket_file_descriptor, output_message, strlen(output_message), 0));
                close(socket_file_descriptor);
                exit(EXIT_FAILURE);
            } else {
                while(strlen(measurement.payload) != request.msg_size) {
                    ssize_t message_length = recv(socket_file_descriptor, buffer, MAX_MESSAGE_SIZE, 0);
                    char *new_payload;
                    new_payload = (char *) malloc(strlen(measurement.payload) + message_length + 1);
                    check_allocation(new_payload);
                    new_payload[0] = '\0';  // ensures the memory is an empty string
                    strcat(new_payload, measurement.payload);
                    char message_length_message[message_length];
                    sprintf(message_length_message, "%.*s", (int) message_length, buffer);
                    strcat(new_payload, message_length_message);
                    free(measurement.payload);
                    measurement.payload = new_payload;
                }
                sleep(request.server_delay);
                request.next_probe++;
                int send_return_value = send(socket_file_descriptor, measurement.payload, strlen(measurement.payload), 0);
                check_send(socket_file_descriptor, measurement.payload, send_return_value);
            }
        } else if(is_bye_phase(buffer, message_length)) {
            output_message = "200 OK - Closing";
            ssize_t send_return_value = send(socket_file_descriptor, output_message, strlen(output_message), 0);
            check_send(socket_file_descriptor, output_message, send_return_value);
            close(socket_file_descriptor);
            exit(EXIT_SUCCESS);
        } else {
            output_message = "404 ERROR - No operation found";
            ssize_t send_return_value = send(socket_file_descriptor, output_message, strlen(output_message), 0);
            check_send(socket_file_descriptor, output_message, send_return_value);
            exit(EXIT_FAILURE);
        }
    }

}
