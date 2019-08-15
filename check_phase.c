#include "check_phase.h"

#include <string.h>
#include <stdio.h>

int is_hello_phase(char *message, int msg_length) {
    char copy_of_message[msg_length];
    strcpy(copy_of_message, message);
    char *protocol_phase = strtok(copy_of_message, " ");
    return strcmp(protocol_phase, "h") == 0 ? 1 : 0; 
}

int is_measurement_phase(char *message, int msg_length) {
    char copy_of_message[msg_length];
    strcpy(copy_of_message, message);
    char *protocol_phase = strtok(copy_of_message, " ");
    return strcmp(protocol_phase, "m") == 0 ? 1 : 0;
}

int is_bye_phase(char *message, int msg_length) {
    char copy_of_message[msg_length];
    strcpy(copy_of_message, message);
    char *protocol_phase = strtok(copy_of_message, " ");
    return strcmp(protocol_phase, "b") == 0 ? 1 : 0; 
}

int is_request_uninitialized(hello_message message) {
    if(message.measure_type[0] == '\0' && message.msg_size == 0 
        && message.n_probes == 0 && message.server_delay == 0 && message.probes_counted == 0) {
        return 1;
    }
    return 0;
}