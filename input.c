#include "input.h"

#include <stdio.h>
#include <stdlib.h>

input_buf *new_input_buf() {
	input_buf *buffer = malloc(sizeof(input_buf));
	buffer->data = NULL;
	buffer->data_size = 0;
	buffer->alloc_size = 0;

	return buffer;
}

void del_input_buf(input_buf *buffer) {
	if (buffer->data != NULL) {
		free(buffer->data);
	}

	free(buffer);
}

void prompt(input_buf *buffer) {
	printf("[db]> ");

	uint64_t bytes = getline(&buffer->data, &buffer->alloc_size, stdin);
	if (bytes <= 0) {
		fprintf(stderr, "failed to read input\n");
		exit(EXIT_FAILURE);
	}

	buffer->data_size = bytes - 1;
	buffer->data[buffer->data_size] = '\0';
}
