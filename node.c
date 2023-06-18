#include "node.h"
#include "row.h"
#include "table.h"
#include "cursor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

internal_node *internal_init(table *t, uint32_t page, node_header *parent);
void split_insert(leaf_node *node, cursor *cur, row *value);
child get_child_meta(node_header *node);

leaf_node *leaf_init(table *t, uint32_t page, node_header *parent) {
	leaf_node *node = get_page(t->cache, page);

	// Common header
	node->header.type = NODE_LEAF;
	node->header.is_root = (parent == NULL);
	node->header.page = page;
	if (parent != NULL) {
		node->header.parent = parent->page;
	}
	// Leaf node fields
	node->cell_count = 0;
	node->next_leaf = 0;

	return node;
}

void leaf_insert(cursor *cur, uint32_t key, row *value) {
	leaf_node *node = get_page(cur->table->cache, cur->page);
	
	// Node is full
	if (node->cell_count >= LEAF_MAX_CELLS) {
		split_insert(node, cur, value);
		return;
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

uint32_t leaf_find_pos(leaf_node *node, uint32_t key) {
	uint32_t min = 0;
	// Exclusive max
	uint32_t emax = node->cell_count;

	// Binary search
	while (emax != min) {
		uint32_t i = (min + emax) / 2;
		uint32_t i_key = node->data[i].key;
		if (key == i_key) {
			return i;
		}

		if (key < i_key) {
			emax = i;
		} else {
			min = i + 1;
		}
	}

	return min;
}

leaf_node *internal_find_leaf(table_cache *cache, internal_node *node, uint32_t key) {
	uint32_t min = 0;
	uint32_t max = node->key_count;

	// Binary search
	while (min != max) {
		uint32_t i = (min + max) / 2;
		if (node->data[i].key >= key) {
			max = i;
		} else {
			min = i + 1;
		}
	}
	
	// Search child
	uint32_t child_page = (min == node->key_count) ? node->last_child : node->data[min].page;
	node_header *child = get_page(cache, child_page);
	if (child->type == NODE_LEAF) {
		return (leaf_node *)child;
	} else {
		return internal_find_leaf(cache, (internal_node *)child, key);
	}
}

/* Internal */

internal_node *internal_init(table *t, uint32_t page, node_header *parent) {
	internal_node *node = get_page(t->cache, page);

	// Common header
	node->header.type = NODE_INTERNAL;
	node->header.is_root = (parent == NULL);
	node->header.page = page;
	if (parent != NULL) {
		node->header.parent = parent->page;
	}
	// Internal node fields
	node->key_count = 0;

	return node;
}

void split_insert(leaf_node *old_node, cursor *cur, row *value) {
	// Split items between nodes
	leaf_node *new_node = leaf_init(cur->table, new_page_idx(cur->table->cache), NULL);
	for (uint32_t i = 0; i <= LEAF_MAX_CELLS; i++) {
		leaf_node *target_node = (i >= LEAF_SPLIT_LEFT) ? new_node : old_node;
		cell *dest = target_node->data + i % LEAF_SPLIT_LEFT;

		if (i == cur->cell) {
			dest->key = value->id;
			serialize(value, dest->value);
		} else if (i > cur->cell) {
			memcpy(dest, old_node->data + i - 1, sizeof(cell));
		} else {
			memcpy(dest, old_node->data + i, sizeof(cell));
		}
	}

	// Update counts
	old_node->cell_count = LEAF_SPLIT_LEFT;
	new_node->cell_count = LEAF_SPLIT_RIGHT;

	// Attach to parent
	if (old_node->header.is_root) {
		// Move old node to new node
		uint32_t old_page_id = old_node->header.page;
		uint32_t new_page_id = new_page_idx(cur->table->cache);
		leaf_node *copy = get_page(cur->table->cache, new_page_id);
		memcpy(copy, old_node, PAGE_SIZE);
		// Update metadata
		copy->header.parent = old_page_id;
		copy->header.is_root = 0;
		copy->header.page = new_page_id;

		// Update pointer
		old_node = copy;
		
		// Create root node
		internal_node *new_root = internal_init(cur->table, old_page_id, NULL);
		new_root->key_count = 1;
		new_root->data[0] = get_child_meta(&copy->header);
		new_root->last_child = new_node->header.page;
	} else {
		// TODO: implement
		fprintf(stderr, "not implemented\n");
		exit(EXIT_FAILURE);
	}

	// Update connections
	new_node->next_leaf = old_node->next_leaf;
	old_node->next_leaf = new_node->header.page;
}

child get_child_meta(node_header *node) {
	child metadata;
	metadata.page = node->page;

	if (node->type == NODE_LEAF) {
		leaf_node *full_node = (leaf_node *)node;
		metadata.key = full_node->data[full_node->cell_count - 1].key;
	} else {
		internal_node *full_node = (internal_node *)node;
		metadata.key = full_node->data[full_node->key_count - 1].key;
	}

	return metadata;
}
