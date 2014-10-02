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

#ifndef DYNAMIC_HASH_TABLE_H_
#define DYNAMIC_HASH_TABLE_H_

#include <stdint.h>

/**
 * A hash table interface for storing only stores keys, no associated values.
 * Keys cannot be NULL.
 */

typedef struct dynamic_hash_table DynamicHashTable;
typedef struct dynamic_hash_table_iterator DynamicHashTableIterator;


/**
 * Hash table.
 */
struct dynamic_hash_table {
	void** elements;			// the buckets
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
 * Insert the given key into the hash table. The key cannot be NULL.
 * Returns 1 if the key is inserted, 0 if it already existed.
 */
inline char dynamic_hash_table_insert(void* key, DynamicHashTable* hash_table);

/**
 * Remove the given key from the hash table. The key cannot be NULL.
 * Returns 1 if the key is removed, 0 if it did not exist.
 */
char dynamic_hash_table_remove(void* key, DynamicHashTable* hash_table);

/**
 * Check whether the given key exists in the hash table. The key cannot be NULL.
 * Returns 1 if so, 0 otherwise.
 */
inline char dynamic_hash_table_contains(void* key, DynamicHashTable* hash_table);

/**
 * Create an iterator for the given hash table.
 */
inline DynamicHashTableIterator* dynamic_hash_table_iterator_create(DynamicHashTable* h);

/**
 * Get the next element.
 * Returns NULL if there is no next element.
 */
inline void* dynamic_hash_table_iterator_next(DynamicHashTableIterator* iterator);

/**
 * Free the space allocated for a hash table iterator.
 * Returns the number of freed bytes.
 */
inline int dynamic_hash_table_iterator_free(DynamicHashTableIterator* iterator);

#endif
