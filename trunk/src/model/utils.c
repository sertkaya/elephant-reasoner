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

#include "model.h"
#include "datatypes.h"
#include "limits.h"
#include "../hashing/utils.h"

// return the atomic concept with the given name if it exists
// NULL if it does not exist
inline Concept* get_atomic_concept(unsigned char* name, TBox* tbox) {
	return get_value(tbox->atomic_concepts,
			hash_string(name));
}

// insert the atomic concept with the given name
inline void put_atomic_concept(unsigned char* name, Concept* c, TBox* tbox) {
	insert_key_value(tbox->atomic_concepts,
			hash_string(name),
			c);
}

// return the atomic role with the given name if it exists
// NULL if it does not exist
Role* get_atomic_role(unsigned char* name, TBox* tbox) {
	return get_value(tbox->atomic_roles,
			hash_string(name));
}

// insert the atomic role with the given name
void put_atomic_role(unsigned char* name, Role* r, TBox* tbox) {
	insert_key_value(tbox->atomic_roles,
			hash_string(name),
			r);
}

// get the existential restriction with role r and filler f from hash
Concept* get_exists_restriction(uint32_t role_id, uint32_t filler_id, TBox* tbox) {
	Concept* c = get_value(tbox->exists_restrictions,
			HASH_INTEGERS(role_id, filler_id));

	return c;
}

// put the existential restriction with role r and filler f into hash
void put_exists_restriction(uint32_t role_id, uint32_t filler_id, Concept* c, TBox* tbox) {
	insert_key_value(tbox->exists_restrictions,
			HASH_INTEGERS(role_id, filler_id),
			c);
}

// We order c and d based on c->id and d->id!
Concept* get_conjunction(Concept* c, Concept* d, TBox* tbox) {

	if (c->id <= d->id)
		return get_value(tbox->conjunctions, HASH_INTEGERS(c->id, d->id));
	else
		return get_value(tbox->conjunctions, HASH_INTEGERS(d->id, c->id));
}

// the conjuncts of c are assumed to be ordered
void put_conjunction(Concept* c, TBox* tbox) {

	if (c->description.conj->conjunct1->id <= c->description.conj->conjunct2->id)
		insert_key_value(tbox->conjunctions, HASH_INTEGERS(c->description.conj->conjunct1->id, c->description.conj->conjunct2->id), c);
	else
		insert_key_value(tbox->conjunctions, HASH_INTEGERS(c->description.conj->conjunct2->id, c->description.conj->conjunct1->id), c);
}

Role* get_role_composition(Role* r, Role* s, TBox* tbox) {
	if (r->id <= s->id)
		return get_value(tbox->role_compositions, HASH_INTEGERS(r->id, s->id));
	else
		return get_value(tbox->role_compositions, HASH_INTEGERS(s->id, r->id));
}

void put_role_composition(Role* r, TBox* tbox) {
	if (r->description.role_composition->role1->id <= r->description.role_composition->role2->id)
		insert_key_value(tbox->role_compositions, HASH_INTEGERS(r->description.role_composition->role1->id, r->description.role_composition->role2->id), r);
	else
		insert_key_value(tbox->role_compositions, HASH_INTEGERS(r->description.role_composition->role2->id, r->description.role_composition->role1->id), r);
}
