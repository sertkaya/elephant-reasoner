#include <stdint.h>

#include "key_hash_table.h"
#include "key_value_hash_table.h"

/* Round up to the next highest power of 2 */
/* from http://www-graphics.stanford.edu/~seander/bithacks.html */
inline uint32_t roundup_pow2(uint32_t value) {
  --value;
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  ++value;

  return value;
}

/*
inline int hash_unsigned(KeyHashTable* hash_table, uint32_t key) {
	// return ((key << 5) - key) % hash_table->size;
	// this is more efficient
	return ((key << 5) - key) & hash_table->bucket_mask;
}
*/

/*
inline int hash_string(int bucket_count, unsigned char* key) {
	unsigned long hash_value = 5381;
	int c;

	while (c = *key++)
		hash_value = ((hash_value << 5) + hash_value) + c; // hash * 33 + c

	return hash_value & (bucket_count - 1);
}
*/

inline unsigned int hash_string(unsigned char* key) {
	unsigned int hash_value = 5381;
	int c;

	while (c = *key++)
		// hash_value = ((hash_value << 5) + hash_value) + c; /* hash * 33 + c */
		hash_value = ((hash_value << 5) - hash_value) + c; /* hash * 33 - c */

	return hash_value;
}
