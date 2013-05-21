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


#include <assert.h>

#include "../model/datatypes.h"



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

/*
	int added_told_subsumer;

	J1S(added_told_subsumer, c->told_subsumers, (Word_t) s);
	if (added_told_subsumer == JERR) {
		fprintf(stderr, "could not add told subsumer, aborting\n");
		exit(EXIT_FAILURE);
	}
	*/
}

// add concept s to the subsumer list of concept c
// returns 1 if added, 0 otherwise
// note that it maintains both the array and the judy array
// of subsumers. the reason for keeping the subsumers twice
// is performance in saturation
int add_to_concept_subsumer_list(Concept* c, Concept* s) {
	int added_to_subsumer_list;
	Concept** tmp;

	J1S(added_to_subsumer_list, c->subsumers, (Word_t) s);
	if (added_to_subsumer_list == JERR) {
		fprintf(stderr, "could not add to subsumer list, aborting\n");
		exit(EXIT_FAILURE);
	}
	if (added_to_subsumer_list) {
		tmp = realloc(c->subsumer_list, (c->subsumer_count + 1) * sizeof(Concept*));
		assert(tmp != NULL);
		c->subsumer_list = tmp;
		c->subsumer_list[c->subsumer_count] = s;
		c->subsumer_count++;
	}

	return added_to_subsumer_list;
}


/******************************************************************************
 * functions for adding subsumer/told subsumer to a role
 *****************************************************************************/
// TODO: what about multiple occurrences? Performance?
// add s to the told subsumer list of c
// note that multiple occurrence of s is allowed!
void add_told_subsumer_role(Role* r, Role* s) {
	Role** tmp;

	tmp = realloc(r->told_subsumers, (r->told_subsumer_count + 1) * sizeof(Role*));
	assert(tmp != NULL);
	r->told_subsumers = tmp;
	r->told_subsumers[r->told_subsumer_count] = s;
	r->told_subsumer_count++;
}

// add role s to the subsumer list of concept r
// returns 1 if added, 0 otherwise
// note that it maintains both the array and the judy array
// of subsumers. the reason for keeping the subsumers twice
// is performance in saturation
int add_to_role_subsumer_list(Role* r, Role* s) {
	int added_to_subsumer_list;
	Role** tmp;

	J1S(added_to_subsumer_list, r->subsumers, (Word_t) s);
	if (added_to_subsumer_list == JERR) {
		fprintf(stderr, "could not add to subsumer list, aborting\n");
		exit(EXIT_FAILURE);
	}
	if (added_to_subsumer_list) {
		tmp = realloc(r->subsumer_list, (r->subsumer_count + 1) * sizeof(Role*));
		assert(tmp != NULL);
		r->subsumer_list = tmp;
		r->subsumer_list[r->subsumer_count] = s;
		r->subsumer_count++;
	}

	return added_to_subsumer_list;
}


/******************************************************************************
 * functions for adding ...
 *****************************************************************************/
// add 'conjunction' to the list of conjunctions whose first conjunct is 'concept'
// note that for performance reasons in saturation, this information is kept twice:
// once in a judy array, once in a usual array. the judy array is for searching
// during saturation, the usual array is for iteration on the elements. the memory
// overhead is worth the performance gain.
void add_to_first_conjunct_of_list(Concept* concept, Concept* conjunction) {
	int added_to_conjunct_of_list;
	Concept** tmp;

	J1S(added_to_conjunct_of_list, concept->first_conjunct_of, (Word_t) conjunction);
	if (added_to_conjunct_of_list == JERR) {
		fprintf(stderr, "could not add to first conjunct of list, aborting\n");
		exit(EXIT_FAILURE);
	}

	if (added_to_conjunct_of_list) {
		tmp = realloc(concept->first_conjunct_of_list, (concept->first_conjunct_of_count + 1) * sizeof(Concept*));
		assert(tmp != NULL);
		concept->first_conjunct_of_list = tmp;
		concept->first_conjunct_of_list[concept->first_conjunct_of_count] = conjunction;
		concept->first_conjunct_of_count++;
	}
}

// add 'conjunction' to the list of conjunctions whose second conjunct is 'concept'
// (see the note for the function add_to_second_conjunct_of_list above)
void add_to_second_conjunct_of_list(Concept* concept, Concept* conjunction) {
	int added_to_conjunct_of_list;
	Concept** tmp;

	J1S(added_to_conjunct_of_list, concept->second_conjunct_of, (Word_t) conjunction);
	if (added_to_conjunct_of_list == JERR) {
		fprintf(stderr, "could not add to second conjunct of list, aborting\n");
		exit(EXIT_FAILURE);
	}
	if (added_to_conjunct_of_list) {
		tmp = realloc(concept->second_conjunct_of_list, (concept->second_conjunct_of_count + 1) * sizeof(Concept*));
		assert(tmp != NULL);
		concept->second_conjunct_of_list = tmp;
		concept->second_conjunct_of_list[concept->second_conjunct_of_count] = conjunction;
		concept->second_conjunct_of_count++;
	}
}

// add ex to the filler_of_negative_exists hash of the filler of ex.
// key of the hash is ex->description.exists->role, the value is ex.
void add_to_negative_exists(Concept* ex) {
	PWord_t neg_exists_pp;

	// JLI(neg_exists_pp, c->filler_of_negative_exits, (Word_t) ex->description.exists->role);
	JLI(neg_exists_pp, ex->description.exists->filler->filler_of_negative_exists, (Word_t) ex->description.exists->role);
	if (neg_exists_pp == PJERR) {
		fprintf(stderr, "could not insert into the negative existentials hash, aborting\n");
		exit(EXIT_FAILURE);
	}
	*neg_exists_pp = (Word_t) ex;
}

// return the negative existential restriction whose role is r and filler is c
// returns NULL if no such existential restriction is found
Concept* get_negative_exists(Concept* c, Role* r) {
	PWord_t neg_exists_pp;

	JLG(neg_exists_pp, c->filler_of_negative_exists, (Word_t) r);
	if (neg_exists_pp == NULL)
		return NULL;

	return (Concept*) *neg_exists_pp;
}
