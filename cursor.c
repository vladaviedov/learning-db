#include "cursor.h"
#include "row.h"
#include "table.h"

#include <stdlib.h>

cursor *table_start(table *t) {
	cursor *cur = malloc(sizeof(cursor));

	cur->table = t;
	cur->row = 0;
	cur->end = (t->row_num == 0);

	return cur;
}

cursor *table_end(table *t) {
	cursor *cur = malloc(sizeof(cursor));

	cur->table = t;
	cur->row = t->row_num;
	cur->end = 1;

	return cur;
}

void *cursor_value(cursor *cur) {
	uint32_t page_num = cur->row / ROWS_PER_PAGE;
	void *page = get_page(cur->table->cache, page_num);

	uint32_t offset = (cur->row % ROWS_PER_PAGE) * ROW_SIZE;
	return page + offset;
}

void cursor_inc(cursor *cur) {
	cur->row++;
	if (cur->row >= cur->table->row_num) {
		cur->end = 1;
	}
}
