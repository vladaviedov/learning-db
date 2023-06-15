#pragma once

#include <stdint.h>

#include "row.h"
#include "table.h"
#include "cursor.h"

typedef enum {
	NODE_INTERNAL,
	NODE_LEAF
} ntype;

typedef struct node_header {
	ntype type;
	uint8_t is_root;
	uint32_t page;
	uint32_t parent;
} node_header;

// Leaf node storage cell
typedef struct {
	uint32_t key;
	uint8_t value[ROW_SIZE];
} cell;

// Leaf memory layout
#define LEAF_CELL_MEM (PAGE_SIZE - sizeof(node_header) - sizeof(uint32_t))
#define LEAF_MAX_CELLS (LEAF_CELL_MEM / sizeof(cell))
#define LEAF_PADDING (LEAF_CELL_MEM % sizeof(cell))

// Leaf split counts
#define LEAF_SPLIT_RIGHT ((LEAF_MAX_CELLS + 1) / 2)
#define LEAF_SPLIT_LEFT ((LEAF_MAX_CELLS + 1) - LEAF_SPLIT_RIGHT)

// Must be 4 kB
typedef struct {
	node_header header;
	uint32_t cell_count;
	cell data[LEAF_MAX_CELLS]; 
	uint8_t padding[LEAF_PADDING];
} leaf_node;

// Internal node child cell
typedef struct {
	uint32_t page;
	uint32_t key;
} child;

// Internal memory layout
#define INTERNAL_CHILD_MEM (PAGE_SIZE - sizeof(node_header) - sizeof(uint32_t) * 2)
#define INTERNAL_MAX_CHILDREN (INTERNAL_CHILD_MEM / sizeof(child))
#define INTERNAL_PADDING (INTERNAL_CHILD_MEM % sizeof(child))

// Must be 4 kB
typedef struct {
	node_header header;
	uint32_t key_count;
	uint32_t last_child;
	child data[INTERNAL_MAX_CHILDREN];
	uint8_t padding[INTERNAL_PADDING];
} internal_node;

leaf_node *leaf_init(table *t, uint32_t page, node_header *parent);
void leaf_insert(cursor *cur, uint32_t key, row *value);
uint32_t leaf_find_pos(leaf_node *node, uint32_t key);
