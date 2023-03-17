#pragma once

#include <stdint.h>

typedef struct {
	char *data;
	uint64_t alloc_size;
	uint64_t data_size;
} input_buf;

input_buf *new_input_buf();
void del_input_buf(input_buf *buffer);
void prompt(input_buf *buffer);
