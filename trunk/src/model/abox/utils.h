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


#ifndef ABOX_MODEL_UTILS_H_
#define ABOX_MODEL_UTILS_H_

#include <stdint.h>
#include "datatypes.h"

// returns the individual with the given name if it exists
// NULL if it does not exist
#define GET_INDIVIDUAL(name, abox)			get_value(abox->individuals, hash_string(name))

// Inserts the given individual into the hash of individuals.
// Individual name is the key.
#define PUT_INDIVIDUAL(name, i, abox)		insert_key_value(abox->individuals, hash_string(name), i)

#endif
