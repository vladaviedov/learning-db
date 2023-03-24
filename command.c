#include "command.h"
#include "input.h"
#include "table.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT ".exit"

#define INSERT "insert"
#define SELECT "select"

result execute_insert(statement *st, table *t);
result execute_select(statement *st, table *t);
void print_row(row *r);

result run_meta(input_buf *buffer) {
	if (strcmp(buffer->data, EXIT) == 0) {
		return RES_EXIT;
	}

	return RES_UNRECOGNIZED;
}

result parse_statement(input_buf *buffer, statement *out) {
	if (strncmp(buffer->data, INSERT, 6) == 0) {
		out->type = ST_INSERT;
		int scanned = sscanf(buffer->data, "insert %d %s %s",
			&out->insert_row.id,
			out->insert_row.username,
			out->insert_row.email);
		if (scanned < 3) {
			return RES_FAILED;
		}

		return RES_SUCCESS;
	}
	if (strncmp(buffer->data, SELECT, 6) == 0) {
		out->type = ST_SELECT;
		return RES_SUCCESS;
	}

	return RES_UNRECOGNIZED;
}

result execute(statement *st, table *t) {
	switch (st->type) {
		case ST_INSERT:
			return execute_insert(st, t);
			break;
		case ST_SELECT:
			return execute_select(st, t);
			break;
	}

	return RES_UNRECOGNIZED;
}

// Internal

result execute_insert(statement *st, table *t) {
	if (t->row_num >= TABLE_MAX_ROWS) {
		return RES_FULL;
	}

	serialize(&st->insert_row, get_row(t, t->row_num));
	t->row_num++;

	return RES_SUCCESS;
}

result execute_select(unused statement *st, table *t) {
	row row;
	for (uint32_t i = 0; i < t->row_num; i++) {
		deserialize(get_row(t, i), &row);
		print_row(&row);
	}

	return RES_SUCCESS;
}

void print_row(row *r) {
	printf("%u\t\t%s\t\t%s\n",
		r->id,
		r->username,
		r->email);
}
