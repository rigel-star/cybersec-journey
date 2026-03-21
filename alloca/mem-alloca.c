#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

struct mem_block_hdr {
    size_t 						size;
    uint32_t 					is_free;
    uint32_t 					canary;
    struct mem_block_hdr*		next;
};

#define META_SIZE sizeof(struct mem_block_hdr)

struct mem_block_hdr* global_base = NULL;

struct mem_block_hdr* find_free_block(struct mem_block_hdr** last, size_t size) {
	struct mem_block_hdr* current = global_base;
	while (current && !(current->is_free && current->size >= size)) {
		*last = current;
		current = current->next;
	}

	return NULL;
}

struct mem_block_hdr* request_space(struct mem_block_hdr* last, size_t size) {
	struct mem_block_hdr* block = sbrk(0);
	void* request = sbrk(size + META_SIZE);

	assert((void*)block == request);

	if (request == (void*)-1) {
		return NULL; // sbrk() failed
	}

	if (last) {
		last->next = block;
	}

	block->size = size;
	block->is_free = 0;
	block->next = NULL;
	block->canary = 0x12345678;
	return block;
}

void* arena_alloc(size_t size) {
	struct mem_block_hdr* block;
	if (size <=0 ) return NULL;

	if (!global_base) {
		block = request_space(NULL, size);
		if (!block) {
			return NULL;
		}
		global_base = block;
	}
	else {
		struct mem_block_hdr* last = global_base;
		block = find_free_block(&last, size);

		if (!block) {
			block = request_space(last, size);
			if (!block) {
				return NULL;
			}
		}
		else {
			block->is_free = 0;
			block->canary = 0x77777777;
		}
	}

	return block + 1;
}

struct mem_block_hdr* get_block_ptr(void *ptr) {
  	return (struct mem_block_hdr*)ptr - 1;
}

void arena_free(void* ptr) {
  	if (!ptr) {
    	return;
  	}

  	struct mem_block_hdr* block_ptr = get_block_ptr(ptr);
  	
	if (block_ptr->is_free == 1) {
		fprintf(stderr, "fatal: double-free detected\n");
		exit(-1);
	}

  	assert(block_ptr->canary == 0x77777777 || block_ptr->canary == 0x12345678);

  	block_ptr->is_free = 1;
  	block_ptr->canary = 0x55555555;
}

int main(void) {
	uint32_t* items = arena_alloc(sizeof(uint32_t) * 5);
	uint32_t* items2 = arena_alloc(sizeof(uint32_t) * 5);

	if (!items) {
		fprintf(stderr, "arena_alloc failed");
		return -1;
	}

	*(items) = 1234;
	*(items + 1) = 12345;
	*(items + 2) = 123456;

	printf("%d\n", *items);
	printf("%d\n", *(items + 1));
	printf("%d\n", *(items + 2));

	arena_free(items2);
	*(items + 4 + sizeof(size_t)) = 0;
	arena_free(items2);

	return 0;
}
