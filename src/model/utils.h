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
#include "../utils/map.h"
#include "datatypes.h"

// returns the prefix with the given prefix name
#define GET_ONTOLOGY_PREFIX(prefix_name, kb)	MAP_GET(hash_string(prefix_name), &(kb->prefixes))

// inserts the prefix with the given prefix name into the prefixes hash
#define PUT_ONTOLOGY_PREFIX(prefix_name, prefix, kb)		MAP_PUT(hash_string(prefix_name), prefix, &(kb->prefixes))

// returns the atomic concept with the given name if it exists
// NULL if it does not exist
#define GET_ATOMIC_CONCEPT(IRI, tbox)		MAP_GET(hash_string(IRI), &(tbox->classes))

// inserts the atomic concept with the given name to the hash
#define PUT_ATOMIC_CONCEPT(IRI, c, tbox)	MAP_PUT(hash_string(IRI), c, &(tbox->classes))

// get the existential restriction with role r and filler f from hash
#define GET_EXISTS_RESTRICTION(role_id, filler_id, tbox)		MAP_GET(HASH_INTEGERS(role_id, filler_id), &(tbox->object_some_values_from_exps))

// put the existential restriction with role r and filler f into hash
#define PUT_EXISTS_RESTRICTION(role_id, filler_id, c, tbox)		MAP_PUT(HASH_INTEGERS(role_id, filler_id), c, &(tbox->object_some_values_from_exps))

// get the (binary) conjunction with the first conjunct c1 and second conjunct c2
#define GET_CONJUNCTION(c1, c2, tbox)		(c1->id <= c2->id) ? MAP_GET(HASH_INTEGERS(c1->id, c2->id), &(tbox->object_intersection_of_exps)) : MAP_GET(HASH_INTEGERS(c2->id, c1->id), &(tbox->object_intersection_of_exps))

// put the (binary) conjunction
#define PUT_CONJUNCTION(c, tbox)			(c->description.conj.conjunct1->id <= c->description.conj.conjunct2->id) ? MAP_PUT(HASH_INTEGERS(c->description.conj.conjunct1->id, c->description.conj.conjunct2->id), c, &(tbox->object_intersection_of_exps)) : MAP_PUT(HASH_INTEGERS(c->description.conj.conjunct2->id, c->description.conj.conjunct1->id), c, &(tbox->object_intersection_of_exps))

// return the nominal with the given individual if it exists
// NULL if it does not exist
#define GET_NOMINAL(individual, tbox)		MAP_GET(individual->id, &(tbox->object_one_of_exps))

// insert the nominal with the given individual
#define PUT_NOMINAL(n, tbox)				MAP_PUT(n->description.nominal.individual->id, n, &(tbox->object_one_of_exps))

// return the atomic role with the given name if it exists
// NULL if it does not exist
#define GET_ATOMIC_ROLE(IRI, tbox)			MAP_GET(hash_string(IRI), &(tbox->object_properties))

// insert the atomic role with the given name to the hash
#define PUT_ATOMIC_ROLE(IRI, r, tbox)		MAP_PUT(hash_string(IRI), r, &(tbox->object_properties))


// get the role compoisiton with the given roles
#define GET_ROLE_COMPOSITION(r1, r2, tbox)	MAP_GET(HASH_INTEGERS(r1->id, r2->id), &(tbox->object_property_chains))

// put the role composition with the given roles into the role compositions hash
#define PUT_ROLE_COMPOSITION(r, tbox)		MAP_PUT(HASH_INTEGERS(r->description.object_property_chain.role1->id, r->description.object_property_chain.role2->id), r, &(tbox->object_property_chains))

/******************************************************************************/
// Returns the individual with the given name if it exists
// NULL if it does not exist
#define GET_INDIVIDUAL(IRI, abox)			hash_map_get(abox->individuals, hash_string(IRI))

// Inserts the given individual into the hash of individuals.
// Individual name is the key.
#define PUT_INDIVIDUAL(IRI, i, abox)		hash_map_put(abox->individuals, hash_string(IRI), i)

#endif
