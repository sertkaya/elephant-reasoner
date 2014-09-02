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


#ifndef MODEL_UTILS_H_
#define MODEL_UTILS_H_

#include <stdint.h>

#include "../hashing/utils.h"
#include "../hashing/hash_map.h"
#include "datatypes.h"

// returns the atomic concept with the given name if it exists
// NULL if it does not exist
#define GET_ATOMIC_CONCEPT(IRI, tbox)		hash_map_get(tbox->atomic_concepts, hash_string(IRI))

// inserts the atomic concept with the given name to the hash
#define PUT_ATOMIC_CONCEPT(IRI, c, tbox)	hash_map_put(tbox->atomic_concepts, hash_string(IRI), c)

// get the existential restriction with role r and filler f from hash
#define GET_EXISTS_RESTRICTION(role_id, filler_id, tbox)		hash_map_get(tbox->exists_restrictions, HASH_INTEGERS(role_id, filler_id))

// put the existential restriction with role r and filler f into hash
#define PUT_EXISTS_RESTRICTION(role_id, filler_id, c, tbox)		hash_map_put(tbox->exists_restrictions, HASH_INTEGERS(role_id, filler_id), c)

// get the (binary) conjunction with the first conjunct c1 and second conjunct c2
#define GET_CONJUNCTION(c1, c2, tbox)		(c1->id <= c2->id) ? hash_map_get(tbox->conjunctions, HASH_INTEGERS(c1->id, c2->id)) : hash_map_get(tbox->conjunctions, HASH_INTEGERS(c2->id, c1->id))

// put the (binary) conjunction
#define PUT_CONJUNCTION(c, tbox)			(c->description.conj->conjunct1->id <= c->description.conj->conjunct2->id) ? hash_map_put(tbox->conjunctions, HASH_INTEGERS(c->description.conj->conjunct1->id, c->description.conj->conjunct2->id), c) : hash_map_put(tbox->conjunctions, HASH_INTEGERS(c->description.conj->conjunct2->id, c->description.conj->conjunct1->id), c)

// return the nominal with the given individual if it exists
// NULL if it does not exist
#define GET_NOMINAL(individual, tbox)		hash_map_get(tbox->nominals, individual->id)

// insert the nominal with the given individual
#define PUT_NOMINAL(n, tbox)				hash_map_put(tbox->nominals, n->description.nominal->individual->id, n)

// return the atomic role with the given name if it exists
// NULL if it does not exist
#define GET_ATOMIC_ROLE(IRI, tbox)			hash_map_get(tbox->atomic_roles, hash_string(IRI))

// insert the atomic role with the given name to the hash
#define PUT_ATOMIC_ROLE(IRI, r, tbox)		hash_map_put(tbox->atomic_roles, hash_string(IRI), r)


// get the role compoisiton with the given roles
#define GET_ROLE_COMPOSITION(r1, r2, tbox)	(r1->id <= r2->id) ? hash_map_get(tbox->role_compositions, HASH_INTEGERS(r1->id, r2->id)) : hash_map_get(tbox->role_compositions, HASH_INTEGERS(r2->id, r1->id))

// put the role composition with the given roles into the role compositions hash
#define PUT_ROLE_COMPOSITION(r, tbox)		(r->description.role_composition->role1->id <= r->description.role_composition->role2->id) ? hash_map_put(tbox->role_compositions, HASH_INTEGERS(r->description.role_composition->role1->id, r->description.role_composition->role2->id), r) : hash_map_put(tbox->role_compositions, HASH_INTEGERS(r->description.role_composition->role2->id, r->description.role_composition->role1->id), r)

/******************************************************************************/
// Returns the individual with the given name if it exists
// NULL if it does not exist
#define GET_INDIVIDUAL(IRI, abox)			hash_map_get(abox->individuals, hash_string(IRI))

// Inserts the given individual into the hash of individuals.
// Individual name is the key.
#define PUT_INDIVIDUAL(IRI, i, abox)		hash_map_put(abox->individuals, hash_string(IRI), i)

#endif
