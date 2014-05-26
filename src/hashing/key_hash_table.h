/*
 * The Elephant Reasoner
 *
 * Copyright (C) Baris Sertkaya (sertkaya.baris@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
