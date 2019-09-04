#define ff fflush(stdout)

/**
 * Prints a string on file.
 * - file: the file
 * - string: the string
 */
void print_string(FILE *file, char *string);

/**
 * Returns the max of the array.
 * - array: the array
 * - array_length: the array length
 */
int max_array(int array[], int array_length);

/**
 * Checks if pointer is NULL. If it is, it exits returning EXIT_FAILURE
 */
void check_allocation(void *pointer);

/**
 * Checks if the send operation succeeded.
 * If yes, it prints the output message.
 * Otherwise, it exits returning EXIT_FAILURE.
 */
void check_send(int socket_file_descriptor, char *send_message, ssize_t send_return_value);
/**
 * Checks if the recv operation succeeded.
 * If yes, it prints the output message.
 * Otherwise, it exits returning EXIT_FAILURE.
 */
void check_recv(int socket_file_descriptor, char *recv_message, ssize_t recv_return_value);

/**
 * TODO
 */
void log_on_file(int packet_size, int rtt, char path[]);