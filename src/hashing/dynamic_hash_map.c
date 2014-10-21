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
#include "dynamic_hash_map.h"
#include "utils.h"


void dynamic_hash_map_init(DynamicHashMap* hash_map, unsigned int size) {

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	hash_map->elements = (DynamicHashMapElement*) calloc(size, sizeof(DynamicHashMapElement));
	assert(hash_map->elements != NULL);

	hash_map->end_indexes = (unsigned int*) calloc(size, sizeof(unsigned int));
	assert(hash_map->end_indexes != NULL);

	hash_map->size = size;
	hash_map->element_count = 0;

	int i;
	for (i = 0; i < hash_map->size; ++i)
		// each chain initially ends at its starting point
		hash_map->end_indexes[i] = i;

	return;
}

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

	hash_map->end_indexes = (unsigned int*) calloc(size, sizeof(unsigned int));
	assert(hash_map->end_indexes != NULL);

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

inline char dynamic_hash_map_put(DynamicHashMap* hash_map, uint64_t key, void* value) {
	int i, j, new_size;
	size_t start_index;

	assert(key != HASH_MAP_EMPTY_KEY && key != HASH_MAP_DELETED_KEY);

	start_index  = key & (hash_map->size - 1);
	for (i = start_index; ; i = (i + 1) & (hash_map->size - 1)) {
		if (hash_map->elements[i].key == key) {
			// the key already exists, overwrite the value
			hash_map->elements[i].value = value;
			return 0;
		}

		if (hash_map->elements[i].key == HASH_MAP_EMPTY_KEY) {
			// insert the key here
			hash_map->elements[i].key = key;
			hash_map->elements[i].value = value;
			++hash_map->element_count;
			// mark the new end index
			hash_map->end_indexes[start_index] = (i + 1) & (hash_map->size - 1);
			break;
		}
	}

	// check if we need to resize. load factor is 0.75
	if (hash_map->element_count * 4 >= hash_map->size * 3) {
		// the load factor is reached, resize
		new_size = 2 * hash_map->size;
		DynamicHashMapElement* tmp_elements = (DynamicHashMapElement*) calloc(new_size, sizeof(DynamicHashMapElement));
		assert(tmp_elements != NULL);


		// free the end indexes, allocate and initialize new
		free(hash_map->end_indexes);
		hash_map->end_indexes = (unsigned int*) calloc(new_size, sizeof(unsigned int));
		assert(hash_map->end_indexes != NULL);
		for (i = 0; i < new_size; ++i)
			hash_map->end_indexes[i] = i;


		// re-populate
		for (i = 0; i < hash_map->size; ++i)
			if (hash_map->elements[i].key != HASH_MAP_EMPTY_KEY && hash_map->elements[i].key != HASH_MAP_DELETED_KEY) {
				start_index = hash_map->elements[i].key & (new_size - 1);
				for (j = start_index; ; j = (j + 1) & (new_size - 1))
					if (tmp_elements[j].key == HASH_MAP_EMPTY_KEY) {
						tmp_elements[j].key = hash_map->elements[i].key;
						tmp_elements[j].value = hash_map->elements[i].value;
						break;
					}
				hash_map->end_indexes[start_index] = (j + 1) & (new_size - 1);
			}

		// change the size, the element count does not change
		hash_map->size = new_size;

		// free the existing elements
		free(hash_map->elements);

		// the new elements
		hash_map->elements = tmp_elements;
	}
	return 1;
}

inline void* dynamic_hash_map_get(DynamicHashMap* hash_map, uint64_t key) {

	assert(key != HASH_MAP_EMPTY_KEY && key != HASH_MAP_DELETED_KEY);
	size_t start_index = key & (hash_map->size - 1);

	int i;
	for (i = start_index; i != hash_map->end_indexes[start_index]; i = (i + 1) & (hash_map->size - 1)) {
		if (hash_map->elements[i].key == key)
			return hash_map->elements[i].value;
	}
	return NULL;
}

inline char dynamic_hash_map_remove(uint64_t key, DynamicHashMap* hash_map) {
	assert(key != HASH_MAP_EMPTY_KEY && key != HASH_MAP_DELETED_KEY);

	int i;
	size_t start_index = key & (hash_map->size - 1);

	for (i = start_index; i != hash_map->end_indexes[start_index]; i = (i + 1) & (hash_map->size - 1)) {
		if (hash_map->elements[i].key == key) {
			// key found
			hash_map->elements[i].key = HASH_MAP_DELETED_KEY;
			--hash_map->element_count;
			return 1;
		}
	}
	return 0;
}

inline DynamicHashMapIterator* dynamic_hash_map_iterator_create(DynamicHashMap* hash_map) {
	DynamicHashMapIterator* iterator = (DynamicHashMapIterator*) malloc(sizeof(DynamicHashMapIterator));
	assert(iterator != NULL);
	iterator->hash_map = hash_map;
	iterator->current_index = 0;

	return iterator;
}

inline void dynamic_hash_map_iterator_init(DynamicHashMapIterator* iterator, DynamicHashMap* hash_map) {

	iterator->hash_map = hash_map;
	iterator->current_index = 0;

	return;
}

inline void* dynamic_hash_map_iterator_next(DynamicHashMapIterator* iterator) {

	int i;
	for (i = iterator->current_index; i < iterator->hash_map->size; ++i)
		if (iterator->hash_map->elements[i].key != HASH_MAP_EMPTY_KEY && iterator->hash_map->elements[i].key != HASH_MAP_DELETED_KEY) {
			iterator->current_index = i + 1;
			return iterator->hash_map->elements[i].value;
		}

	return NULL;
}

inline int dynamic_hash_map_iterator_free(DynamicHashMapIterator* iterator) {
	free(iterator);

	return sizeof(DynamicHashMapIterator);
}
