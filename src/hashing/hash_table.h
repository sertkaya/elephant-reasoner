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

#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include "utils.h"

/**
 * An hash table interface for storing only stores keys, no associated values.
 * Keys cannot be NULL.
 */

typedef struct hash_table HashTable;
typedef struct hash_table_iterator HashTableIterator;


/**
 * Hash table.
 */
struct hash_table {
	void*** buckets;			// the buckets
	unsigned int bucket_count;	// the number of buckets
	unsigned int* chain_sizes;	// sizes of the chains
};

/**
 * Iterator for hash table.
 */
struct hash_table_iterator {
	HashTable* hash_table;
	unsigned int bucket_index;	// index of the current bucket
	unsigned int chain_index;	// index of the current chain element
};

/**
 * Create a hash table with the given number of buckets.
 */
HashTable* hash_table_create(unsigned int bucket_count);

/**
 * Free the space for the given hash table.
 */
int hash_table_free(HashTable* hash_table);

/**
 * Insert the given key into the hash table. The key cannot be NULL.
 * Returns 1 if the key is inserted, 0 if it already existed.
 */
inline char hash_table_insert(HashTable* hash_table, void* key) {
	assert(key != NULL);

	int i;
	size_t hash_value = HASH_POINTER(key) & (hash_table->bucket_count - 1);
	int chain_size = hash_table->chain_sizes[hash_value];

	// check if the key already exists
	for (i = 0; i < chain_size; ++i)
		if (hash_table->buckets[hash_value][i] == key)
			return 0;

	// insert the key
	void** tmp = (void**) realloc(hash_table->buckets[hash_value], (chain_size + 1) * sizeof(void*));
	assert(tmp != NULL);
	hash_table->buckets[hash_value] = tmp;
	hash_table->buckets[hash_value][chain_size] = key;
	++hash_table->chain_sizes[hash_value];

	return 1;
}

/**
 * Check whether the given key exists in the hash table. The key cannot be NULL.
 * Returns 1 if so, 0 otherwise.
 */
inline char hash_table_contains(HashTable* hash_table, void* key) {
	// assert(key != NULL);
	size_t hash_value = HASH_POINTER(key) & (hash_table->bucket_count - 1);

	int i;
	for (i = 0; i < hash_table->chain_sizes[hash_value]; ++i)
		if (hash_table->buckets[hash_value][i] == key)
			return 1;

	return 0;
}

/**
 * Remove the given key from the hash table. The key cannot be NULL.
 * Returns 1 if the key is removed, 0 if it did not exist.
 */

inline char hash_table_remove(HashTable* hash_table, void* key) {
	assert(key != NULL);

	if  (!hash_table_contains(hash_table, key))
		return 0;

	int i;
	size_t hash_value = HASH_POINTER(key) & (hash_table->bucket_count - 1);
	int chain_size = hash_table->chain_sizes[hash_value];

	for (i = 0; i < chain_size; ++i)
		if (hash_table->buckets[hash_value][i] == key)
			break;

	// return 0 if key is not in the hash
	if (i == chain_size - 1 && hash_table->buckets[hash_value][i] != key)
		return 0;

	// i is the index of the key to be removed.
	// mark it as a removed element.
	hash_table->buckets[hash_value][i] = NULL;

	/*
	// i is the index of the key to be removed.
	int j;
	// shift the keys, overwrite the key to be removed
	for (j = i; j < chain_size - 1; ++j)
		hash_table->buckets[hash_value][j] = hash_table->buckets[hash_value][j + 1];

	// shrink the chain
	void** tmp = (void**) realloc(hash_table->buckets[hash_value], (chain_size - 1) * sizeof(void*));
	if(chain_size > 1)
		assert(tmp != NULL);
	hash_table->buckets[hash_value] = tmp;

	// decrement the chain size
	--hash_table->chain_sizes[hash_value];
	*/

	return 1;
}

/**
 * Create an iterator for the given hash table.
 */
inline HashTableIterator* hash_table_iterator_create(HashTable* h) {
	HashTableIterator* iterator = (HashTableIterator*) malloc(sizeof(HashTableIterator));
	assert(iterator != NULL);

	iterator->hash_table = h;
	iterator->bucket_index = 0;
	iterator->chain_index = 0;

	return iterator;
}

/**
 * Get the next element.
 * Returns NULL if there is no next element.
 */
inline void* hash_table_iterator_next(HashTableIterator* it) {

	while (it->bucket_index < it->hash_table->bucket_count)
		if (it->hash_table->buckets[it->bucket_index] != NULL) {
			while (it->chain_index < it->hash_table->chain_sizes[it->bucket_index]) {
				if (it->hash_table->buckets[it->bucket_index][it->chain_index] != NULL) {
					// found a non-NULL chain element
					return it->hash_table->buckets[it->bucket_index][it->chain_index++];
				}
				else {
					// a NULL-element (indicates a removed element)
					++it->chain_index;
				}
			}
			// traversed all elements of this chain, go to the next one
			++it->bucket_index;
			it->chain_index = 0;
		}
		else {
			// a NULL-bucket (indicates a not yet used bucket)
			++it->bucket_index;
		}

	// No non-NULL bucket element und no non-NULL chain element found
	return NULL;
}


/**
 * Free the space allocated for a hash table iterator.
 * Returns the number of freed bytes.
 */
int hash_table_iterator_free(HashTableIterator* it);

#endif /* HASH_TABLE_H_ */
