#include "cursor.h"
#include "row.h"
#include "table.h"
#include "node.h"

#include <stdlib.h>

cursor *table_start(table *t) {
	cursor *cur = malloc(sizeof(cursor));

	cur->table = t;
	cur->page = t->root_page;
	cur->cell = 0;

	// Retrieve root node
	leaf_node *root = get_page(t->cache, t->root_page);

	cur->end = (root->cell_count == 0);

	return cur;
}

cursor *table_end(table *t) {
	cursor *cur = malloc(sizeof(cursor));

	cur->table = t;
	cur->page = t->root_page;
	cur->end = 1;

	// Retrieve root node
	leaf_node *root = get_page(t->cache, t->root_page);
	cur->cell = root->cell_count;

	return cur;
}

void *cursor_value(cursor *cur) {
	leaf_node *node = get_page(cur->table->cache, cur->page);
	return node->data[cur->cell].value;
}

void cursor_inc(cursor *cur) {
	leaf_node *node = get_page(cur->table->cache, cur->page);
	cur->cell++;
	if (cur->cell >= node->cell_count) {
		cur->end = 1;
	}
}
