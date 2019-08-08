/**
 * 
 */
typedef struct {
    char *measure_type;
    int n_probes;
    int msg_size;
    int server_delay;
} hello_message;

/**
 * 
 */
typedef struct {
    int probe_seq_num;
    char *payload;
} measurement_message;

/**
 * 
 */
int parse_hello_message(char *buffer, hello_message *message);

/**
 * 
 */
int parse_and_check_measurement_message(char *buffer, hello_message request, measurement_message *message);