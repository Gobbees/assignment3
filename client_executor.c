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
#define TRIES 20
#define MAX_SIZE 32100
#define RTT 1
#define THROUGHTPUT 2
#define TO_MILLISECONDS 1000000000

char * get_string_by_length(int length);

void print_average_rtt(long rtts[], int array_length){
    long total = 0;
    for(int i=0; i < array_length; i++){
        total += rtts[i];
    }
    double average = (double) total / array_length;
    printf("Average RTT: %e\n", (average / TO_MILLISECONDS));
}

void print_average_throughtput(long rtts[]){
    //TODO: calculate throughput
}

void execute(char * port, char * ip_address, int measurements[], int measurements_size, int mode){
    int number_of_measurements = TRIES * measurements_size;
    long rtts[number_of_measurements];
    
    char rcv_buffer[MAX_SIZE];
    char send_buffer[MAX_SIZE];
    struct timespec start, stop;
    
    char *string = get_string_by_length(MAX_SIZE);
    for(int i = 0; i < measurements_size; i++) {
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
        
        sprintf(send_buffer, "h %s 20 %d 0", mode == RTT ? "rtt" : "thput", measurements[i]);
        send(sfd, send_buffer, strlen(send_buffer), 0);
        recv(sfd, rcv_buffer, MAX_SIZE, 0);
        int size = measurements[i];
        for(int j = 1; j <= TRIES; j++) {
            sprintf(send_buffer, "m %d %.*s", j, size, string);
            printf("String of length %d going to be sent to server.\n", size); ff;
            
            clock_gettime(CLOCK_REALTIME, &start);
            send(sfd, send_buffer, strlen(send_buffer), 0);
            ssize_t byteRecv = recv(sfd, rcv_buffer, MAX_SIZE, 0);
            clock_gettime(CLOCK_REALTIME, &stop);
            printf("%d %zd \n", size, byteRecv); ff;
            if(byteRecv != size) {
                // printData(rcv_buffer, byteRecv); ff;
                exit(1);
            }
            long current_rtt = stop.tv_nsec - start.tv_nsec;
            rtts[(i + 1) * j + j] = current_rtt;
        }
        sprintf(send_buffer, "b");
        send(sfd, send_buffer, MAX_SIZE, 0);
        close(sfd);
    }
    sleep(1);
    if (mode == RTT) {
        print_average_rtt(rtts, number_of_measurements);
    }else{
        // CHANGE
        print_average_rtt(rtts, number_of_measurements);
    }
    
    exit(0);
}


int execute_rtt(char *ip_address, char *port) {
    int measurements[6] = {1, 100, 200, 400, 800, 1000};
    execute(port, ip_address, measurements, 6, RTT);
    exit(0);
}

int execute_throughput(char *ip_address, char *port) {
    int measurements[5] = {1000, 2000, 4000, 16000, 32000};
    execute(port, ip_address, measurements, 5, THROUGHTPUT);
    exit(0);
}
