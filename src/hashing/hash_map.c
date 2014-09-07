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
#include <inttypes.h>
#include "hash_map.h"
#include "utils.h"


inline HashMap* hash_map_create(unsigned int size) {

	HashMap* hash_map = (HashMap*) malloc(sizeof(HashMap));
	assert(hash_map != NULL);

	if (size < 8)
		size = 8;
	else
		size = roundup_pow2(size);

	// allocate space for the buckets
	hash_map->buckets = (HashMapElement**) calloc(size, sizeof(HashMapElement*));
	assert(hash_map->buckets != NULL);

	// allocate space for the chain sizes
	hash_map->chain_sizes = (unsigned int*) calloc(size, sizeof(unsigned int));
	assert(hash_map->buckets != NULL);

	hash_map->bucket_count = size;

	// hash_map->tail = NULL;
	hash_map->tail_bucket_index = -1;
	hash_map->tail_chain_index = -1;

	return hash_map;
}

int hash_map_free(HashMap* hash_table) {
	int freed_bytes = 0;

	/*
	int i;
	for (i = 0; i < hash_table->bucket_count; ++i) {
		if (hash_table->buckets[i] != NULL) {
			int j;
			for (j = 0; j < hash_table->chain_sizes[i]; ++j) {
				// note that we only free the node.
				// the space allocated for  node->value is not freed!
				free(hash_table->buckets[i][j]);
				freed_bytes += sizeof(HashMapElement);
			}

			free(hash_table->buckets[i]);
			freed_bytes += hash_table->chain_sizes[i] * sizeof(HashMapElement*);
		}
	}
	free(hash_table->buckets);
	freed_bytes += hash_table->bucket_count * sizeof(HashMapElement***);

	free(hash_table->chain_sizes);
	freed_bytes += hash_table->bucket_count * sizeof(unsigned int);

	free(hash_table);
	freed_bytes += sizeof(HashMap);
	*/

	return freed_bytes;
}

inline char hash_map_put(HashMap* hash_table,
		uint64_t key,
		void* value) {

	int hash_value = key & (hash_table->bucket_count - 1);
	// HashMapElement* bucket = hash_table->buckets[hash_value];
	int chain_size = hash_table->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; ++i)
		// if (bucket[i].key == key)
		if (hash_table->buckets[hash_value][i].key == key)
			return 0;

	// HashMapElement* tmp = realloc(bucket, (chain_size + 1) * sizeof(HashMapElement));
	HashMapElement* tmp = realloc(hash_table->buckets[hash_value], (chain_size + 1) * sizeof(HashMapElement));
	assert(tmp != NULL);
	// bucket = hash_table->buckets[hash_value] = tmp;
	hash_table->buckets[hash_value] = tmp;

	/*
	((HashMapElement*) (&bucket[chain_size]))->key = key;
	((HashMapElement*) (&bucket[chain_size]))->value = value;
	((HashMapElement*) (&bucket[chain_size]))->previous = hash_table->tail;
	*/
	/*
	bucket[chain_size].key = key;
	bucket[chain_size].value = value;
	bucket[chain_size].previous = hash_table->tail;
	*/

	hash_table->buckets[hash_value][chain_size].key = key;
	hash_table->buckets[hash_value][chain_size].value = value;
	// hash_table->buckets[hash_value][chain_size].previous = hash_table->tail;
	hash_table->buckets[hash_value][chain_size].previous_bucket_index = hash_table->tail_bucket_index;
	hash_table->buckets[hash_value][chain_size].previous_chain_index = hash_table->tail_chain_index;

	// hash_table->tail =  &bucket[chain_size];
	// hash_table->tail =  &hash_table->buckets[hash_value][chain_size];
	hash_table->tail_bucket_index = hash_value;
	hash_table->tail_chain_index = chain_size;


	++hash_table->chain_sizes[hash_value];

	// printf("key: %" PRIu64 "\tx.key: %" PRIu64 "\n", key, hash_table->buckets[hash_value][chain_size].key);

	return 1;
}

inline void* hash_map_get(HashMap* hash_table, uint64_t key) {
	int hash_value = key & (hash_table->bucket_count - 1);
	HashMapElement* bucket = hash_table->buckets[hash_value];
	int chain_size = hash_table->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; ++i)
		if (key == bucket[i].key)
			return &(bucket[i]).value;

	return NULL;
}
/*
inline HashMapElement* hash_map_last_element(HashMap* hash_table) {
	return hash_table->tail;
}

inline HashMapElement* hash_map_previous_element(HashMapElement* current_node) {
	return current_node->previous;
}
*/
