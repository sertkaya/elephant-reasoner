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
#include <string.h>

#include "hash_table.h"
#include "hash_map.h"
#include "murmur3.h"

/*
 * Round up to the next highest power of 2.
 * From http://www-graphics.stanford.edu/~seander/bithacks.html
 */
extern inline uint32_t roundup_pow2(uint32_t value);

/*
 * The Jenkins hash algorithm. Originally 32-bit, modified here to 64-bit.
 * Switched to the Murmur3 algorithm due to collisions in class names in large
 * ontologies, for instance the ore_ont_13355.owl ontology in the ORE 2015 dataset.
 */
/*
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
*/
