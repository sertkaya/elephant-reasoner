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

#ifndef HASH_MAP_64_H
#define HASH_MAP_64_H

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

/*
 * A hash map implementation for storing 64 bit expression ids as values.
 * Keys are of type uint64_t
 * Values are of type void*
 */
typedef struct hash_map_64 HashMap_64;
typedef struct hash_map_element_64 HashMapElement_64;
typedef struct hash_map_iterator_64 HashMapIterator_64;

struct hash_map_element_64 {
	uint64_t key;
	void* value;
	HashMapElement_64* previous;
};

struct hash_map_64 {
	HashMapElement_64*** buckets;		// the buckets
	unsigned int bucket_count;		// the number of buckets
	unsigned int* chain_sizes;		// sizes of the chains
	unsigned int element_count;	// the number of elements
	HashMapElement_64* tail;			// the last node of the hash.
									// we maintain a backward linked list.
};

/**
 * Iterator for hash map.
 */
struct hash_map_iterator_64 {
	HashMap_64* hash_map;
	HashMapElement_64* current_element;// of the current element
};

/**
 * Create a hash map with the given number of buckets.
 */
HashMap_64* hash_map_create_64(unsigned int size);

/**
 * Initialize a hash map with the given number of buckets.
 */
void hash_map_init_64(HashMap_64*hash_map, unsigned int size);

/**
 * Free the space allocated for the given hash map.
 */
int hash_map_free_64(HashMap_64* hash_map);

/**
 * Free the space allocated for the elements of a given hash map.
 * Does not free the space allocated for the hash map itself.
 */
int hash_map_reset_64(HashMap_64* hash_map);

/**
 * Insert a key value pair to the hash map. If the key already exists, the given
 * value is not inserted, i.e., the existing value is not overwritten.
 * Returns 1 if the key value pair is inserted, 0 otherwise.
 */
inline int hash_map_put_64(HashMap_64* hash_map, uint64_t key, void* value) {

	int hash_value = key & (hash_map->bucket_count - 1);
	// int hash_value = HASH_UNSIGNED(key) & (hash_map->bucket_count - 1);
	HashMapElement_64** bucket = hash_map->buckets[hash_value];
	int chain_size = hash_map->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; i++)
		if (bucket[i]->key == key) {
			bucket[i]->value = value;
			return 0;
		}

	HashMapElement_64** tmp = realloc(bucket, (chain_size + 1) * sizeof(HashMapElement_64*));
	assert(tmp != NULL);
	bucket = hash_map->buckets[hash_value] = tmp;

	bucket[chain_size] = malloc(sizeof(HashMapElement_64));
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
 * Returns the value for the given key, it it exists, NULL if it does not exist.
 */
inline void* hash_map_get_64(HashMap_64* hash_map, uint64_t key) {
	int bucket_index = key & (hash_map->bucket_count - 1);
	HashMapElement_64** bucket = hash_map->buckets[bucket_index];
	int chain_size = hash_map->chain_sizes[bucket_index];

	int i;
	for (i = 0; i < chain_size; i++)
		if (key == bucket[i]->key)
			return bucket[i]->value;

	return NULL;
}
/**
 * Reset a given hash map iterator.
 */
void hash_map_iterator_init_64(HashMapIterator_64* iterator, HashMap_64* map);

/**
 * Get the next element.
 * Returns NULL if there is no next element.
 */
inline void* hash_map_iterator_next_64(HashMapIterator_64* iterator) {
	void* next = iterator->current_element->previous;
	iterator->current_element = iterator->current_element->previous;
	if (!next) return NULL;
	return ((HashMapElement_64*) next)->value;
}


/**
 * Returns the last node in the hash table or NULL if it is empty.
 * Note that we iterate the nodes in hash table in the backward order
 * of insertion, since the implementation is easier and the order of
 * iteration is not relevant for our purposes.
 */
// inline HashMapElement_64* hash_map_last_element(HashMap_64* hash_map);
#define HASH_MAP_LAST_ELEMENT_64(hash_map)			hash_map->tail

/**
 * Returns the node that comes before the current node, or NULL if
 * there is none.
 */
#define HASH_MAP_PREVIOUS_ELEMENT_64(current_element)	current_element->previous

#endif
