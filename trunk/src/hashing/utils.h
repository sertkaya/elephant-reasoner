
#ifndef HASHING_UTILS_H_
#define HASHING_UTILS_H_

#include "key_hash_table.h"
#include "key_value_hash_table.h"

/**
 * Simple hash function for hashing an unsigned integer
 */
#define HASH_UNSIGNED(hash_table_bucket_count,key)			(((key << 5) - key) & (hash_table_bucket_count - 1))

/**
 * Round up to the next higher power of 2. Used for hash sizes.
 */
inline uint32_t roundup_pow2(uint32_t value);

/**
 * Hash a string into an integer
 */
inline int hash_string(int hash_table_bucket_count, uint32_t key);

#endif /* UTILS_H_ */
