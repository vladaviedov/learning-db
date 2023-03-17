#include "command.h"
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT ".exit"

#define INSERT "insert"
#define SELECT "select"

result run_meta(input_buf *buffer) {
	if (strcmp(buffer->data, EXIT) == 0) {
		del_input_buf(buffer);
		exit(EXIT_SUCCESS);
	}

	return RES_UNRECOGNIZED;
}

result parse_statement(input_buf *buffer, statement *out) {
	if (strncmp(buffer->data, INSERT, 6) == 0) {
		out->type = ST_INSERT;
		return RES_SUCCESS;
	}
	if (strncmp(buffer->data, SELECT, 6) == 0) {
		out->type = ST_SELECT;
		return RES_SUCCESS;
	}

	return RES_UNRECOGNIZED;
}

void execute(statement *st) {
	switch (st->type) {
		case ST_INSERT:
			printf("do insert here\n");
			break;
		case ST_SELECT:
			printf("do select here\n");
			break;
	}
}
