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


inline HashMap* hash_map_create(unsigned int size) {

	HashMap* hash_map = (HashMap*) malloc(sizeof(HashMap));
	assert(hash_map != NULL);

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	// allocate space for the buckets
	hash_map->buckets = (HashMapElement***) calloc(size, sizeof(HashMapElement**));
	assert(hash_map->buckets != NULL);

	// allocate space for the chain sizes
	hash_map->chain_sizes = (unsigned*) calloc(size, sizeof(unsigned int));
	assert(hash_map->buckets != NULL);

	hash_map->bucket_count = size;

	hash_map->tail = NULL;

	hash_map->element_count = 0;

	return hash_map;
}

inline void hash_map_init(HashMap* hash_map, unsigned int size) {

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	// allocate space for the buckets
	hash_map->buckets = (HashMapElement***) calloc(size, sizeof(HashMapElement**));
	assert(hash_map->buckets != NULL);

	// allocate space for the chain sizes
	hash_map->chain_sizes = (unsigned*) calloc(size, sizeof(unsigned int));
	assert(hash_map->buckets != NULL);

	hash_map->bucket_count = size;

	hash_map->tail = NULL;

	hash_map->element_count = 0;
}


int hash_map_free(HashMap* hash_map) {
	int freed_bytes = 0;

	int i;
	for (i = 0; i < hash_map->bucket_count; ++i) {
		if (hash_map->buckets[i] != NULL) {
			int j;
			for (j = 0; j < hash_map->chain_sizes[i]; ++j) {
				// note that we only free the node.
				// the space allocated for  node->value is not freed!
				free(hash_map->buckets[i][j]);
				freed_bytes += sizeof(HashMapElement);
			}

			free(hash_map->buckets[i]);
			freed_bytes += hash_map->chain_sizes[i] * sizeof(HashMapElement*);
		}
	}
	free(hash_map->buckets);
	freed_bytes += hash_map->bucket_count * sizeof(HashMapElement***);

	free(hash_map->chain_sizes);
	freed_bytes += hash_map->bucket_count * sizeof(unsigned int);

	free(hash_map);
	freed_bytes += sizeof(HashMap);

	return freed_bytes;
}

inline int hash_map_put(HashMap* hash_map, uint64_t key, void* value) {

	int hash_value = key & (hash_map->bucket_count - 1);
	// int hash_value = HASH_UNSIGNED(key) & (hash_map->bucket_count - 1);
	HashMapElement** bucket = hash_map->buckets[hash_value];
	int chain_size = hash_map->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; i++)
		if (bucket[i]->key == key) {
			bucket[i]->value = value;
			return 0;
		}

	HashMapElement** tmp = realloc(bucket, (chain_size + 1) * sizeof(HashMapElement*));
	assert(tmp != NULL);
	bucket = hash_map->buckets[hash_value] = tmp;

	bucket[chain_size] = malloc(sizeof(HashMapElement));
	assert(bucket[chain_size] != NULL);
	bucket[chain_size]->key = key;
	bucket[chain_size]->value = value;
	bucket[chain_size]->previous = hash_map->tail;

	hash_map->tail = bucket[chain_size];

	++hash_map->chain_sizes[hash_value];

	++hash_map->element_count;

	return 1;
}

inline void* hash_map_get(HashMap* hash_map, uint64_t key) {
	int bucket_index = key & (hash_map->bucket_count - 1);
	HashMapElement** bucket = hash_map->buckets[bucket_index];
	int chain_size = hash_map->chain_sizes[bucket_index];

	int i;
	for (i = 0; i < chain_size; i++)
		if (key == bucket[i]->key)
			return bucket[i]->value;

	return NULL;
}

void hash_map_iterator_init(HashMapIterator* iterator, HashMap* map) {
	iterator->hash_map = map;
	HashMapElement* tmp = (HashMapElement*) calloc(1, sizeof(HashMapElement));
	assert(tmp != NULL);
	tmp->previous = map->tail;
	iterator->current_element = tmp;
}


inline void* hash_map_iterator_next(HashMapIterator* iterator) {
	void* next = iterator->current_element->previous;
	iterator->current_element = iterator->current_element->previous;
	if (!next) return NULL;
	return ((HashMapElement*) next)->value;
}


/*
inline HashMapElement* hash_map_last_element(HashMap* hash_map) {
	return hash_map->tail;
}

inline HashMapElement* hash_map_previous_element(HashMapElement* current_node) {
	return current_node->previous;
}
*/
