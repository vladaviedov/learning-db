#pragma once

#include "table.h"

typedef struct {
	table *table;
	uint32_t row;
	int end;
} cursor;

cursor *table_start(table *t);
cursor *table_end(table *t);
void *cursor_value(cursor *cur);
void cursor_inc(cursor *cur);
