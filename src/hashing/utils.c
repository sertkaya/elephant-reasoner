/*
 * The ELepHant Reasoner
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

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "hash_table.h"
#include "hash_map.h"

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

// inline uint64_t hash_string(unsigned char* key) {
// 	uint64_t hash_value = 5381;
// 	int c;
//
// 	while ((c = *key++))
// 		hash_value = ((hash_value << 5) + hash_value) + c; /* hash * 33 + c */
// 		// hash_value = ((hash_value << 5) - hash_value) + c; /* hash * 31 + c */
//
// 	return hash_value;
// }

uint64_t hash_string(unsigned char *key) {
    uint64_t hash_value = 0;
    int c;
	while ((c = *key++)) {
        hash_value += c;
        hash_value += (hash_value << 10);
        hash_value ^= (hash_value >> 6);
    }
    hash_value += (hash_value << 3);
    hash_value ^= (hash_value >> 11);
    hash_value += (hash_value << 15);

    return hash_value;
}

// murmur3
inline uint32_t hash_pointer(uint32_t key) {
	uint32_t hash_value;
	hash_value ^= key >> 16;
	hash_value *= 0x85ebca6b;
	hash_value ^= hash_value >> 13;
	hash_value *= 0xc2b2ae35;
	hash_value ^= hash_value >> 16;

	return hash_value;
}
