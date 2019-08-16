#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "myfunction.h"
#include "message_sizes.h"
#include "client_executor.h"

#define RTT 1
#define THROUGHPUT 2

int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr; // struct containing server address information
    struct sockaddr_in client_addr; // struct containing client address information
    int sfd; // Server socket filed descriptor
    int cr; // Connect result
    int stop = 0;
    ssize_t byteRecv; // Number of bytes received
    ssize_t byteSent; // Number of bytes sent
    size_t msgLen;
    char receivedData[MAX_MESSAGE_SIZE]; // Data to be received
    char sendData[MAX_MESSAGE_SIZE]; // Data to be sent

    if (argc != 3) {
        printf("\nErrore numero errato di parametri\n");
        printf("\n%s <server IP (dotted notation)> <server port>\n", argv[0]);
        exit(1);
    }

    int mode;
    printf("Select the operation you want to perform:\n- 1: rtt measurement\n- 2: throughput measurement\n");
    scanf("%d", &mode);
    if(mode == RTT) {
        execute_rtt(argv[1], argv[2]);   
    } else if(mode == THROUGHPUT) {
    
    } else { 
        fprintf(stderr, "Invalid mode");
        exit(1);
    }

    return 0;
}

