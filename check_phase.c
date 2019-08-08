#include "check_phase.h"
#include <string.h>

int is_hello_phase(char *message) {
    char *protocol_phase = strtok(message, " ");
    return strcmp(protocol_phase, "h") == 0 ? 1 : 0; 
}

int is_measurement_phase(char *message) {
    char *protocol_phase = strtok(message, " ");
    return strcmp(protocol_phase, "m") == 0 ? 1 : 0;
}

int is_bye_phase(char *message) {
    char *protocol_phase = strtok(message, " ");
    return strcmp(protocol_phase, "b") == 0 ? 1 : 0; 
}