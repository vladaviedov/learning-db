#pragma once

#include "input.h"

typedef enum {
	RES_SUCCESS,
	RES_FAILED,
	RES_UNRECOGNIZED
} result;

typedef enum {
	ST_INSERT,
	ST_SELECT
} statement_type;

typedef struct {
	statement_type type;
} statement;

result run_meta(input_buf *buffer);
result parse_statement(input_buf *buffer, statement *out);
void execute(statement *st);
