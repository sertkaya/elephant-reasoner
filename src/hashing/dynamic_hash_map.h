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
typedef struct dynamic_hash_map_iterator DynamicHashMapIterator;


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
 * Iterator for hash map.
 */
struct dynamic_hash_map_iterator {
	DynamicHashMap* hash_map;
	unsigned int current_index;	// index of the current element
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

/**
 * Removes the given key (and its associated value) from the hash map.
 * Returns 1 if removed successfully, 0 otherwise.
 */
inline char dynamic_hash_map_remove(uint64_t key, DynamicHashMap* hash_map);

/**
 * Create an iterator for the given hash map.
 */
inline DynamicHashMapIterator* dynamic_hash_map_iterator_create(DynamicHashMap* hash_map);

/**
 * Reset a given hash map iterator.
 */
inline void dynamic_hash_map_iterator_init(DynamicHashMapIterator* iterator, DynamicHashMap* hash_map);

/**
 * Get the next element.
 * Returns NULL if there is no next element.
 */
inline void* dynamic_hash_map_iterator_next(DynamicHashMapIterator* iterator);

/**
 * Free the space allocated for a hash map iterator.
 * Returns the number of freed bytes.
 */
inline int dynamic_hash_map_iterator_free(DynamicHashMapIterator* iterator);
#endif
