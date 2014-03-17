/*
 * hash_table.h
 *
 *  Created on: Feb 4, 2014
 *      Author: baris
 */

#ifndef KEY_HASH_TABLE_H_
#define KEY_HASH_TABLE_H_

#include <stdint.h>

/*
 * A simple hash table whose keys are integers. It is mainly used to check whether a
 * key exists or not. We do not store values.
 */

typedef struct key_hash_table KeyHashTable;

struct key_hash_table {
	unsigned** buckets;		// the buckets
	unsigned bucket_mask;	// bit mask to access the buckets
	unsigned bucket_count;	// the number of buckets
	unsigned* chain_sizes;	// sizes of the chains
};

inline KeyHashTable* create_key_hash_table(unsigned size);

int free_key_hash_table(KeyHashTable* hash_table);

inline char insert_key(KeyHashTable* hash_table, uint32_t key);

inline char remove_key(KeyHashTable* hash_table, uint32_t key);

inline char contains_key(KeyHashTable* hash_table, uint32_t key);

#endif /* HASH_TABLE_H_ */
