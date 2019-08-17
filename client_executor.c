#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "myfunction.h"

#define ff fflush(stdout);

char * get_string_by_length(int length);

int execute_rtt(char *ip_address, char *port) {
    
    char receivedData[1024];
    int measurements[6] = {400, 800, 1000,2, 2, 2};
    time_t start, end;
    

    for(int i = 0; i < 6; i++) {

        int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    
        if (sfd < 0){
            perror("socket"); // Print error message
            return 1;
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

        char str[80];
        sprintf(str, "h rtt 20 %d 0", measurements[i]);
        send(sfd, str, 80, 0);
        ssize_t byteRecv = recv(sfd, receivedData, 1000, 0);
        int size = measurements[i];
        for(int j = 1; j <= 20; j++) {
            char data[1024];
            sprintf(data, "m %d %s", j, get_string_by_length(size));
            printf("%s\n", data); ff;
            printf("String of length %d going to be sent to server.\n", size); fflush(stdout);

            time(&start);
            ssize_t byteSent = send(sfd, data, strlen(data), 0);
            ssize_t byteRecv = recv(sfd, receivedData, 1024, 0);
            time(&end);
            printf("%d %zd \n", size, byteRecv); ff;
            if((int) byteRecv != size) {
                printData(receivedData, byteRecv); ff;
                exit(1);
            }
            printf("Time: %lf", difftime(end, start)); fflush(stdout);
        }
        char data[1024];
        sprintf(data, "b");
        send(sfd, data, 1024, 0);
        close(sfd);
    }

    return 0;
}


char * get_string_by_length(int length) {
    char *string;
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