#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void leaf_init(leaf_node *node, node_header *parent) {
	// Common header
	node->header.type = NODE_LEAF;
	node->header.parent = parent;
	node->header.is_root = (parent == NULL);
	// Leaf node fields
	node->cell_count = 0;
}

void leaf_insert(cursor *cur, uint32_t key, row *value) {
	leaf_node *node = get_page(cur->table->cache, cur->page);
	
	// Node is full
	if (node->cell_count >= LEAF_MAX_CELLS) {
		// TODO: implement multinode trees
		fprintf(stderr, "multiple nodes are not implemented yet\n");
		exit(EXIT_FAILURE);
	}

	// Inserting in the middle, move bigger elements
	if (cur->cell < node->cell_count) {
		for (uint32_t i = node->cell_count; i > cur->cell; i--) {
			memcpy(node->data + i, node->data + i - 1, sizeof(cell));
		}
	}

	// Insert row
	node->cell_count++;
	node->data[cur->cell].key = key;
	serialize(value, node->data[cur->cell].value);
}
