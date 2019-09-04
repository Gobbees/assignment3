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
#include "../common/utilities.h"

#define RTT 1
#define THROUGHPUT 2

/**
 * Defines an operation (RTT or THROUGHPUT) and some utils.
 */
typedef struct{
    int code;
    char * as_string;
    char * measure_unit;
} operation;

/**
 * Executes a measurement request.
 * - operation: the operation (RTT or THROUGPUT)
 * - sizes: the sizes of the various measurement probes
 * - sizes_length: the number of sizes
 * - n_measurements_for_size: the number of measurements for each size
 */
void execute_request(operation operation, int sizes[], int sizes_length, int n_measurements_for_size, int server_delay);

/**
 * Returns a string of length 'length' where all chars are set to 'a'.
 * - length: the length of the string that will be returned
 */
char * get_string_by_length(int length);

/**
 * Returns the average by operation. In particular:
 * - if operation_code is RTT, it returns the average RTT from start to end
 * - if operation_code is THROUGHPUT, it returns the througput computed as size / (avg_rtt from start to end)
 * 
 * - operation_code: the operation code
 * - rtts: the array that contains the rtts
 * - start: the starting point of the avg computation
 * - end: the ending point of the avg computation
 * - size: the size of the messages 
 */
double get_average_by_operation(int operation_code, long rtts[], int start, int end, int size);

/**
 * Returns the current time in milliseconds.
 */
long long get_time_in_milliseconds();

/**
 * Print on a file named operation.txt the given result. 
 * If the file does not exists, it's created. If it is exist, the print is appended on the file.
 * The operation string must be less thant 50 characters.
 */
void print_on_file(char * operation, int size, double average);

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

    //tries to connect to test that the server is working
    int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_file_descriptor == -1) {
        perror("Socket initialization failed");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_port = htons(atoi(port));
    
    int connect_return_value = connect(socket_file_descriptor, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connect_return_value == -1) {
        perror("connect operation failed");
        exit(EXIT_FAILURE);
    }
    close(socket_file_descriptor);
        
    int mode;
    printf("Select the operation you want to perform:\n- 1: rtt measurement\n- 2: throughput measurement\n");
    scanf("%d", &mode);
    if(mode != RTT && mode != THROUGHPUT) {
        fprintf(stderr, "Invalid mode");
        return 1;
    }

    int sizes[100];
    int n_sizes = 0;
    int new_size = 0;
    while(1) {
        printf("Add a new size to the measurement (-1 to exit size input)(MAX 32,768): ");
        scanf("%d", &new_size);
        if(new_size == -1) {
            break;
        } else if(new_size <= 0) {
            fprintf(stderr, "Invalid size: it must be a positive non-zero number.\n");
            continue;
        } 
        sizes[n_sizes++] = new_size;
    }

    if(n_sizes == 0) {
        printf("No sizes. Closing\n");
        exit(EXIT_SUCCESS);
    }

    int n_measurements_for_size;
    printf("Insert the number of measurement for each size: ");
    scanf("%d", &n_measurements_for_size);
    if(n_measurements_for_size <= 0) {
        fprintf(stderr, "Invalid number of measurement for each size: it must be a positive non-zero number.\n");
        return 1;
    }
    
    int server_delay;
    printf("Insert the server delay: ");
    scanf("%d", &server_delay);
    if(server_delay < 0) {
        fprintf(stderr, "Invalid server delay: it must be a positive number.\n");
        return 1;
    }

    if(mode == RTT) {
        operation op;
        op.code = RTT;
        op.as_string = "RTT";
        op.measure_unit = "ms";
        execute_request(op, sizes, n_sizes, n_measurements_for_size, server_delay);
    } else {
        operation op;
        op.code = THROUGHPUT;
        op.as_string = "Throughput";
        op.measure_unit = "kbps";
        execute_request(op, sizes, n_sizes, n_measurements_for_size, server_delay);
    }

    return 0;
}

void execute_request(operation operation, int sizes[], int sizes_length, int n_measurements_for_size, int server_delay) {
    int number_of_measurements = n_measurements_for_size * sizes_length;
    long rtts[number_of_measurements];
    double total_average = 0;

    char *hello_operation = NULL;
    if(operation.code == RTT) {
        hello_operation = "rtt";
    } else {
        hello_operation = "thput";
    }
    int max_size = max_array(sizes, sizes_length) + 50;
    // + 50 since the messages are longer than the max_size since they contain also the message fields
    char *recv_buffer = (char *) malloc(max_size); 
    check_allocation(recv_buffer);
    char *send_buffer = (char *) malloc(max_size);
    check_allocation(send_buffer);
    long long start, end;
    ssize_t send_return_value;
    ssize_t recv_message_size;
    
    char *string = get_string_by_length(max_size);

    for(int i = 0; i < sizes_length; i++) {
        int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_file_descriptor == -1) {
            perror("Socket initialization failed");
            exit(EXIT_FAILURE);
        }
        
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip_address);
        server_addr.sin_port = htons(atoi(port));
        
        int connect_return_value = connect(socket_file_descriptor, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (connect_return_value == -1) {
            perror("connect operation failed");
            exit(EXIT_FAILURE);
        }
        
        //Hello phase
        sprintf(send_buffer, "h %s %d %d %d", hello_operation, n_measurements_for_size, sizes[i], server_delay);
        send_return_value = send(socket_file_descriptor, send_buffer, strlen(send_buffer), 0);
        check_send(socket_file_descriptor, send_buffer, send_return_value);
        send_buffer = memset(send_buffer, '\0', max_size);
        recv_buffer = memset(recv_buffer, '\0', max_size);
        recv_message_size = recv(socket_file_descriptor, recv_buffer, max_size, 0);
        check_recv(socket_file_descriptor, recv_buffer, recv_message_size);
        if(strcmp(recv_buffer, "200 OK - Ready") != 0) {
            fprintf(stderr, "Error: recv returned wrong string: "); ff;
            print_string(stderr, recv_buffer);
        }

        int size = sizes[i];
        //Measurement phase
        for(int j = 1; j <= n_measurements_for_size; j++) {
            sprintf(send_buffer, "m %d %.*s", j, size, string);
            start = get_time_in_milliseconds();
            send_return_value = send(socket_file_descriptor, send_buffer, strlen(send_buffer), 0);
            check_send(socket_file_descriptor, send_buffer, send_return_value);
            send_buffer = memset(send_buffer, '\0', max_size);
            int tmp_recv_message_size = 0;
            recv_message_size = 0;
            while(recv_message_size != size && tmp_recv_message_size != -1) {
                recv_buffer = memset(recv_buffer, '\0', max_size);
                tmp_recv_message_size = recv(socket_file_descriptor, recv_buffer, max_size, 0);
                check_recv(socket_file_descriptor, recv_buffer, tmp_recv_message_size);
                recv_message_size += tmp_recv_message_size;
            }
            end = get_time_in_milliseconds();
            
            if(recv_message_size != size) {
                fprintf(stderr, "Error: recv returned wrong string: "); ff;
                print_string(stderr, recv_buffer);
                exit(EXIT_FAILURE);
            }
            rtts[i * n_measurements_for_size + j - 1] = end - start;
            printf("Probe sequence numer: %d. Measured RTT: %ld ms.\n", (i * n_measurements_for_size + j), rtts[i * n_measurements_for_size + j - 1]);
        }
        double average =  get_average_by_operation(operation.code, rtts, i * n_measurements_for_size, (i + 1) * n_measurements_for_size, size);
        total_average += average;
        printf("Average %s for message of size %d: %lf\n", operation.as_string, size, average);
        print_on_file(operation.as_string, size, average);
        //Bye phase
        sprintf(send_buffer, "b");
        send_return_value = send(socket_file_descriptor, send_buffer, max_size, 0);
        check_send(socket_file_descriptor, send_buffer, send_return_value);
        send_buffer = memset(send_buffer, '\0', max_size);
        
        recv_buffer = memset(recv_buffer, '\0', max_size);
        recv_message_size = recv(socket_file_descriptor, recv_buffer, max_size, 0);
        check_recv(socket_file_descriptor, recv_buffer, recv_message_size);
        if(strcmp(recv_buffer, "200 OK - Closing") != 0) {
            fprintf(stderr, "Error: recv returned wrong string: "); ff;
            print_string(stderr, recv_buffer);
            exit(EXIT_FAILURE);
        }
        close(socket_file_descriptor);
    }
    
    printf("Total Average %s: %lf %s.\n", operation.as_string, total_average / sizes_length, operation.measure_unit);
    exit(EXIT_SUCCESS);
}

double get_average_by_operation(int operation_code, long rtts[], int start, int end, int size) {
    long total_rtts = 0;
    for(int i = start; i < end; i++) {
        total_rtts += rtts[i];
    }
    double avg_rtt = (double) (total_rtts / (end - start));
    if(operation_code == RTT) {
        return avg_rtt;
    } else if(operation_code == THROUGHPUT) {
        return size / avg_rtt;
    } else {
        fprintf(stderr, "Unsupported operation.");
        exit(EXIT_FAILURE);
    }
}

char * get_string_by_length(int length) {
    char *string = NULL;
    string = (char *) malloc(length);
    if(string == NULL) {
        fprintf(stderr, "Malloc returned NULL");
        exit(EXIT_FAILURE);
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

void print_on_file(char * operation, int size, double average){
    char filename[54];
    strcpy(filename, operation);
    strcat(filename, ".txt");
    FILE * to_print_on = fopen(filename, "a+");
    if(to_print_on == NULL){
        fprintf(stderr, "File opening failed");
        exit(EXIT_FAILURE);
    }
    fprintf(to_print_on, "%d %f\n", size, average);
    fflush(to_print_on); // to make it print each time. Otherwise it will stack them up and print in reverse order
    fclose(to_print_on);
}