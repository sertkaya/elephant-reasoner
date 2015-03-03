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

#include <stdint.h>

#ifndef HASHING_UTILS_H_
#define HASHING_UTILS_H_

/**
 * Hash function for hashing a pointer.
 * TODO: temporary implementation!
 */
// #define HASH_POINTER(key)									(((size_t) key << 5) - (size_t) key)
#define HASH_POINTER(key)									((size_t) key) >> 3	// for 64 bit
// #define HASH_POINTER(key)									((unsigned int) key) >> 3	// for 64 bit
// #define HASH_POINTER(key)									((unsigned int) key) 	// for 64 bit
// #define HASH_POINTER(key)									(size_t)((13 * (size_t) key) ^ ((size_t) key >> 15))

/**
 * Simple hash function for hashing an unsigned integer.
 */
// #define HASH_UNSIGNED(hash_table_bucket_count,key)			(((key << 5) - key) & (hash_table_bucket_count - 1))
#define HASH_UNSIGNED(key)									((key << 3) - key)

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

#endif
