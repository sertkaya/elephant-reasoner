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


#ifndef MODEL_UTILS_H_
#define MODEL_UTILS_H_

#include <stdint.h>
#include "datatypes.h"

// returns the atomic concept with the given name if it exists
// NULL if it does not exist
Concept* get_atomic_concept(unsigned char* name, TBox* tbox);

// inserts the atomic concept with the given name to the hash
void put_atomic_concept(unsigned char* name, Concept* c, TBox* tbox);

// return the atomic role with the given name if it exists
// NULL if it does not exist
Role* get_atomic_role(unsigned char* name, TBox* tbox);

// insert the atomic role with the given name to the hash
void put_atomic_role(unsigned char* name, Role* c, TBox* tbox);

// get the existential restriction with role r and filler f from hash
Concept* get_exists_restriction(int role_id, uint32_t filler_id, TBox* tbox);

// put the existential restriction with role r and filler f into hash
void put_exists_restriction(int role_id, uint32_t filler_id, Concept* c, TBox* tbox);

// get the (binary) conjunction with the first conjunct c1 and second conjunct c2
Concept* get_conjunction(Concept* c1, Concept* c2, TBox* tbox);

// put the (binary) conjunction
void put_conjunction(Concept* c, TBox* tbox);

// get the role compoisiton with the given roles
Role* get_role_composition(Role* r1, Role* r2, TBox* tbox);

// put the role composition with the given roles into the role compositions hash
void put_role_composition(Role* r, TBox* tbox);


#endif
