#define ff fflush(stdout)

void print_string(FILE *file, char *string);

int max_array(int array[], int array_length);

void check_allocation(void *pointer);

void log_on_file(int packet_size, int rtt, char path[]);