
#ifndef KEY_VALUE_HASH_TABLE_H_
#define KEY_VALUE_HASH_TABLE_H_

#include <stdint.h>

typedef struct node Node;
typedef struct key_value_hash_table KeyValueHashTable;

struct node {
	void* key;
	void* value;
};

struct key_value_hash_table {
	Node*** buckets;		// the buckets
	unsigned bucket_mask;	// bit mask to access the buckets
	unsigned bucket_count;			// the number of buckets
	unsigned* chain_sizes;	// sizes of the chains
};

KeyValueHashTable* create_key_value_hash_table(unsigned size);

// inline char insert_key_value(KeyValueHashTable* hash_table, void* key, uint32_t hash_value, void* value);
// the compare_func returns 0 if the two keys are equal, 1 otherwise
inline char insert_key_value(KeyValueHashTable* hash_table,
		uint32_t hash_value,
		int (*compare_func)(void* key_to_insert, void* key),
		void* key_to_insert,
		void* value);

// inline void* get_value(KeyValueHashTable* hash_table, void* key, uint32_t hash_value);
inline void* get_value(KeyValueHashTable* hash_table,
		uint32_t hash_value,
		int (*compare_func)(void* key_to_insert, void* key),
		void* key);

#endif
