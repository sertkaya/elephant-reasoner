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

#include "hash_table.h"
#include "utils.h"

/**
 * An efficient hash table implementation that only stores keys, no associated values.
 * Keys cannot be NULL.
 */


inline HashTable* hash_table_create(unsigned int bucket_count) {

	HashTable* hash_table = (HashTable*) malloc(sizeof(HashTable));
	assert(hash_table != NULL);

	if (bucket_count < 8)
		bucket_count = 8;
	else
		bucket_count = roundup_pow2(bucket_count);

	hash_table->buckets = (void***) calloc(bucket_count, sizeof(void**));
	assert(hash_table->buckets != NULL);

	hash_table->chain_sizes = (unsigned int*) calloc(bucket_count, sizeof(unsigned int));
	assert(hash_table->chain_sizes != NULL);

	hash_table->bucket_count = bucket_count;

	return hash_table;
}

int hash_table_free(HashTable* hash_table) {
	int i;
	int freed_bytes = 0;

	for (i = 0; i < hash_table->bucket_count; ++i) {
		if (hash_table->buckets[i] != NULL) {
			free(hash_table->buckets[i]);
			freed_bytes += hash_table->chain_sizes[i] * sizeof(void*);
		}
	}
	free(hash_table->buckets);
	freed_bytes += hash_table->bucket_count * sizeof(void**);

	free(hash_table->chain_sizes);
	freed_bytes += hash_table->bucket_count * sizeof(unsigned int);

	free(hash_table);
	freed_bytes += sizeof(HashTable);

	return freed_bytes;
}

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

inline char hash_table_contains(HashTable* hash_table, void* key) {
	// assert(key != NULL);
	size_t hash_value = HASH_POINTER(key) & (hash_table->bucket_count - 1);

	int i;
	for (i = 0; i < hash_table->chain_sizes[hash_value]; ++i)
		if (hash_table->buckets[hash_value][i] == key)
			return 1;

	return 0;
}


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

inline HashTableIterator* hash_table_iterator_create(HashTable* h) {
	HashTableIterator* iterator = (HashTableIterator*) malloc(sizeof(HashTableIterator));
	assert(iterator != NULL);

	iterator->hash_table = h;
	iterator->bucket_index = 0;
	iterator->chain_index = 0;

	return iterator;
}

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

inline int hash_table_iterator_free(HashTableIterator* it) {
	free(it);

	return sizeof(HashTableIterator);
}
