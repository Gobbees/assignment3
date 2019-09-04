/**
 * The struct that defines an hello message.
 */
typedef struct {
    char measure_type[6];
    int n_probes;
    int msg_size;
    int server_delay;
    int next_probe;
} hello_message;

/**
 * The struct that defines a measurement message.
 */
typedef struct {
    int probe_seq_num;
    char *payload;
} measurement_message;

/**
 * Parses and creates an hello message.
 * - buffer: the string that will be parsed
 * - message: the variable that will contain the hello_message if it will be computed correctly
 */
int parse_hello_message(char *buffer, hello_message *message);

/**
 * Returns an unitialized hello message.
 */
hello_message get_uninitialized_hello_message();

/**
 * Parses and creates a measurement message.
 * - buffer: the string that will be parsed
 * - request: the hello message request
 * - message: the variable that will contain the measurement_message if it will be computed correctly
 * Returns:
 * - 1 if the measurement_request is invalid
 * - 0 if the message is created correctly
 */
int parse_and_check_measurement_message(char *buffer, hello_message request, measurement_message *message);