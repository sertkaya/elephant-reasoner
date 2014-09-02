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

#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <stdint.h>

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
inline char hash_table_insert(HashTable* hash_table, void* key);

/**
 * Remove the given key from the hash table. The key cannot be NULL.
 * Returns 1 if the key is removed, 0 if it did not exist.
 */
char hash_table_remove(HashTable* hash_table, void* key);

/**
 * Check whether the given key exists in the hash table. The key cannot be NULL.
 * Returns 1 if so, 0 otherwise.
 */
inline char hash_table_contains(HashTable* hash_table, void* key);

/**
 * Create an iterator for the given hash table.
 */
inline HashTableIterator* hash_table_iterator_create(HashTable* h);

/**
 * Get the next element.
 * Returns NULL if there is no next element.
 */
inline void* hash_table_iterator_next(HashTableIterator* i);

/**
 * Free the space allocated for a hash table iterator.
 * Returns the number of freed bytes.
 */
inline int hash_table_iterator_free(HashTableIterator* it);

#endif /* HASH_TABLE_H_ */
