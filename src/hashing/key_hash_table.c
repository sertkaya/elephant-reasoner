#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "key_hash_table.h"
#include "utils.h"


inline KeyHashTable* create_key_hash_table(unsigned size) {

	KeyHashTable* hash_table = (KeyHashTable*) malloc(sizeof(KeyHashTable));
	assert(hash_table != NULL);

	if (size < 16)
		size = 16;
	else
		size = roundup_pow2(size);

	hash_table->buckets = (unsigned**) calloc(size, sizeof(unsigned*));
	assert(hash_table->buckets != NULL);

	hash_table->chain_sizes = (unsigned*) calloc(size, sizeof(unsigned));
	assert(hash_table->buckets != NULL);

	hash_table->bucket_count = size;
	hash_table->bucket_mask = hash_table->bucket_count - 1;

	return hash_table;
}

int free_key_hash_table(KeyHashTable* hash_table) {
	int i;
	int freed_bytes = 0;

	for (i = 0; i < hash_table->bucket_count; ++i) {
		if (hash_table->buckets[i] != NULL) {
			free(hash_table->buckets[i]);
			freed_bytes += hash_table->chain_sizes[i] * sizeof(unsigned);
		}
	}
	free(hash_table->buckets);
	freed_bytes += hash_table->bucket_count * sizeof(unsigned*);

	free(hash_table->chain_sizes);
	freed_bytes += hash_table->bucket_count * sizeof(unsigned);

	free(hash_table);
	freed_bytes += sizeof(KeyHashTable);

	return freed_bytes;
}

inline char insert_key(KeyHashTable* hash_table, uint32_t key) {
	int i;
	int hash_value = HASH_UNSIGNED(hash_table->bucket_count, key);
	int chain_size = hash_table->chain_sizes[hash_value];

	for (i = 0; i < chain_size; i++)
		if (hash_table->buckets[hash_value][i] == key)
			return 0;

	int *tmp = realloc(hash_table->buckets[hash_value], (chain_size + 1) * sizeof(int));
	assert(tmp != NULL);
	hash_table->buckets[hash_value] = tmp;
	hash_table->buckets[hash_value][chain_size] = key;
	++hash_table->chain_sizes[hash_value];

	return 1;
}

inline char contains_key(KeyHashTable* hash_table, uint32_t key) {
	int hash_value = HASH_UNSIGNED(hash_table->bucket_count, key);
	int chain_size = hash_table->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; i++)
		if (hash_table->buckets[hash_value][i] == key)
			return 1;

	return 0;
}


inline char remove_key(KeyHashTable* hash_table, uint32_t key) {

	if  (!contains_key(hash_table, key))
		return 0;

	int i;
	int hash_value = HASH_UNSIGNED(hash_table->bucket_count, key);
	int chain_size = hash_table->chain_sizes[hash_value];

	for (i = 0; i < chain_size; ++i)
		if (hash_table->buckets[hash_value][i] == key)
			break;

	// return 0 if key is not in the hash
	if (i == chain_size - 1 && hash_table->buckets[hash_value][i] != key)
		return 0;

	// i is the index of the key to be removed.
	int j;
	// shift the keys, overwrite the key to be removed
	for (j = i; j < chain_size - 1; ++j)
		hash_table->buckets[hash_value][j] = hash_table->buckets[hash_value][j + 1];

	// shrink the chain
	int* tmp = realloc(hash_table->buckets[hash_value], (chain_size - 1) * sizeof(int));
	if(chain_size > 1)
		assert(tmp != NULL);
	hash_table->buckets[hash_value] = tmp;

	// decrement the chain size
	--hash_table->chain_sizes[hash_value];

	return 1;
}
