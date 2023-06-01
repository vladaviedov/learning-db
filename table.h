#pragma once

#include <stdint.h>

#include "row.h"

#define PAGE_SIZE 4096
#define TABLE_MAX_PAGES 100

typedef struct {
	int fd;
	uint32_t fsize;
	uint32_t num_pages;
	void *pages[TABLE_MAX_PAGES];
} table_cache;

typedef struct {
	table_cache *cache;
	uint32_t root_page;
} table;

table *open_table(const char *file);
void close_table(table *t);
void *get_page(table_cache *cache, uint32_t num);
