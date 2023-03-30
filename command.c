#include "command.h"
#include "input.h"
#include "table.h"
#include "util.h"
#include "row.h"
#include "cursor.h"

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
		
		unused char *key = strtok(buffer->data, " ");
		char *id_str = strtok(NULL, " ");
		char *username = strtok(NULL, " ");
		char *email = strtok(NULL, " ");

		// Validate null
		if (id_str == NULL || username == NULL || email == NULL) {
			fprintf(stderr, "bad arguments\n");
			return RES_FAILED;
		}

		// Validate size
		if (strlen(username) > COL_USERNAME_SIZE) {
			fprintf(stderr, "username too long\n");
			return RES_FAILED;
		}
		if (strlen(email) > COL_EMAIL_SIZE) {
			fprintf(stderr, "email too long\n");
			return RES_FAILED;
		}

		// Validate id
		int id = atoi(id_str);
		if (id < 0) {
			fprintf(stderr, "id cannot be negative\n");
			return RES_FAILED;
		}

		out->insert_row.id = id;
		strncpy(out->insert_row.username, username, COL_USERNAME_SIZE);
		strncpy(out->insert_row.email, email, COL_EMAIL_SIZE);

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

	cursor *end = table_end(t);
	serialize(&st->insert_row, cursor_value(end));
	t->row_num++;

	free(end);
	return RES_SUCCESS;
}

result execute_select(unused statement *st, table *t) {
	cursor *cur = table_start(t);

	row row;
	while (!cur->end) {
		deserialize(cursor_value(cur), &row);
		print_row(&row);
		cursor_inc(cur);
	}

	return RES_SUCCESS;
}

void print_row(row *r) {
	printf("%u\t\t%s\t\t%s\n",
		r->id,
		r->username,
		r->email);
}
