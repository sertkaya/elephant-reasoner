#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "key_value_hash_table.h"
#include "utils.h"


inline KeyValueHashTable* create_key_value_hash_table(unsigned size) {

	KeyValueHashTable* hash_table = (KeyValueHashTable*) malloc(sizeof(KeyValueHashTable));
	assert(hash_table != NULL);

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	// allocate space for the buckets
	hash_table->buckets = (Node***) calloc(size, sizeof(Node**));
	assert(hash_table->buckets != NULL);

	// allocate space for the chain sizes
	hash_table->chain_sizes = (unsigned*) calloc(size, sizeof(unsigned));
	assert(hash_table->buckets != NULL);

	hash_table->bucket_count = size;

	// memset(hash_table->buckets, 0, hash_table->bucket_count * sizeof(Node**));
	// memset(hash_table->chain_sizes, 0, hash_table->bucket_count * sizeof(unsigned));

	hash_table->tail = NULL;

	return hash_table;
}

int free_key_value_hash_table(KeyValueHashTable* hash_table) {
	int freed_bytes = 0;

	int i;
	for (i = 0; i < hash_table->bucket_count; ++i) {
		if (hash_table->buckets[i] != NULL) {
			int j;
			for (j = 0; j < hash_table->chain_sizes[i]; ++j) {
				// note that we only free the node.
				// the space allocated for  node->value is not freed!
				free(hash_table->buckets[i][j]);
				freed_bytes += sizeof(Node);
			}

			free(hash_table->buckets[i]);
			freed_bytes += hash_table->chain_sizes[i] * sizeof(Node*);
		}
	}
	free(hash_table->buckets);
	freed_bytes += hash_table->bucket_count * sizeof(Node***);

	free(hash_table->chain_sizes);
	freed_bytes += hash_table->bucket_count * sizeof(unsigned);

	free(hash_table);
	freed_bytes += sizeof(KeyValueHashTable);

	return freed_bytes;
}

/*
inline char insert_key_value(KeyValueHashTable* hash_table,
		uint32_t hash_value,
		int (*compare_func)(void* key_to_insert, void* key),
		void* key_to_insert,
		void* value) {
	int i;
	int chain_size = hash_table->chain_sizes[hash_value];

	for (i = 0; i < chain_size; i++)
		// if (hash_table->buckets[hash_value][i]->key == key)
		if (compare_func(key_to_insert, hash_table->buckets[hash_value][i]->key) == 0)
			return 0;

	Node** tmp = realloc(hash_table->buckets[hash_value], (chain_size + 1) * sizeof(Node*));
	assert(tmp != NULL);
	hash_table->buckets[hash_value] = tmp;

	hash_table->buckets[hash_value][chain_size] = malloc(sizeof(Node));
	assert(hash_table->buckets[hash_value][chain_size] != NULL);
	hash_table->buckets[hash_value][chain_size]->key = key_to_insert;
	hash_table->buckets[hash_value][chain_size]->value = value;

	++hash_table->chain_sizes[hash_value];

	return 1;
}
*/


inline char insert_key_value(KeyValueHashTable* hash_table,
		unsigned long key,
		void* value) {

	int bucket_index = key & (hash_table->bucket_count - 1);
	Node** bucket = hash_table->buckets[bucket_index];
	int chain_size = hash_table->chain_sizes[bucket_index];

	int i;
	for (i = 0; i < chain_size; i++)
		// if (hash_table->buckets[hash_value][i]->key == key)
		if (key == bucket[i]->key)
			return 0;

	Node** tmp = realloc(bucket, (chain_size + 1) * sizeof(Node*));
	assert(tmp != NULL);
	bucket = hash_table->buckets[bucket_index] = tmp;

	bucket[chain_size] = calloc(1, sizeof(Node));
	assert(bucket[chain_size] != NULL);
	bucket[chain_size]->key = key;
	bucket[chain_size]->value = value;
	bucket[chain_size]->previous = hash_table->tail;

	hash_table->tail = bucket[chain_size];
	++hash_table->chain_sizes[bucket_index];

	return 1;
}

/*
inline void* get_value(KeyValueHashTable* hash_table, uint32_t hash_value, int (*compare_func)(void* key_to_search, void* key), void* key_to_search) {
	int chain_size = hash_table->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; i++)
		if (compare_func(key_to_search, hash_table->buckets[hash_value][i]->key) == 0)
			return hash_table->buckets[hash_value][i]->value;

	return NULL;
}
*/

inline void* get_value(KeyValueHashTable* hash_table, unsigned long key) {
	int bucket_index = key & (hash_table->bucket_count - 1);
	Node** bucket = hash_table->buckets[bucket_index];
	int chain_size = hash_table->chain_sizes[bucket_index];

	int i;
	for (i = 0; i < chain_size; i++)
		if (key == bucket[i]->key)
			return bucket[i]->value;

	return NULL;
}

inline Node* last_node(KeyValueHashTable* hash_table) {
	return hash_table->tail;
}

inline Node* previous_node(Node* current_node) {
	return current_node->previous;
}
