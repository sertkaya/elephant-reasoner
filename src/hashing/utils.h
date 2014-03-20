
#ifndef HASHING_UTILS_H_
#define HASHING_UTILS_H_

#include "key_hash_table.h"
#include "key_value_hash_table.h"

/**
 * Simple hash function for hashing an unsigned integer
 */
#define HASH_UNSIGNED(hash_table_bucket_count,key)			(((key << 5) - key) & (hash_table_bucket_count - 1))

/**
 * Hash two 32-bit unsigned integers into a 64-bit unsigned integer.
 */
#define HASH_INTEGERS(int1,int2)							((uint64_t)int1 << 32 | int2)

/**
 * Round up to the next higher power of 2. Used for hash sizes.
 */
inline uint32_t roundup_pow2(uint32_t value);

/**
 * Hash a string into a 64-bit int.
 */
inline uint64_t hash_string(unsigned char* key);

#endif /* UTILS_H_ */
