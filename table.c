#include "table.h"
#include "row.h"
#include "node.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

table_cache *open_cache(const char *file);
void flush_page(table_cache *cache, uint32_t num);

table *open_table(const char *file) {
	table *t = malloc(sizeof(table));

	t->cache = open_cache(file);
	t->root_page = 0;

	// New database
	if (t->cache->num_pages == 0) {
		// Create root lead node
		leaf_node *root = get_page(t->cache, 0);
		leaf_init(root, NULL);
	}

	return t;
}

void close_table(table *t) {
	table_cache *cache = t->cache;

	// Write pages
	for (uint32_t i = 0; i < cache->num_pages; i++) {
		if (cache->pages[i] == NULL) {
			continue;
		}

		flush_page(cache, i);
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

		// Update number of pages
		if (num >= cache->num_pages) {
			cache->num_pages = num + 1;
		}
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
	cache->num_pages = cache->fsize / PAGE_SIZE;

	if (cache->fsize % PAGE_SIZE != 0) {
		fprintf(stderr, "db file is corrupt\n");
		exit(EXIT_FAILURE);
	}
	
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		cache->pages[i] = NULL;
	}

	return cache;
}

void flush_page(table_cache *cache, uint32_t num) {
	if (cache->pages[num] == NULL) {
		fprintf(stderr, "failed to flush: null page\n");
		exit(EXIT_FAILURE);
	}

	int32_t offset = lseek(cache->fd, num * PAGE_SIZE, SEEK_SET);
	if (offset < 0) {
		fprintf(stderr, "failed to flush: seek failed\n");
		exit(EXIT_FAILURE);
	}

	int64_t bytes = write(cache->fd, cache->pages[num], PAGE_SIZE);
	if (bytes < 0) {
		fprintf(stderr, "failed to flush: write failed\n");
		exit(EXIT_FAILURE);
	}
}
