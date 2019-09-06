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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "dynamic_hash_table.h"
#include "utils.h"

/**
 * A dynamic hash table implementation for storing keys only, no associated values.
 * Keys are of type uint32_t.
 * The allocated space is doubled once the load factor reaches 0.75.
 * Uses open addressing with linear probing.
 * Keys cannot be NULL.
 * Allocated space is not shrunk after a removal.
 */


void dynamic_hash_table_init(DynamicHashTable* hash_table, unsigned int size) {

	if (size < 8)
		size = 8;
	else
		size = roundup_pow2(size);

	hash_table->elements = (uint32_t*) calloc(size, sizeof(uint32_t));
	assert(hash_table->elements != NULL);
	hash_table->end_indexes = (unsigned int*) calloc(size, sizeof(unsigned int));
	assert(hash_table->end_indexes != NULL);
	hash_table->size = size;
	hash_table->element_count = 0;
	int i;
	for (i = 0; i < hash_table->size; ++i)
		// each chain initially ends at its starting point
		hash_table->end_indexes[i] = i;

	return;
}

DynamicHashTable* dynamic_hash_table_create(unsigned int size) {
	if (size < 8)
		size = 8;
	else
		size = roundup_pow2(size);
	DynamicHashTable* hash_table = (DynamicHashTable*) malloc(sizeof(DynamicHashTable));
	assert(hash_table != NULL);

	hash_table->elements = (uint32_t*) calloc(size, sizeof(uint32_t));
	assert(hash_table->elements != NULL);
	hash_table->end_indexes = (unsigned int*) calloc(size, sizeof(unsigned int));
	assert(hash_table->end_indexes != NULL);
	hash_table->size = size;
	hash_table->element_count = 0;
	int i;
	for (i = 0; i < hash_table->size; ++i)
		// initialize with HASH_TABLE_EMPTY_KEY
		hash_table->elements[i] = HASH_TABLE_EMPTY_KEY;
		// each chain initially ends at its starting point
		hash_table->end_indexes[i] = i;

	return hash_table;
}

extern inline char dynamic_hash_table_insert(uint32_t key, DynamicHashTable* hash_table);

extern inline char dynamic_hash_table_contains(uint32_t key, DynamicHashTable* hash_table);

extern char dynamic_hash_table_remove(uint32_t key, DynamicHashTable* hash_table);

/**
 * Create an iterator for the given hash table.
 */
extern inline DynamicHashTableIterator* dynamic_hash_table_iterator_create(DynamicHashTable* h);

extern inline void dynamic_hash_table_iterator_init(DynamicHashTableIterator* iterator, DynamicHashTable* hash_table);

/**
 * Get the next element.
 * Returns HASH_TABLE_ZERO_KEY if there is no next element.
 */
extern inline uint32_t dynamic_hash_table_iterator_next(DynamicHashTableIterator* iterator);



int dynamic_hash_table_free(DynamicHashTable* hash_table) {
	int i;
	int freed_bytes = 0;

	free(hash_table->elements);
	freed_bytes += hash_table->size * sizeof(uint32_t);

	free(hash_table->end_indexes);
	freed_bytes += hash_table->size * sizeof(unsigned int);

	free(hash_table);
	freed_bytes += sizeof(DynamicHashTable);

	return freed_bytes;
}

int dynamic_hash_table_reset(DynamicHashTable* hash_table) {
	int i;
	int freed_bytes = 0;

	free(hash_table->elements);
	freed_bytes += hash_table->size * sizeof(uint32_t);

	free(hash_table->end_indexes);
	freed_bytes += hash_table->size * sizeof(unsigned int);

	hash_table->element_count = 0;
	hash_table->size = 0;

	return freed_bytes;
}

inline int dynamic_hash_table_iterator_free(DynamicHashTableIterator* iterator) {
	free(iterator);

	return sizeof(DynamicHashTableIterator);
}
