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

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>

/*
 * A hash map implementation for storing 32 bit ids as values.
 * Keys are of type uint64_t
 * Values are of type uint32_t
 */

#define KEY_NOT_FOUND_IN_HASH_MAP UINT32_MAX

typedef struct hash_map HashMap;
typedef struct hash_map_element HashMapElement;
typedef struct hash_map_iterator HashMapIterator;

struct hash_map_element {
	uint64_t key;
	uint32_t value;
	HashMapElement *previous;
};

struct hash_map {
	HashMapElement*** buckets;		// the buckets
	unsigned int bucket_count;		// the number of buckets
	unsigned int *chain_sizes;		// sizes of the chains
	unsigned int element_count;	// the number of elements
	HashMapElement *tail;			// the last node of the hash.
									// we maintain a backward linked list.
};

/**
 * Iterator for hash map.
 */
struct hash_map_iterator {
	HashMap *hash_map;
	HashMapElement *current_element;
};

/**
 * Create a hash map with the given number of buckets.
 */
HashMap* hash_map_create(unsigned int size);

/**
 * Initialize a hash map with the given number of buckets.
 */
void hash_map_init(HashMap*hash_map, unsigned int size);

/**
 * Free the space allocated for the given hash map.
 */
int hash_map_free(HashMap* hash_map);

/**
 * Free the space allocated for the elements of a given hash map.
 * Does not free the space allocated for the hash map itself.
 */
int hash_map_reset(HashMap* hash_map);

/**
 * Insert a key value pair to the hash map. If the key already exists, the given
 * value is not inserted, i.e., the existing value is not overwritten.
 * Returns 1 if the key value pair is inserted, 0 otherwise.
 */
inline int hash_map_put(HashMap* hash_map, uint64_t key, uint32_t value) {

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


/**
 * Returns the value for the given key, it it exists, KEY_NOT_FOUND if it does not exist.
 */
inline uint32_t hash_map_get(HashMap* hash_map, uint64_t key) {
	int bucket_index = key & (hash_map->bucket_count - 1);
	HashMapElement** bucket = hash_map->buckets[bucket_index];
	int chain_size = hash_map->chain_sizes[bucket_index];

	int i;
	for (i = 0; i < chain_size; i++)
		if (key == bucket[i]->key)
			return bucket[i]->value;

	return KEY_NOT_FOUND_IN_HASH_MAP;
}
/**
 * Reset a given hash map iterator.
 */
void hash_map_iterator_init(HashMapIterator* iterator, HashMap* map);

/**
 * Get the next element.
 * Returns NULL if there is no next element.
 */
inline uint32_t  hash_map_iterator_next(HashMapIterator* iterator) {
	/*
	HashMapElement *next = iterator->current_element->previous;
	iterator->current_element = next;
	if (!next)
		return(NULL);
	return(next->value);
	*/
	HashMapElement *next = iterator->current_element;
	if (!next)
		return(KEY_NOT_FOUND_IN_HASH_MAP);
	iterator->current_element = next->previous;
	return(next->value);
}


/**
 * Returns the last node in the hash table or NULL if it is empty.
 * Note that we iterate the nodes in hash table in the backward order
 * of insertion, since the implementation is easier and the order of
 * iteration is not relevant for our purposes.
 */
// inline HashMapElement* hash_map_last_element(HashMap* hash_map);
#define HASH_MAP_LAST_ELEMENT(hash_map)			hash_map->tail

/**
 * Returns the node that comes before the current node, or NULL if
 * there is none.
 */
#define HASH_MAP_PREVIOUS_ELEMENT(current_element)	current_element->previous

#endif
