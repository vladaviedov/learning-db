#pragma once

#include <stdint.h>
#include "row.h"

#define PAGE_SIZE 4096
#define TABLE_MAX_PAGES 100
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_PAGES)

typedef struct {
	int fd;
	uint32_t fsize;
	void *pages[TABLE_MAX_PAGES];
} table_cache;

typedef struct {
	uint64_t row_num;
	table_cache *cache;
} table;

table *open_table(const char *file);
void close_table(table *t);
void *get_row(table *table, uint32_t num);
