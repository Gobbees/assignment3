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

#define ff fflush(stdout);

char * get_string_by_length(int length);

int execute_rtt(char *ip_address, char *port) {

    double rtts[120];
    
    char receivedData[1024];
    int measurements[6] = {1, 100, 200, 400, 800, 1000};
    struct timespec start, stop;
    
    char *string = get_string_by_length(1000);
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
        recv(sfd, receivedData, 1000, 0);
        int size = measurements[i];
        for(int j = 1; j <= 20; j++) {
            char data[1024];
            sprintf(data, "m %d %.*s", j, size, string);
            printf("%s\n", data); ff;
            printf("String of length %d going to be sent to server.\n", size); ff;

            clock_gettime(CLOCK_REALTIME, &start);
            send(sfd, data, strlen(data), 0);
            ssize_t byteRecv = recv(sfd, receivedData, 1024, 0);
            clock_gettime(CLOCK_REALTIME, &stop);
            printf("%d %zd \n", size, byteRecv); ff;
            if((int) byteRecv != size) {
                // printData(receivedData, byteRecv); ff;
                exit(1);
            }
            rtts[(i + 1) * j + j] = (double) (stop.tv_nsec - start.tv_nsec);
            printf("Time: %lf\n", rtts[(i + 1) * j + j]); ff;
        }
        char data[1024];
        sprintf(data, "b");
        send(sfd, data, 1024, 0);
        close(sfd);
    }

    double total_time = 0;
    for(int i = 0; i < 120; i++) {
        total_time += rtts[i];
    }
    printf("RTT: %lf ms\n", total_time / (120 * 1000000)); ff;

    return 0;
}

int execute_throughput(char *ip_address, char *port) {

    double rtts[100];
    
    char data[50000];
    int measurements[5] = {1000, 2000, 4000, 16000, 32000};
    struct timespec start, stop;
    
    char *string = get_string_by_length(50000);
    for(int i = 0; i < 5; i++) {

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
        sprintf(str, "h thput 20 %d 0", measurements[i]);
        send(sfd, str, 80, 0);
        recv(sfd, data, 1000, 0);
        int size = measurements[i];
        for(int j = 1; j <= 20; j++) {
            sprintf(data, "m %d %.*s", j, size, string);
            printf("String of length %d going to be sent to server. Data lenght: %lu\n", size, strlen(data)); ff;

            clock_gettime(CLOCK_REALTIME, &start);
            send(sfd, data, strlen(data), 0);
            ssize_t byteRecv = recv(sfd, data, 32800, 0);
            clock_gettime(CLOCK_REALTIME, &stop);
            printf("%d %zd \n", size, byteRecv); ff;
            if((int) byteRecv != size) {
                // printData(data, byteRecv); ff;
                exit(1);
            }
            rtts[(i + 1) * j + j] = size / ((double) (stop.tv_nsec - start.tv_nsec) / 1000000000);
            printf("Time: %lf\n", rtts[(i + 1) * j + j]); ff;
        }
        sprintf(data, "b");
        send(sfd, data, 50000, 0);
        close(sfd);
    }

    double total_time = 0;
    for(int i = 0; i < 100; i++) {
        total_time += rtts[i];
    }
    printf("RTT: %lf ms\n", total_time / 100); ff;

    exit(0);
}
