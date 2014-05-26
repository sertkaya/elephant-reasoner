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
#include <stdlib.h>
#include <assert.h>

#include "../model/datatypes.h"
#include "../model/limits.h"
#include "../hashing/key_value_hash_table.h"



/******************************************************************************
 * functions for adding subsumer/told subsumer to a concept
 *****************************************************************************/
// TODO: what about multiple occurrences? Performance?
// add s to the told subsumer list of c
// note that multiple occurrence of s is allowed!
void add_told_subsumer_concept(Concept* c, Concept* s) {
	Concept** tmp;

	tmp = realloc(c->told_subsumers, (c->told_subsumer_count + 1) * sizeof(Concept*));
	assert(tmp != NULL);
	c->told_subsumers = tmp;
	c->told_subsumers[c->told_subsumer_count] = s;
	c->told_subsumer_count++;
}

// add concept s to the subsumer list of concept c
// returns 1 if added, 0 otherwise
// note that it maintains both the array and the judy array
// of subsumers. the reason for keeping the subsumers twice
// is performance in saturation
// int add_to_concept_subsumer_list(Concept* c, Concept* s) {
void add_to_concept_subsumer_list(Concept* c, Concept* s) {
	Concept** tmp;

	tmp = realloc(c->subsumer_list, (c->subsumer_count + 1) * sizeof(Concept*));
	assert(tmp != NULL);
	c->subsumer_list = tmp;
	c->subsumer_list[c->subsumer_count] = s;
	c->subsumer_count++;
}


/******************************************************************************
 * functions for adding subsumer/told subsumer to a role
 *****************************************************************************/
// TODO: what about multiple occurrences? Performance?
// add s to the told subsumer list of c
// note that multiple occurrence of s is allowed!
void add_told_subsumer_role(Role* r, Role* s) {
	/*
	Role** tmp;

	tmp = realloc(r->told_subsumers, (r->told_subsumer_count + 1) * sizeof(Role*));
	assert(tmp != NULL);
	r->told_subsumers = tmp;
	r->told_subsumers[r->told_subsumer_count] = s;
	r->told_subsumer_count++;
	*/
	insert_key_value(r->told_subsumers, s->id, s);
}

// add role s to the subsumee list of  r
void add_told_subsumee_role(Role* r, Role* s) {
	insert_key_value(r->told_subsumees, s->id, s);
}

// add role s to the subsumer list of r
// returns 1 if added, 0 otherwise
// note that it maintains both the array and the hash table
// of subsumers. the reason for keeping the subsumers twice
// is performance in saturation
int add_to_role_subsumer_list(Role* r, Role* s) {
	Role** tmp;

	if (insert_key(r->subsumers, s->id)) {
		tmp = realloc(r->subsumer_list, (r->subsumer_count + 1) * sizeof(Role*));
		assert(tmp != NULL);
		r->subsumer_list = tmp;
		r->subsumer_list[r->subsumer_count] = s;
		r->subsumer_count++;

		return 1;
	}

	return 0;
}

int add_to_role_subsumee_list(Role*r, Role* s) {
	Role** tmp;

	if (insert_key(r->subsumees, s->id)) {
		tmp = realloc(r->subsumee_list, (r->subsumee_count + 1) * sizeof(Role*));
		assert(tmp != NULL);
		r->subsumee_list = tmp;
		r->subsumee_list[r->subsumee_count] = s;
		++r->subsumee_count;

		return 1;
	}

	return 0;
}

/******************************************************************************
 * functions for adding ...
 *****************************************************************************/
// add 'conjunction' to the list of conjunctions whose first conjunct is 'concept'
// note that for performance reasons in saturation, this information is kept twice:
// once in a hash table, once in a usual array. the hash table is for searching
// during saturation, the usual array is for iteration on the elements. the memory
// overhead is worth the performance gain.

void add_to_first_conjunct_of_list(Concept* concept, Concept* conjunction) {
	Concept** tmp;

	// create the hash if we are adding it for the first time
	if (concept->first_conjunct_of == NULL)
		concept->first_conjunct_of = create_key_hash_table(DEFAULT_FIRST_CONJUNCT_OF_HASH_SIZE);

	if (insert_key(concept->first_conjunct_of, conjunction->id)) {
		tmp = realloc(concept->first_conjunct_of_list, (concept->first_conjunct_of_count + 1) * sizeof(Concept*));
		assert(tmp != NULL);
		concept->first_conjunct_of_list = tmp;
		concept->first_conjunct_of_list[concept->first_conjunct_of_count] = conjunction;
		concept->first_conjunct_of_count++;
	}
}

// add 'conjunction' to the list of conjunctions whose second conjunct is 'concept'
// (see the note for the function add_to_first_conjunct_of_list above)
void add_to_second_conjunct_of_list(Concept* concept, Concept* conjunction) {
	Concept** tmp;

	// create the hash if we are adding it for the first time
	if (concept->second_conjunct_of == NULL)
		concept->second_conjunct_of = create_key_hash_table(DEFAULT_SECOND_CONJUNCT_OF_HASH_SIZE);

	if (insert_key(concept->second_conjunct_of, conjunction->id)) {
		tmp = realloc(concept->second_conjunct_of_list, (concept->second_conjunct_of_count + 1) * sizeof(Concept*));
		assert(tmp != NULL);
		concept->second_conjunct_of_list = tmp;
		concept->second_conjunct_of_list[concept->second_conjunct_of_count] = conjunction;
		concept->second_conjunct_of_count++;
	}
}

// add ex to the filler_of_negative_exists hash of the filler of ex.
void add_to_negative_exists(Concept* ex, TBox* tbox) {
	// We allocate the space here (in indexing)
	if (ex->description.exists->filler->filler_of_negative_exists == NULL) {
		ex->description.exists->filler->filler_of_negative_exists =
				(Concept**) calloc(tbox->atomic_role_count + tbox->unique_binary_role_composition_count, sizeof(Concept*));
		assert(ex->description.exists->filler->filler_of_negative_exists != NULL);
	}
	ex->description.exists->filler->filler_of_negative_exists[ex->description.exists->role->id] = ex;
}

// add 'composition' to the list of compositions whose first component is 'role'
// note that for performance reasons in saturation, this information is kept twice:
// once in a hash table, once in a usual array. the hash table is for searching
// during saturation, the usual array is for iteration on the elements. the memory
// overhead is worth the performance gain.
void add_role_to_first_component_of_list(Role* role, Role* composition) {
	Role** tmp;

	if (insert_key(role->first_component_of, composition->id)) {
		tmp = realloc(role->first_component_of_list, (role->first_component_of_count + 1) * sizeof(Role*));
		assert(tmp != NULL);
		role->first_component_of_list = tmp;
		role->first_component_of_list[role->first_component_of_count] = composition;
		role->first_component_of_count++;
	}
}

// add 'composition' to the list of compositions whose second component is 'role'
// (see the note for the function add_role_to_second_component_of_list above)
void add_role_to_second_component_of_list(Role* role, Role* composition) {
	Role** tmp;

	if (insert_key(role->second_component_of, composition->id)) {
		tmp = realloc(role->second_component_of_list, (role->second_component_of_count + 1) * sizeof(Concept*));
		assert(tmp != NULL);
		role->second_component_of_list = tmp;
		role->second_component_of_list[role->second_component_of_count] = composition;
		role->second_component_of_count++;
	}
}
