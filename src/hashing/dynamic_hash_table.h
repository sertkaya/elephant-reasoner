/*
 * The ELepHant Reasoner
 *
 * Copyright (C) Baris Sertkaya (sertkaya@fb2.fra-uas.de)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DYNAMIC_HASH_TABLE_H_
#define DYNAMIC_HASH_TABLE_H_

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"

/**
 * A hash table interface for storing keys only, no associated values. Keys are of type uint32_t.
 * They cannot be -1.
 */

typedef struct dynamic_hash_table DynamicHashTable;
typedef struct dynamic_hash_table_iterator DynamicHashTableIterator;

// TODO: !!
#define HASH_TABLE_ZERO_KEY		0
#define HASH_TABLE_EMPTY_KEY		-1
#define HASH_TABLE_DELETED_KEY		-2

/**
 * Hash table.
 */
struct dynamic_hash_table {
	uint32_t* elements;			// the buckets
	unsigned int* end_indexes;	// the end indexes of chains
	unsigned int size;			// size of the hash table
	unsigned int element_count;	// the number of elements
};

/**
 * Iterator for hash table.
 */
struct dynamic_hash_table_iterator {
	DynamicHashTable* hash_table;
	unsigned int current_index;	// index of the current slot
};

DynamicHashTable* dynamic_hash_table_create(unsigned int size);

/**
 * Initialize a hash table with a given initial size.
 */
void dynamic_hash_table_init(DynamicHashTable* hash_table, unsigned int size);

/**
 * Free the space for the given hash table.
 */
int dynamic_hash_table_free(DynamicHashTable* hash_table);

/**
 * Free the space for the elements of the given hash table. Intended for
 * hash tables that are not dynamically created, but only initialized.
 */
int dynamic_hash_table_reset(DynamicHashTable* hash_table);

/**
 * Insert the given key into the hash table. The key cannot be NULL.
 * Returns 1 if the key is inserted, 0 if it already existed.
 */
inline char dynamic_hash_table_insert(uint32_t key, DynamicHashTable* hash_table) {
	int i, j, new_size;
	size_t start_index;

	assert(key != HASH_TABLE_EMPTY_KEY && key != HASH_TABLE_DELETED_KEY && key != HASH_TABLE_ZERO_KEY);

	// start_index = HASH_POINTER(key) & (hash_table->size - 1);
	start_index = HASH_UNSIGNED(key) & (hash_table->size - 1);
	for (i = start_index; ; i = (i + 1) & (hash_table->size - 1)) {
		if (hash_table->elements[i] == key)
			// the key already exists
			return 0;

		if (hash_table->elements[i] == HASH_TABLE_EMPTY_KEY) {
			// an empty slot is found, insert the key here
			hash_table->elements[i] = key;
			++hash_table->element_count;
			// mark the new end index
			hash_table->end_indexes[start_index] = (i + 1) & (hash_table->size - 1);
			break;
		}
	}

	// check if we need to resize. load factor 0.75
	if (hash_table->element_count * 4 >= hash_table->size * 3) {
		// the load factor is reached, resize
		new_size = 2 * hash_table->size;
		uint32_t* tmp_elements = (uint32_t*) calloc(new_size, sizeof(uint32_t));
		assert(tmp_elements != NULL);

		// free the end indexes, allocate and initialize new
		free(hash_table->end_indexes);
		hash_table->end_indexes = (unsigned int*) calloc(new_size, sizeof(unsigned int));
		assert(hash_table->end_indexes != NULL);
		for (i = 0; i < new_size; ++i) {
			// initialize the elements with HASH_TABLE_EMPTY_KEY
			hash_table->elements[i] = HASH_TABLE_EMPTY_KEY;
			hash_table->end_indexes[i] = i;
		}

		// re-populate
		for (i = 0; i < hash_table->size; ++i) {
			// TODO: Check if commenting it out caused malfunctioning
			// if (hash_table->elements[i] != NULL) {
				start_index = HASH_UNSIGNED(hash_table->elements[i]) & (new_size - 1);
				for (j = start_index; ; j = (j + 1) & (new_size - 1))
					// if (tmp_elements[j] == HASH_TABLE_ZERO_KEY) {
					if (tmp_elements[j] == HASH_TABLE_EMPTY_KEY) {
						tmp_elements[j] = hash_table->elements[i];
						break;
					}
				hash_table->end_indexes[start_index] = (j + 1) & (new_size - 1);
			// }
		}

		// change the size, the element count does not change
		hash_table->size = new_size;

		// free the existing elements
		free(hash_table->elements);

		// the new elements
		hash_table->elements = tmp_elements;
	}

	// to suppress warnings
	return 1;
}


/**
 * Removes a given key from the given hash table. The slot is marked as empty afterwards.
 * We do not shrink the allocated space after a removal.
 * Returns 1 if the key is removed, 0 otherwise.
 */
inline char dynamic_hash_table_remove(uint32_t key, DynamicHashTable* hash_table) {
	assert(key != HASH_TABLE_EMPTY_KEY && key != HASH_TABLE_DELETED_KEY && key != HASH_TABLE_ZERO_KEY);

	int i;
	size_t start_index = HASH_POINTER(key) & (hash_table->size - 1);

	for (i = start_index; i != hash_table->end_indexes[start_index]; i = (i + 1) & (hash_table->size - 1)) {
		if (hash_table->elements[i] == key) {
			// key found, mark it as empty
			hash_table->elements[i] = HASH_TABLE_DELETED_KEY;
			return 1;
		}
	}
	return 0;
}


/**
 * Check whether the given key exists in the hash table. The key cannot be NULL.
 * Returns 1 if so, 0 otherwise.
 */
inline char dynamic_hash_table_contains(uint32_t key, DynamicHashTable* hash_table) {
	assert(key != HASH_TABLE_EMPTY_KEY && key != HASH_TABLE_DELETED_KEY);
	size_t start_index = HASH_POINTER(key) & (hash_table->size - 1);

	int i;
	for (i = start_index; i != hash_table->end_indexes[start_index]; i = (i + 1) & (hash_table->size - 1)) {
		if (hash_table->elements[i] == key)
			return 1;
	}
	return 0;
}

inline DynamicHashTableIterator* dynamic_hash_table_iterator_create(DynamicHashTable* hash_table) {
	DynamicHashTableIterator* iterator = (DynamicHashTableIterator*) malloc(sizeof(DynamicHashTableIterator));
	assert(iterator != NULL);
	iterator->hash_table = hash_table;
	iterator->current_index = 0;

	return iterator;
}

inline void dynamic_hash_table_iterator_init(DynamicHashTableIterator* iterator, DynamicHashTable* hash_table) {

	iterator->hash_table = hash_table;
	iterator->current_index = 0;

	return;
}

inline uint32_t dynamic_hash_table_iterator_next(DynamicHashTableIterator* iterator) {

	int i;
	for (i = iterator->current_index; i < iterator->hash_table->size; ++i)
		if (iterator->hash_table->elements[i] != HASH_TABLE_EMPTY_KEY && iterator->hash_table->elements[i] != HASH_TABLE_DELETED_KEY) {
			iterator->current_index = i + 1;
			return iterator->hash_table->elements[i];
		}

	return HASH_TABLE_ZERO_KEY;
}

/**
 * Free the space allocated for a hash table iterator.
 * Returns the number of freed bytes.
 */
int dynamic_hash_table_iterator_free(DynamicHashTableIterator* iterator);

#endif
