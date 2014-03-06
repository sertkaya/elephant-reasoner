#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "key_value_hash_table.h"
#include "utils.h"

inline int hash(KeyValueHashTable* hash_table, key_t key) {
	return ((key << 5) - key) % hash_table->size;
	// return ((key << 5) - key) & hash_table->bucket_mask;
}



/*
 * Integer hash of 32 bits.
 * Implementation of the Robert Jenkins "4-byte Integer Hashing",
 * from http://burtleburtle.net/bob/hash/integer.html
 */

/*
static inline uint32_t hash(KeyValueHashTable* hash_table, key_t key) {
    key -= (key<<6);
    key ^= (key>>17);
    key -= (key<<9);
    key ^= (key<<4);
    key -= (key<<3);
    key ^= (key<<10);
    key ^= (key>>15);

    key &= hash_table->bucket_mask;
    return key;
}
*/


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

	hash_table->size = size;
	hash_table->bucket_mask = hash_table->size - 1;

	/*
	int i;
	for (i = 0; i < size; i++) {
		hash_table->buckets[i] = NULL;
		hash_table->chain_sizes[i] = 0;
	}
	*/
	memset(hash_table->buckets, 0, hash_table->size * sizeof(Node**));
	memset(hash_table->chain_sizes, 0, hash_table->size * sizeof(int));

	return hash_table;
}

inline char insert_key_value(KeyValueHashTable* hash_table, uint32_t key, void* value) {
	int i;
	int hash_value = hash(hash_table, key);
	int chain_size = hash_table->chain_sizes[hash_value];

	for (i = 0; i < chain_size; i++)
		if (hash_table->buckets[hash_value][i]->key == key)
			return 0;

	Node** tmp = realloc(hash_table->buckets[hash_value], (chain_size + 1) * sizeof(Node*));
	assert(tmp != NULL);
	hash_table->buckets[hash_value] = tmp;

	hash_table->buckets[hash_value][chain_size] = malloc(sizeof(Node));
	assert(hash_table->buckets[hash_value][chain_size] != NULL);
	hash_table->buckets[hash_value][chain_size]->key = key;
	hash_table->buckets[hash_value][chain_size]->value = value;

	++hash_table->chain_sizes[hash_value];

	return 1;
}

inline void* get_value(KeyValueHashTable* hash_table, uint32_t key) {
	int hash_value = hash(hash_table, key);
	int chain_size = hash_table->chain_sizes[hash_value];

	int i;
	for (i = 0; i < chain_size; i++)
		if (hash_table->buckets[hash_value][i]->key == key)
			return hash_table->buckets[hash_value][i]->value;

	return NULL;
}
