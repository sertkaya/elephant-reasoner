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

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>

typedef struct hash_map_element HashMapElement;
typedef struct hash_map HashMap;

struct hash_map_element {
	uint64_t key;
	void* value;
	HashMapElement* previous;
	unsigned int previous_bucket_index;
	unsigned int previous_chain_index;
};

struct hash_map {
	HashMapElement** buckets;		// the buckets
	unsigned int bucket_count;		// the number of buckets
	unsigned int* chain_sizes;		// sizes of the chains
	HashMapElement* tail;			// the last node of the hash.
									// we maintain a backward linked list.
	unsigned int tail_bucket_index;
	unsigned int tail_chain_index;
};

/**
 * Create a hash map with the given number of buckets.
 */
HashMap* hash_map_create(unsigned int size);

/**
 * Free the space allocated for the given hash map.
 */
int hash_map_free(HashMap* hash_table);

/**
 * Insert a key value pair to the hash map. If the key already exists, the given
 * value is not inserted, i.e., the existing value is not overwritten.
 * Returns 1 if the key value pair is inserted, 0 otherwise.
 */
inline char hash_map_put(HashMap* hash_table, uint64_t key, void* value);

/**
 * Returns the value for the given key, it it exists, NULL if it does not exist.
 */
inline void* hash_map_get(HashMap* hash_table, uint64_t key);

/**
 * Returns the last node in the hash table or NULL if it is empty.
 * Note that we iterate the nodes in hash table in the backward order
 * of insertion, since the implementation is easier and the order of
 * iteration is not relevant for our purposes.
 */
// inline HashMapElement* hash_map_last_element(HashMap* hash_table);
// #define HASH_MAP_LAST_ELEMENT(hash_map)							hash_map->tail
#define HASH_MAP_LAST_ELEMENT(hash_map)							&hash_map->buckets[hash_map->tail_bucket_index][hash_map->tail_chain_index]

/**
 * Returns the node that comes before the current node, or NULL if
 * there is none.
 */
// inline HashMapElement* hash_map_previous_element(HashMapElement* current_node);
// #define HASH_MAP_PREVIOUS_ELEMENT(current_element)	current_element->previous
#define HASH_MAP_PREVIOUS_ELEMENT(hash_map, current_element)	((current_element->previous_bucket_index == -1) ? NULL : (&hash_map->buckets[current_element->previous_bucket_index][current_element->previous_chain_index]))
// #define HASH_MAP_PREVIOUS_ELEMENT_NEW(hash_map, current_element)	(&hash_map->buckets[current_element->previous_bucket_index][current_element->previous_chain_index])

// #define IS_FIRST_ELEMENT(hash_map, element)							(element->previous_bucket_index == 0) && (element->previous_chain_index == 0)
#define IS_FIRST_ELEMENT(hash_map, element)							(element == (&hash_map->buckets[0][0]))
#endif
