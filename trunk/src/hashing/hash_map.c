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

	HashMap* hash_table = (HashMap*) malloc(sizeof(HashMap));
	assert(hash_table != NULL);

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	// allocate space for the buckets
	hash_table->buckets = (HashMapElement**) calloc(size, sizeof(HashMapElement*));
	assert(hash_table->buckets != NULL);

	// allocate space for the chain sizes
	hash_table->chain_sizes = (unsigned*) calloc(size, sizeof(unsigned int));
	assert(hash_table->buckets != NULL);

	hash_table->bucket_count = size;

	hash_table->tail = NULL;

	return hash_table;
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
	HashMapElement* bucket = hash_table->buckets[hash_value];
	int chain_size = hash_table->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; i++)
		if (bucket[i].key == key)
			return 0;

	HashMapElement* tmp = realloc(bucket, (chain_size + 1) * sizeof(HashMapElement));
	assert(tmp != NULL);
	bucket = hash_table->buckets[hash_value] = tmp;

	((HashMapElement*) (&bucket[chain_size]))->key = key;
	((HashMapElement*) (&bucket[chain_size]))->value = value;
	((HashMapElement*) (&bucket[chain_size]))->previous = hash_table->tail;

	hash_table->tail = (HashMapElement*) &(bucket[chain_size]);

	++hash_table->chain_sizes[hash_value];

	return 1;
}

inline void* hash_map_get(HashMap* hash_table, uint64_t key) {
	int bucket_index = key & (hash_table->bucket_count - 1);
	HashMapElement* bucket = hash_table->buckets[bucket_index];
	int chain_size = hash_table->chain_sizes[bucket_index];

	int i;
	for (i = 0; i < chain_size; ++i)
		if (key == ((HashMapElement*) (&bucket[i]))->key)
			return ((HashMapElement*) (&bucket[i]))->value;

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
