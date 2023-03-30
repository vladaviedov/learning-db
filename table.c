#include "table.h"
#include "row.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

table_cache *open_cache(const char *file);
void flush_page(table_cache *cache, uint32_t num, uint32_t size);

table *open_table(const char *file) {
	table *t = malloc(sizeof(table));
	t->cache = open_cache(file);
	t->row_num = t->cache->fsize / ROW_SIZE;

	return t;
}

void close_table(table *t) {
	table_cache *cache = t->cache;
	uint32_t full_num = t->row_num / ROWS_PER_PAGE;

	// Write full pages
	for (uint32_t i = 0; i < full_num; i++) {
		if (cache->pages[i] == NULL) {
			continue;
		}

		flush_page(cache, i, PAGE_SIZE);
	}

	// Write partial pages
	uint32_t extra_rows = t->row_num % ROWS_PER_PAGE;
	if (extra_rows > 0 && cache->pages[full_num] != NULL) {
		flush_page(cache, full_num, extra_rows * ROW_SIZE);
	}

	// Close page
	if (close(cache->fd) < 0) {
		fprintf(stderr, "failed to close file\n");
		exit(EXIT_FAILURE);
	}
	
	// Cleanup
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		void *page = cache->pages[i];
		if (page != NULL) {
			free(page);
		}
	}
	free(cache);
	free(t);
}

void *get_page(table_cache *cache, uint32_t num) {
	if (num > TABLE_MAX_PAGES) {
		fprintf(stderr, "tried to access outside bounds\n");
		exit(EXIT_FAILURE);
	}

	// Cache miss
	if (cache->pages[num] == NULL) {
		void *page = malloc(PAGE_SIZE);
		uint32_t saved_num = cache->fsize / PAGE_SIZE;

		// Partial pages
		if (cache->fsize % PAGE_SIZE) {
			saved_num++;
		}

		// If already exists, load from file
		if (num <= saved_num) {
			lseek(cache->fd, num * PAGE_SIZE, SEEK_SET);
			int64_t bytes = read(cache->fd, page, PAGE_SIZE);
			if (bytes < 0) {
				fprintf(stderr, "failed to read file\n");
				exit(EXIT_FAILURE);
			}
		}

		cache->pages[num] = page;
	}

	return cache->pages[num];
}

/* Cache */

table_cache *open_cache(const char *file) {
	int fd = open(file, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
	if (fd < 0) {
		fprintf(stderr, "failed to open file\n");
		exit(EXIT_FAILURE);
	}

	table_cache *cache = malloc(sizeof(table_cache));
	cache->fd = fd;
	cache->fsize = lseek(fd, 0, SEEK_END);

	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		cache->pages[i] = NULL;
	}

	return cache;
}

void flush_page(table_cache *cache, uint32_t num, uint32_t size) {
	if (cache->pages[num] == NULL) {
		fprintf(stderr, "failed to flush: null page\n");
		exit(EXIT_FAILURE);
	}

	int32_t offset = lseek(cache->fd, num * PAGE_SIZE, SEEK_SET);
	if (offset < 0) {
		fprintf(stderr, "failed to flush: seek failed\n");
		exit(EXIT_FAILURE);
	}

	int64_t bytes = write(cache->fd, cache->pages[num], size);
	if (bytes < 0) {
		fprintf(stderr, "failed to flush: write failed\n");
		exit(EXIT_FAILURE);
	}
}
