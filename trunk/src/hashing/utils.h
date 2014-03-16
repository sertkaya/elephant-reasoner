
#ifndef HASHING_UTILS_H_
#define HASHING_UTILS_H_

#include "key_hash_table.h"
#include "key_value_hash_table.h"

inline uint32_t roundup_pow2(uint32_t value);

inline int hash_unsigned(KeyHashTable* hash_table, uint32_t key);

inline int hash_string(KeyValueHashTable* hash_table, uint32_t key);

#endif /* UTILS_H_ */
