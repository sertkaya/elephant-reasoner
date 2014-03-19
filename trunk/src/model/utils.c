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


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <Judy.h>

#include "model.h"
#include "datatypes.h"
#include "limits.h"

// two ids of type int + the underscore in between
// unsigned char exists_restriction_buffer[16];
// #define BUILD_EXISTS_RESTRICTION_ID(r,f) snprintf((char*) exists_restriction_buffer, 16, "%d_%d", r, f)
#define BUILD_EXISTS_RESTRICTION_ID(r, f, buffer) snprintf((char*) buffer, 16, "%d_%d", r, f)

// char conjunctions_buffer[MAX_CONJUNCT_COUNT * sizeof(int)];
// char conjunct_buffer[16];
// unsigned char conjunctions_buffer[19];
unsigned char conjunctions_buffer[16];

// #define BUILD_CONJUNCTION_ID(c1,c2) (c1 <= c2) ? snprintf((char*) conjunctions_buffer, 19, "%p_%p", c1, c2) : snprintf((char*) conjunctions_buffer, 19, "%p_%p", c2, c1)
#define BUILD_CONJUNCTION_ID(c1,c2) (c1 <= c2) ? snprintf((char*) conjunctions_buffer, 16, "%d_%d", c1, c2) : snprintf((char*) conjunctions_buffer, 16, "%d_%d", c2, c1)

unsigned char role_composition_buffer[16];
#define BUILD_ROLE_COMPOSITION_ID(r1,r2) (r1 <= r2) ? snprintf((char*) role_composition_buffer, 16, "%d_%d", r1, r2) : snprintf((char*) role_composition_buffer, 16, "%d_%d", r2, r1)

// return the atomic concept with the given name if it exists
// NULL if it does not exist
inline Concept* get_atomic_concept(unsigned char* name, TBox* tbox) {
	/*
	return get_value(tbox->atomic_concepts,
			hash_string(tbox->atomic_concepts->bucket_count, name),
			(int (*) (void *, void *)) strcmp,
			name);
			*/
	return get_value(tbox->atomic_concepts,
			hash_string(name));
}

// insert the atomic concept with the given name
inline void put_atomic_concept(unsigned char* name, Concept* c, TBox* tbox) {
	/*
	insert_key_value(tbox->atomic_concepts,
			hash_string(tbox->atomic_concepts->bucket_count, name),
			(int (*) (void *, void *)) strcmp,
			name,
			c);
			*/
	insert_key_value(tbox->atomic_concepts,
			hash_string(name),
			c);
}

// return the atomic role with the given name if it exists
// NULL if it does not exist
Role* get_atomic_role(unsigned char* name, TBox* tbox) {
	/*
	return get_value(tbox->atomic_roles,
			hash_string(tbox->atomic_roles->bucket_count, name),
			(int (*) (void *, void *)) strcmp,
			name);
			*/
	return get_value(tbox->atomic_roles,
			hash_string(name));
}

// insert the atomic role with the given name
void put_atomic_role(unsigned char* name, Role* r, TBox* tbox) {
	/*
	insert_key_value(tbox->atomic_roles,
			hash_string(tbox->atomic_roles->bucket_count, name),
			(int (*) (void *, void *)) strcmp,
			name,
			r);
			*/
	insert_key_value(tbox->atomic_roles,
			hash_string(name),
			r);
}

// get the existential restriction with role r and filler f from hash
Concept* get_exists_restriction(int r, int f, TBox* tbox) {
	unsigned char buffer[16];

	BUILD_EXISTS_RESTRICTION_ID(r, f, buffer);
	// printf("get %s\n", buffer);
	/*
	Concept* c = get_value(tbox->exists_restrictions,
			hash_string(tbox->exists_restrictions->bucket_count, buffer),
			(int (*) (void *, void *)) strcmp,
			buffer);
			*/
	Concept* c = get_value(tbox->exists_restrictions,
			hash_string(buffer));

	/*
	if (c != NULL)
		printf("retrieved %d\t%d\t%s\t%d\t%d\n", r, f, buffer, c->description.exists->role->id, c->description.exists->filler->id);
	else
		printf("retrieved NULL\n");
		*/
	return c;
	/*
	PWord_t pvalue;
	// unsigned char exists_restriction_buffer[16];

	BUILD_EXISTS_RESTRICTION_ID(r,f);
	JSLG(pvalue, tbox->exists_restrictions, exists_restriction_buffer);
	if (pvalue == NULL)
		return NULL;
	return (Concept*) *pvalue;
	*/
}

// put the existential restriction with role r and filler f into hash
void put_exists_restriction(int r, int f, Concept* c, TBox* tbox) {
	unsigned char buffer[16];

	BUILD_EXISTS_RESTRICTION_ID(r, f, buffer);
	/*
	insert_key_value(tbox->exists_restrictions,
			hash_string(tbox->exists_restrictions->bucket_count, buffer),
			(int (*) (void *, void *)) strcmp,
			buffer,
			c);
			*/
	insert_key_value(tbox->exists_restrictions,
			hash_string(buffer),
			c);
	// printf("inserted %d\t%d\t%s\t%d\t%d\n", r, f, buffer, c->description.exists->role->id, c->description.exists->filler->id);
	/*
	PWord_t pvalue;

	BUILD_EXISTS_RESTRICTION_ID(r,f);
	JSLI(pvalue, tbox->exists_restrictions, exists_restriction_buffer);
	if (pvalue == PJERR) {
		fprintf(stderr, "could not insert existential restriction (some %d %d), aborting", r, f);
		exit(EXIT_FAILURE);
	}
	*pvalue = (Word_t) c;
	*/
}

// we assume c->id and d->id to be ordered!
// the caller function is responsible for providing c and d s.t. c->id < d->id !
Concept* get_conjunction(Concept* c, Concept* d, TBox* tbox) {
	PWord_t conjunction_pp;

	BUILD_CONJUNCTION_ID(c->id, d->id);
	JSLG(conjunction_pp, tbox->conjunctions, (unsigned char*) conjunctions_buffer);
	if (conjunction_pp == NULL)
		return NULL;

	return (Concept*) *conjunction_pp;
}

// the conjuncts of c are assumed to be ordered
void put_conjunction(Concept* c, TBox* tbox) {
	PWord_t conjunction_pp;

	BUILD_CONJUNCTION_ID(c->description.conj->conjunct1->id, c->description.conj->conjunct2->id);
	JSLI(conjunction_pp, tbox->conjunctions, (unsigned char*) conjunctions_buffer);
	if (conjunction_pp == PJERR) {
		fprintf(stderr, "could not insert conjunction, aborting\n");
		exit(EXIT_FAILURE);
	}
	*conjunction_pp = (Word_t) c;
}

Role* get_role_composition(Role* r, Role* s, TBox* tbox) {
	PWord_t role_composition_pp;

	BUILD_ROLE_COMPOSITION_ID(r->id, s->id);
	JSLG(role_composition_pp, tbox->role_compositions, (unsigned char*) role_composition_buffer);
	if (role_composition_pp == NULL)
		return NULL;

	return (Role*) *role_composition_pp;
}

void put_role_composition(Role* r, TBox* tbox) {
	PWord_t role_composition_pp;

	BUILD_ROLE_COMPOSITION_ID(r->description.role_composition->role1->id, r->description.role_composition->role2->id);
	JSLI(role_composition_pp, tbox->role_compositions, (unsigned char*) role_composition_buffer);
	if (role_composition_pp == PJERR) {
		fprintf(stderr, "could not insert role composition, aborting\n");
		exit(EXIT_FAILURE);
	}
	*role_composition_pp = (Word_t) r;
}
