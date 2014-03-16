
#ifndef HASHING_UTILS_H_
#define HASHING_UTILS_H_

#include "key_hash_table.h"
#include "key_value_hash_table.h"

#define HASH_UNSIGNED(hash_table,key)			(((key << 5) - key) & hash_table->bucket_mask)

inline uint32_t roundup_pow2(uint32_t value);

inline int hash_unsigned(KeyHashTable* hash_table, uint32_t key);

inline int hash_string(KeyValueHashTable* hash_table, uint32_t key);

#endif /* UTILS_H_ */
