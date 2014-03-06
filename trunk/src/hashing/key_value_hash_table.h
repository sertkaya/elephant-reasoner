/*
 * hash_table.h
 *
 *  Created on: Feb 4, 2014
 *      Author: baris
 */

#ifndef KEY_VALUE_HASH_TABLE_H_
#define KEY_VALUE_HASH_TABLE_H_

#include <stdint.h>

typedef struct node Node;
typedef struct key_value_hash_table KeyValueHashTable;

struct node {
	uint32_t key;
	void* value;
};

struct key_value_hash_table {
	Node*** buckets;		// the buckets
	unsigned bucket_mask;	// bit mask to access the buckets
	unsigned size;			// the number of elements
	unsigned* chain_sizes;	// sizes of the chains
};

KeyValueHashTable* create_key_value_hash_table(unsigned size);

inline char insert_key_value(KeyValueHashTable* hash_table, uint32_t key, void* value);

// inline char contains_key(KeyValueHashTable* hash_table, uint32_t key);

inline void* get_value(KeyValueHashTable* hash_table, uint32_t key);


#endif /* HASH_TABLE_H_ */
