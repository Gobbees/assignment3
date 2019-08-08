#include "parse_message.h"
#include <stdio.h>
#include <string.h> //TODO return -1 instead of 1
#include <stdlib.h>

int parse_hello_message(char *buffer, hello_message *message) {
    strtok(buffer, " "); //skips the protocol_phase. Not interesting here.
    char *measure_type = strtok(NULL, " ");
    if (strcmp(measure_type, "rtt") != 0 && strcmp(measure_type, "thput") != 0) {
        perror("Invalid input message: unsupported measure_type");
        return 1;
    }
    int n_probes = atoi(strtok(NULL, " "));
    if(n_probes < 0) {
        perror("Invalid input message: n_probes must be a positive number");
        return 1;
    }
    int msg_size = atoi(strtok(NULL, " "));
    if(msg_size < 1) {
        perror("Invalid input message: msg_size must be a positive non-zero number");
        return 1;
    }
    int server_delay = atoi(strtok(NULL, " "));
    if(server_delay < 0) {
        perror("Invalid input message: server_delay must be a positive non-zero number");
        return 1;
    }
    strcpy(message->measure_type, measure_type);
    message->n_probes = n_probes;
    message->msg_size = msg_size;
    message->server_delay = server_delay;

    return 0;
}

int parse_and_check_measurement_message(char *buffer, hello_message request, measurement_message *message) {
    strtok(buffer, " "); //skips the protocol_phase. Not interesting here.
    int probe_seq_num = atoi(strtok(NULL, " "));
    if(probe_seq_num < 1) {
        perror("Invalid input message: probe_seq_num must be a positive non-zero number");
        return 1;
    } else if(probe_seq_num > request.n_probes) {
        perror("Invalid input message: probe_seq_num must be less or equal than n_probes set in hello_message");
        return 1;
    }
    char *payload = strtok(NULL, " ");
    message->probe_seq_num = probe_seq_num;
    if(strlen(payload) != request.msg_size) {
        perror("Invalid input message: payload size must be exactly the specified msg_size");
        return 1;
    }
    strcpy(message->payload, payload);
    
    return 0;
}