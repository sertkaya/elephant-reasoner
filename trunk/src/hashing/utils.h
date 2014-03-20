
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
 * Hash a string into a 64-bit int.
 */
inline uint64_t hash_string(unsigned char* key);

/**
 * Hash two integers into a 64-bit int.
 */
inline uint64_t hash_integers(unsigned int int1, unsigned int int2);

#endif /* UTILS_H_ */
