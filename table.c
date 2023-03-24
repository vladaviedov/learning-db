#include "table.h"

#include <stdlib.h>

table *create_table() {
	table *t = malloc(sizeof(table));
	t->row_num = 0;
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		t->pages[i] = NULL;
	}

	return t;
}

void destroy_table(table *t) {
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		if (t->pages[i] != NULL) {
			free(t->pages[i]);
		}
	}

	free(t);
}

void *get_row(table *table, uint32_t num) {
	uint32_t page_num = num / ROWS_PER_PAGE;
	void *page = table->pages[page_num];

	if (page == NULL) {
		table->pages[page_num] = malloc(PAGE_SIZE);
		page = table->pages[page_num];
	}

	uint32_t offset = (num % ROWS_PER_PAGE) * ROW_SIZE;
	return page + offset;
}
