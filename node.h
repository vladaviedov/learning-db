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
	struct node_header *parent;
} node_header;

typedef struct {
	uint32_t key;
	uint8_t value[ROW_SIZE];
} cell;

#define LEAF_CELL_MEM (PAGE_SIZE - sizeof(node_header) - sizeof(uint32_t))
#define LEAF_MAX_CELLS (LEAF_CELL_MEM / sizeof(cell))
#define LEAF_PADDING (LEAF_CELL_MEM % sizeof(cell))

// Must be 4 kB
typedef struct {
	node_header header;
	uint32_t cell_count;
	cell data[LEAF_MAX_CELLS]; 
	uint8_t padding[LEAF_PADDING];
} leaf_node;

void leaf_init(leaf_node *node, node_header *parent);
void leaf_insert(cursor *cur, uint32_t key, row *value);
