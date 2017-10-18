/*
 * The ELepHant Reasoner
 *
 * Copyright (C) Baris Sertkaya (sertkaya@fb2.fra-uas.de)
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

#include "murmur3.h"

#ifndef HASHING_UTILS_H_
#define HASHING_UTILS_H_

/**
 * Hash function for hashing a pointer.
 * TODO: improve and check 32-bit/64-bit differences
 */
#define HASH_POINTER(key)			((size_t) key) >> 3	// for 64 bit

/**
 * Simple hash function for hashing an unsigned integer.
 */
#define HASH_UNSIGNED(key)			((key << 3) - key)

/**
 * Hash two 32-bit unsigned integers into a 64-bit unsigned integer.
 */
#define HASH_INTEGERS(int1,int2)	((uint64_t)int1 << 32 | int2)

/**
 * Round up to the next higher power of 2. Used for hash sizes.
 */
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

/**
 * Hash a string into a 64-bit unsigned int using the Murmur3 algorithm.
 * 42 is the seed.
 */
#define HASH_STRING(key)			MurmurHash3_x64_128(key, strlen(key), 42)

#endif
