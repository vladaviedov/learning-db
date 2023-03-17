#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *data;
	uint64_t alloc_size;
	uint64_t data_size;
} input_buf;

input_buf *new_input_buf() {
	input_buf *buffer = malloc(sizeof(input_buf));
	buffer->data = NULL;
	buffer->data_size = 0;
	buffer->alloc_size = 0;

	return buffer;
}

void del_input_buf(input_buf *buf) {
	if (buf->data != NULL) {
		free(buf->data);
	}

	free(buf);
}

void prompt(input_buf *buffer) {
	printf("[db]> ");

	ssize_t bytes = getline(&buffer->data, &buffer->alloc_size, stdin);
	if (bytes <= 0) {
		fprintf(stderr, "failed to read input\n");
		exit(EXIT_FAILURE);
	}

	buffer->data_size = bytes - 1;
	buffer->data[buffer->data_size] = '\0';
}

int main(int argc, char **argv) {
	input_buf *input = new_input_buf();
	while (1) {
		prompt(input);

		if (strcmp(input->data, ".exit") == 0) {
			del_input_buf(input);
			return 0;
		}

		printf("unrecognized option\n");
	}
}
