#include "command.h"
#include "input.h"
#include "table.h"
#include "util.h"
#include "row.h"
#include "cursor.h"
#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT ".exit"
#define BTREE ".btree"
#define DEBUG ".debug"

#define INSERT "insert"
#define SELECT "select"

result execute_insert(statement *st, table *t);
result execute_select(statement *st, table *t);
void print_row(row *r);
void print_node(table_cache *cache, uint32_t page_num, uint32_t indent);
void print_debug();
void print_indents(uint32_t count);

result run_meta(input_buf *buffer, table *t) {
	if (strcmp(buffer->data, EXIT) == 0) {
		return RES_EXIT;
	}
	if (strcmp(buffer->data, BTREE) == 0) {
		print_node(t->cache, t->root_page, 0);
		return RES_SUCCESS;
	}
	if (strcmp(buffer->data, DEBUG) == 0) {
		print_debug();
		return RES_SUCCESS;
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
	leaf_node *node = get_page(t->cache, t->root_page);

	uint32_t key = st->insert_row.id;
	cursor *cur = table_find(t, key);
	if (cur->cell < node->cell_count) {
		if (node->data[cur->cell].key == key) {
			return RES_DUPLICATE;
		}
	}

	leaf_insert(cur, st->insert_row.id, &(st->insert_row));
	free(cur);

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

void print_node(table_cache *cache, uint32_t page_num, uint32_t indent) {
	node_header *header = get_page(cache, page_num);
	
	print_indents(indent);
	if (header->type == NODE_LEAF) {
		leaf_node *node = (leaf_node *)header;
		printf("- leaf (size %u)\n", node->cell_count);
		indent++;
		for (uint32_t i = 0; i < node->cell_count; i++) {
			print_indents(indent);
			printf("- %u\n", node->data[i].key);
		}
	} else {
		internal_node *node = (internal_node *)header;
		printf("- internal (size %u)\n", node->key_count);
		indent++;
		for (uint32_t i = 0; i < node->key_count; i++) {
			print_node(cache, node->data[i].page, indent);
			print_indents(indent);
			printf("- %u\n", node->data[i].key);
		}
		print_node(cache, node->last_child, indent);
	}
}

void print_debug() {
	printf("Debug Info Start\n\n");
	printf("ROW_SIZE: %lu\n", ROW_SIZE);
	printf("sizeof(row): %lu\n", sizeof(row));
	printf("sizeof(node_header): %lu\n", sizeof(node_header));
	printf("sizeof(leaf_node): %lu\n", sizeof(leaf_node));
	printf("sizeof(internal_node): %lu\n", sizeof(internal_node));
	printf("LEAF_CELL_MEM: %lu\n", LEAF_CELL_MEM);
	printf("LEAF_MAX_CELLS: %lu\n", LEAF_MAX_CELLS);
	printf("LEAF_PADDING: %lu\n", LEAF_PADDING);
	printf("INTERNAL_CHILD_MEM: %lu\n", INTERNAL_CHILD_MEM);
	printf("INTERNAL_MAX_CHILDREN: %lu\n", INTERNAL_MAX_CHILDREN);
	printf("INTERNAL_PADDING: %lu\n", INTERNAL_PADDING);
	printf("\nDebug Info End\n");
}

void print_indents(uint32_t count) {
	for (uint32_t i = 0; i < count; i++) {
		printf("\t");
	}
}
