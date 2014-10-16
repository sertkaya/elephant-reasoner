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

#ifndef DYNAMIC_HASH_MAP_H
#define DYNAMIC_HASH_MAP_H

#include <stdint.h>

typedef struct dynamic_hash_map_element DynamicHashMapElement;
typedef struct dynamic_hash_map DynamicHashMap;

#define DELETED_KEY		-1

struct dynamic_hash_map_element {
	uint64_t key;
	void* value;
};

struct dynamic_hash_map {
	DynamicHashMapElement* elements;	// the elements
	unsigned int element_count;			// the number of elements
	unsigned int size;					// sizes of the hash map
	unsigned int* end_indexes;			// the end indexes of chains
};

/**
 * Create a hash map with the given number of initial buckets.
 */
DynamicHashMap* dynamic_hash_map_create(unsigned int size);

/**
 * Free the space allocated for the given hash map.
 */
int dynamic_hash_map_free(DynamicHashMap* hash_map);

/**
 * Insert a key value pair to the hash map. If the key already exists, the given
 * value is overwritten.
 * Returns 1 if the key value pair is inserted, 0 otherwise.
 */
inline char dynamic_hash_map_put(DynamicHashMap* hash_map, uint64_t key, void* value);

/**
 * Returns the value for the given key, it it exists, NULL if it does not exist.
 */
inline void* dynamic_hash_map_get(DynamicHashMap* hash_map, uint64_t key);


#endif
