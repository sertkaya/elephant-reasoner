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

	hash_table->buckets = (Node***) malloc(size * sizeof(Node**));
	assert(hash_table->buckets != NULL);

	hash_table->chain_sizes = (unsigned*) malloc(size * sizeof(unsigned));
	assert(hash_table->buckets != NULL);

	hash_table->bucket_count = size;
	hash_table->bucket_mask = hash_table->bucket_count - 1;

	/*
	int i;
	for (i = 0; i < size; i++) {
		hash_table->buckets[i] = NULL;
		hash_table->chain_sizes[i] = 0;
	}
	*/
	memset(hash_table->buckets, 0, hash_table->bucket_count * sizeof(Node**));
	memset(hash_table->chain_sizes, 0, hash_table->bucket_count * sizeof(unsigned));

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
				// the space allocated for node->key and node->value are not freed!
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


inline char insert_key_value(KeyValueHashTable* hash_table,
		uint32_t hash_value,
		int (*compare_func)(void* key_to_insert, void* key),
		void* key_to_insert,
		void* value) {
	int i;
	int chain_size = hash_table->chain_sizes[hash_value];

	for (i = 0; i < chain_size; i++)
		// if (hash_table->buckets[hash_value][i]->key == key)
		if (!compare_func(key_to_insert, hash_table->buckets[hash_value][i]->key))
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

inline void* get_value(KeyValueHashTable* hash_table, uint32_t hash_value, int (*compare_func)(void* key_to_search, void* key), void* key_to_search) {
	int chain_size = hash_table->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; i++)
		if (!compare_func(key_to_search, hash_table->buckets[hash_value][i]->key))
			return hash_table->buckets[hash_value][i]->value;

	return NULL;
}
