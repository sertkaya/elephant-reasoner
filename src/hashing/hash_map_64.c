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
#include "hash_map_64.h"
#include "utils.h"


inline HashMap_64* hash_map_create_64(unsigned int size) {

	HashMap_64* hash_map = (HashMap_64*) malloc(sizeof(HashMap_64));
	assert(hash_map != NULL);

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	// allocate space for the buckets
	hash_map->buckets = (HashMapElement_64***) calloc(size, sizeof(HashMapElement_64**));
	assert(hash_map->buckets != NULL);

	// allocate space for the chain sizes
	hash_map->chain_sizes = (unsigned*) calloc(size, sizeof(unsigned int));
	assert(hash_map->buckets != NULL);

	hash_map->bucket_count = size;

	hash_map->tail = NULL;

	hash_map->element_count = 0;

	return hash_map;
}

void hash_map_init_64(HashMap_64* hash_map, unsigned int size) {

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	// allocate space for the buckets
	hash_map->buckets = (HashMapElement_64***) calloc(size, sizeof(HashMapElement_64**));
	assert(hash_map->buckets != NULL);

	// allocate space for the chain sizes
	hash_map->chain_sizes = (unsigned*) calloc(size, sizeof(unsigned int));
	assert(hash_map->buckets != NULL);

	hash_map->bucket_count = size;

	hash_map->tail = NULL;

	hash_map->element_count = 0;
}


int hash_map_free_64(HashMap_64* hash_map) {
	int freed_bytes = 0;

	int i;
	for (i = 0; i < hash_map->bucket_count; ++i) {
		if (hash_map->buckets[i] != NULL) {
			int j;
			for (j = 0; j < hash_map->chain_sizes[i]; ++j) {
				// note that we only free the node.
				// the space allocated for  node->value is not freed!
				free(hash_map->buckets[i][j]);
				freed_bytes += sizeof(HashMapElement_64);
			}

			free(hash_map->buckets[i]);
			freed_bytes += hash_map->chain_sizes[i] * sizeof(HashMapElement_64*);
		}
	}
	free(hash_map->buckets);
	freed_bytes += hash_map->bucket_count * sizeof(HashMapElement_64***);

	free(hash_map->chain_sizes);
	freed_bytes += hash_map->bucket_count * sizeof(unsigned int);

	free(hash_map);
	freed_bytes += sizeof(HashMap_64);

	return freed_bytes;
}

int hash_map_reset_64(HashMap_64* hash_map) {
	int freed_bytes = 0;

	int i;
	for (i = 0; i < hash_map->bucket_count; ++i) {
		if (hash_map->buckets[i] != NULL) {
			int j;
			for (j = 0; j < hash_map->chain_sizes[i]; ++j) {
				// note that we only free the node.
				// the space allocated for  node->value is not freed!
				free(hash_map->buckets[i][j]);
				freed_bytes += sizeof(HashMapElement_64);
			}

			free(hash_map->buckets[i]);
			freed_bytes += hash_map->chain_sizes[i] * sizeof(HashMapElement_64*);
		}
	}
	free(hash_map->buckets);
	freed_bytes += hash_map->bucket_count * sizeof(HashMapElement_64***);

	free(hash_map->chain_sizes);
	freed_bytes += hash_map->bucket_count * sizeof(unsigned int);

	hash_map->bucket_count = 0;
	hash_map->element_count = 0;

	return freed_bytes;
}

extern inline int hash_map_put_64(HashMap_64* hash_map, uint64_t key, void* value);

extern inline void* hash_map_get_64(HashMap_64* hash_map, uint64_t key);

void hash_map_iterator_init_64(HashMapIterator_64* iterator, HashMap_64* map) {
	iterator->hash_map = map;
	HashMapElement_64* tmp = (HashMapElement_64*) calloc(1, sizeof(HashMapElement_64));
	assert(tmp != NULL);
	tmp->previous = map->tail;
	iterator->current_element = tmp;
}

extern inline void* hash_map_iterator_next_64(HashMapIterator_64* iterator);
