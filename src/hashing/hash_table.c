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


HashTable* hash_table_create(unsigned int bucket_count) {

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

extern inline char hash_table_insert(HashTable* hash_table, void* key);

extern inline char hash_table_contains(HashTable* hash_table, void* key);

extern inline char hash_table_remove(HashTable* hash_table, void* key);

extern inline HashTableIterator* hash_table_iterator_create(HashTable* h);

extern inline void* hash_table_iterator_next(HashTableIterator* i);

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
inline int hash_table_iterator_free(HashTableIterator* it) {
	free(it);

	return sizeof(HashTableIterator);
}
