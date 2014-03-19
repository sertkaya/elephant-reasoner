
#ifndef KEY_VALUE_HASH_TABLE_H_
#define KEY_VALUE_HASH_TABLE_H_

#include <stdint.h>

typedef struct node Node;
typedef struct key_value_hash_table KeyValueHashTable;

struct node {
	// void* key;
	uint32_t key;
	void* value;
	Node* previous;
};

struct key_value_hash_table {
	Node*** buckets;		// the buckets
	unsigned bucket_count;	// the number of buckets
	unsigned* chain_sizes;	// sizes of the chains
	Node* tail;				// the last node of the hash.
							// we maintain a backward linked list.
};

KeyValueHashTable* create_key_value_hash_table(unsigned size);

int free_key_value_hash_table(KeyValueHashTable* hash_table);

/**
 * The compare_func returns 0 if the two keys are equal, 1 otherwise
 */
/*
inline char insert_key_value(KeyValueHashTable* hash_table,
		uint32_t hash_value,
		int (*compare_func)(void* key_to_insert, void* key),
		void* key_to_insert,
		void* value);
		*/
inline char insert_key_value(KeyValueHashTable* hash_table,
		uint32_t key,
		void* value);

/*
inline void* get_value(KeyValueHashTable* hash_table,
		uint32_t hash_value,
		int (*compare_func)(void* key_to_insert, void* key),
		void* key);
		*/

inline void* get_value(KeyValueHashTable* hash_table,
		uint32_t key);

/**
 * Returns the last node in the hash table or NULL if it is empty.
 * Note that we iterate the nodes in hash table in the backward order
 * of insertion, since the implementation is easier and the order of
 * iteration is not relevant for our purposes.
 */
inline Node* last_node(KeyValueHashTable* hash_table);

/**
 * Returns the node that comes before the current node, or NULL if
 * there is none.
 */
inline Node* previous_node(Node* current_node);

#endif
