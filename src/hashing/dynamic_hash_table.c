/*
 * The ELepHant Reasoner
 *
 * Copyright (C) Baris Sertkaya (sertkaya.baris@gmail.com)
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "dynamic_hash_table.h"
#include "utils.h"

/**
 * An dynamic hash table implementation that only stores keys, no associated values.
 * It resizes if the load factor reaches 0.75
 * Keys cannot be NULL.
 */


void dynamic_hash_table_init(DynamicHashTable* hash_table, unsigned int size) {

	if (size < 8)
		size = 8;
	else
		size = roundup_pow2(size);

	hash_table->elements = (void**) calloc(size, sizeof(void*));
	assert(hash_table->elements != NULL);
	hash_table->size = size;
	hash_table->element_count = 0;

	return;
}

DynamicHashTable* dynamic_hash_table_create(unsigned int size) {
	if (size < 8)
		size = 8;
	else
		size = roundup_pow2(size);
	DynamicHashTable* hash_table = (DynamicHashTable*) malloc(sizeof(DynamicHashTable));
	assert(hash_table != NULL);

	hash_table->elements = (void**) calloc(size, sizeof(void*));
	assert(hash_table->elements != NULL);
	hash_table->end_indexes = (unsigned int*) calloc(size, sizeof(unsigned int));
	assert(hash_table->end_indexes != NULL);
	hash_table->size = size;
	hash_table->element_count = 0;
	int i;
	for (i = 0; i < hash_table->size; ++i)
		// each chain initially ends at its starting point
		hash_table->end_indexes[i] = i;

	return hash_table;
}

inline char dynamic_hash_table_insert(void* key, DynamicHashTable* hash_table) {
	int i, j, new_size;
	size_t start_index;

	assert(key != NULL);

	start_index = HASH_POINTER(key) & (hash_table->size - 1);
	for (i = start_index; ; i = (i + 1) & (hash_table->size - 1)) {
		if (hash_table->elements[i] == key)
			// the key already exists
			return 0;

		if (hash_table->elements[i] == NULL) {
			// insert the key here
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
		void **tmp_elements = (void**) calloc(new_size, sizeof(void*));
		assert(tmp_elements != NULL);

		// free the end indexes, allocate and initialize new
		free(hash_table->end_indexes);
		hash_table->end_indexes = (unsigned int*) calloc(new_size, sizeof(unsigned int));
		assert(hash_table->end_indexes != NULL);
		for (i = 0; i < new_size; ++i)
			hash_table->end_indexes[i] = i;

		// re-populate
		for (i = 0; i < hash_table->size; ++i)
			if (hash_table->elements[i] != NULL) {
				start_index = HASH_POINTER(hash_table->elements[i]) & (new_size - 1);
				for (j = start_index; ; j = (j + 1) & (new_size - 1))
					if (tmp_elements[j] == NULL) {
						tmp_elements[j] = hash_table->elements[i];
						break;
					}
				hash_table->end_indexes[start_index] = (j + 1) & (new_size - 1);
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

int dynamic_hash_table_free(DynamicHashTable* hash_table) {
	int i;
	int freed_bytes = 0;

	free(hash_table->elements);
	freed_bytes += hash_table->size * sizeof(void*);

	free(hash_table->end_indexes);
	freed_bytes += hash_table->size * sizeof(unsigned int);
<<<<<<< HEAD

	free(hash_table);
	freed_bytes += sizeof(HashTable);
=======
>>>>>>> master

	return freed_bytes;
}

inline char dynamic_hash_table_contains(void* key, DynamicHashTable* hash_table) {
	// assert(key != NULL);
	size_t start_index = HASH_POINTER(key) & (hash_table->size - 1);

	int i;
	for (i = start_index; i != hash_table->end_indexes[start_index]; i = (i + 1) & (hash_table->size - 1)) {
		if (hash_table->elements[i] == key)
			return 1;
	}
	return 0;
}

inline char dynamic_hash_table_remove(void* key, DynamicHashTable* hash_table) {
	assert(key != NULL);

	int i;
	size_t start_index = HASH_POINTER(key) & (hash_table->size - 1);

	for (i = start_index; i != hash_table->end_indexes[start_index]; i = (i + 1) & (hash_table->size - 1)) {
		if (hash_table->elements[i] == key) {
			// key found
			hash_table->elements[i] = (void*) -1;
			return 1;
		}
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

inline void* dynamic_hash_table_iterator_next(DynamicHashTableIterator* iterator) {

	int i;
	for (i = iterator->current_index; i < iterator->hash_table->size; ++i)
		if (iterator->hash_table->elements[i] != NULL && iterator->hash_table->elements[i] != ((void*) -1)) {
			iterator->current_index = i + 1;
			return iterator->hash_table->elements[i];
		}

	return NULL;
}

inline int dynamic_hash_table_iterator_free(DynamicHashTableIterator* iterator) {
	free(iterator);

	return sizeof(DynamicHashTableIterator);
}
