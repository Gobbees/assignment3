/**
 * Returns:
 * -1 if the message describes an Hello Phase ("h" as protocol_phase)
 * -0 otherwise
 */
int is_hello_phase(char *message);

/**
 * Returns:
 * -1 if the message describes a Management Phase ("m" as protocol_phase)
 * -0 otherwise
 */
int is_measurement_phase(char *message);

/**
 * Returns:
 * -1 if the message describes a Bye Phase ("b" as protocol_phase)
 * -0 otherwise
 */
int is_bye_phase(char *message);