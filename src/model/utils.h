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

#include "../hashing/utils.h"
#include "datatypes.h"

// returns the ontology prefix with the given name if it exists
// NULL if it does not exist
#define GET_PREFIX(name, kb)				get_value(kb->prefixes, hash_string(name))

// inserts the ontology prefix with the given name into the hash
#define PUT_PREFIX(name, prefix, kb)		insert_key_value(kb->prefixes, hash_string(name), prefix)

// returns the atomic concept with the given name if it exists
// NULL if it does not exist
// Concept* get_atomic_concept(unsigned char* name, TBox* tbox);
#define GET_ATOMIC_CONCEPT(name, tbox)		get_value(tbox->atomic_concepts, hash_string(name))

// inserts the atomic concept with the given name to the hash
// void put_atomic_concept(unsigned char* name, Concept* c, TBox* tbox);
#define PUT_ATOMIC_CONCEPT(name, c, tbox)	insert_key_value(tbox->atomic_concepts, hash_string(name), c)

// get the existential restriction with role r and filler f from hash
// Concept* get_exists_restriction(int role_id, uint32_t filler_id, TBox* tbox);
#define GET_EXISTS_RESTRICTION(role_id, filler_id, tbox)		get_value(tbox->exists_restrictions, HASH_INTEGERS(role_id, filler_id))

// put the existential restriction with role r and filler f into hash
// void put_exists_restriction(int role_id, uint32_t filler_id, Concept* c, TBox* tbox);
#define PUT_EXISTS_RESTRICTION(role_id, filler_id, c, tbox)		insert_key_value(tbox->exists_restrictions, HASH_INTEGERS(role_id, filler_id), c)

// get the (binary) conjunction with the first conjunct c1 and second conjunct c2
Concept* get_conjunction(Concept* c1, Concept* c2, TBox* tbox);

// put the (binary) conjunction
void put_conjunction(Concept* c, TBox* tbox);

// return the nominal with the given individual if it exists
// NULL if it does not exist
#define GET_NOMINAL(individual, tbox)		get_value(tbox->nominals, individual->id)

// insert the nominal with the given individual
#define PUT_NOMINAL(n, tbox)			insert_key_value(tbox->nominals, n->description.nominal->individual->id, n)

// return the atomic role with the given name if it exists
// NULL if it does not exist
Role* get_atomic_role(unsigned char* name, TBox* tbox);

// insert the atomic role with the given name to the hash
void put_atomic_role(unsigned char* name, Role* c, TBox* tbox);


// get the role compoisiton with the given roles
Role* get_role_composition(Role* r1, Role* r2, TBox* tbox);

// put the role composition with the given roles into the role compositions hash
void put_role_composition(Role* r, TBox* tbox);

/******************************************************************************/
// Returns the individual with the given name if it exists
// NULL if it does not exist
#define GET_INDIVIDUAL(name, abox)			get_value(abox->individuals, hash_string(name))

// Inserts the given individual into the hash of individuals.
// Individual name is the key.
#define PUT_INDIVIDUAL(name, i, abox)		insert_key_value(abox->individuals, hash_string(name), i)

#endif
