#pragma once

#include "input.h"
#include "row.h"
#include "table.h"

typedef enum {
	RES_SUCCESS,
	RES_FAILED,
	RES_UNRECOGNIZED,
	RES_FULL,
	RES_EXIT
} result;

typedef enum {
	ST_INSERT,
	ST_SELECT
} statement_type;

typedef struct {
	statement_type type;
	row insert_row;
} statement;

result run_meta(input_buf *buffer, table *t);
result parse_statement(input_buf *buffer, statement *out);
result execute(statement *st, table *t);
