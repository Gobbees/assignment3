#include <string.h>
#include <stdio.h>
#include "check_phase.h"

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
    hello_message uninitialized_message = get_uninitialized_hello_message();
    if(strcmp(message.measure_type, uninitialized_message.measure_type) == 0 && message.msg_size == uninitialized_message.msg_size 
        && message.n_probes == uninitialized_message.n_probes && message.server_delay == uninitialized_message.server_delay 
        && message.next_probe == uninitialized_message.next_probe) {
        return 1;
    }
    return 0;
}