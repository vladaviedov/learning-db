#include "cursor.h"
#include "row.h"
#include "table.h"
#include "node.h"

#include <stdlib.h>

cursor *table_find(table *t, uint32_t key) {
	cursor *cur = malloc(sizeof(cursor));

	cur->table = t;
	cur->page = t->root_page;
	
	node_header *root = get_page(t->cache, t->root_page);
	if (root->type == NODE_LEAF) {
		leaf_node *leaf_root = (leaf_node *)root;
		cur->cell = leaf_find_pos(leaf_root, key);
		cur->end = (cur->cell == leaf_root->cell_count && leaf_root->next_leaf == 0);
	} else {
		leaf_node *target_node = internal_find_leaf(t->cache, (internal_node *)root, key);
		cur->page = target_node->header.page;
		cur->cell = leaf_find_pos(target_node, key);
		cur->end = (cur->cell == target_node->cell_count && target_node->next_leaf == 0);
	}

	return cur;
}

cursor *table_start(table *t) {
	return table_find(t, 0);
}

void *cursor_value(cursor *cur) {
	leaf_node *node = get_page(cur->table->cache, cur->page);
	return node->data[cur->cell].value;
}

void cursor_inc(cursor *cur) {
	leaf_node *node = get_page(cur->table->cache, cur->page);
	cur->cell++;
	if (cur->cell >= node->cell_count) {
		if (node->next_leaf == 0) {
			cur->end = 1;
		} else {
			cur->page = node->next_leaf;
			cur->cell = 0;
		}
	}
}
