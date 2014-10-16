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
#include "hash_map.h"
#include "utils.h"


inline DynamicHashMap* dynamic_hash_map_create(unsigned int size) {

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	// allocate space for the hash map
	DynamicHashMap* hash_map = (DynamicHashMap*) malloc(sizeof(DynamicHashMap));
	assert(hash_map != NULL);

	hash_map->elements = (DynamicHashMapElement*) calloc(size, sizeof(DynamicHashMapElement));
	assert(hash_map->elements != NULL);

	hash_map->size = size;
	hash_map->element_count = 0;

	int i;
	for (i = 0; i < hash_map->size; ++i)
		// each chain initially ends at its starting point
		hash_map->end_indexes[i] = i;

	return hash_map;
}

int dynamic_hash_map_free(DynamicHashMap* hash_map) {
	int i;
	int freed_bytes = 0;

	// free the elements
	free(hash_map->elements);
	freed_bytes += hash_map->size * sizeof(DynamicHashMapElement);

	// free the end_indexes
	free(hash_map->end_indexes);
	freed_bytes += hash_map->size * sizeof(unsigned int);

	free(hash_map);
	freed_bytes += sizeof(DynamicHashMap);

	return freed_bytes;
}

/*
inline char hash_map_put(HashMap* hash_table,
		uint64_t key,
		void* value) {

	int hash_value = key & (hash_table->bucket_count - 1);
	// int hash_value = HASH_UNSIGNED(key) & (hash_table->bucket_count - 1);
	HashMapElement** bucket = hash_table->buckets[hash_value];
	int chain_size = hash_table->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; i++)
		if (bucket[i]->key == key)
			return 0;

	HashMapElement** tmp = realloc(bucket, (chain_size + 1) * sizeof(HashMapElement*));
	assert(tmp != NULL);
	bucket = hash_table->buckets[hash_value] = tmp;

	bucket[chain_size] = malloc(sizeof(HashMapElement));
	assert(bucket[chain_size] != NULL);
	bucket[chain_size]->key = key;
	bucket[chain_size]->value = value;
	bucket[chain_size]->previous = hash_table->tail;

	hash_table->tail = bucket[chain_size];

	++hash_table->chain_sizes[hash_value];

	return 1;
}

inline void* hash_map_get(HashMap* hash_table, uint64_t key) {
	int bucket_index = key & (hash_table->bucket_count - 1);
	HashMapElement** bucket = hash_table->buckets[bucket_index];
	int chain_size = hash_table->chain_sizes[bucket_index];

	int i;
	for (i = 0; i < chain_size; i++)
		if (key == bucket[i]->key)
			return bucket[i]->value;

	return NULL;
}
*/
/*
inline HashMapElement* hash_map_last_element(HashMap* hash_table) {
	return hash_table->tail;
}

inline HashMapElement* hash_map_previous_element(HashMapElement* current_node) {
	return current_node->previous;
}
*/
