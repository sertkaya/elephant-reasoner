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
#include <Judy.h>

#include "../model/datatypes.h"
#include "utils.h"

// For marking an axiom as processed, we add the rhs to the subsumers
// of the lhs. The function returns 1 if it is added, 0 if the rhs was
// already in the list of subsumers of the lhs.
// now as macro in utils.h
/*
int mark_concept_saturation_axiom_processed(ConceptSaturationAxiom* ax) {
	int added_to_subsumer_list;

	added_to_subsumer_list = add_to_concept_subsumer_list(ax->lhs, ax->rhs);

	return added_to_subsumer_list;
}
*/

/*
int is_subsumed_by(Concept* c1, Concept* c2) {
	int is_subsumer;

	J1T(is_subsumer, processed_axioms, HASH(c1->id, c2->id));
	// J1T(is_subsumer, processed_axioms, hash_it(c1->id, c2->id));
	return is_subsumer;
}
*/

// add p to the predecessors hash of c
// the key of the predecessors hash is r
/*
int add_predecessor(Concept* c, Role* r, Concept* p) {
	PPvoid_t predecessors_bitmap_p;
	int inserted_predecessor = 0;

	JLI(predecessors_bitmap_p, c->predecessors, (Word_t) r);
	if (predecessors_bitmap_p == PJERR) {
		fprintf(stderr, "could not insert into predecessor map, aborting\n");
		exit(EXIT_FAILURE);
	}

	// check if we are inserting a predecessor for this role for the first time
	if (*predecessors_bitmap_p == 0) {
		*predecessors_bitmap_p = (Pvoid_t) NULL;
	}

	J1S(inserted_predecessor, *predecessors_bitmap_p, (Word_t) p);
	if (inserted_predecessor == JERR) {
		fprintf(stderr, "could not insert into predecessor bitmap, aborting\n");
		exit(EXIT_FAILURE);
	}

	return inserted_predecessor;
}
*/

int add_predecessor(Concept* c, Role* r, Concept* p, TBox* tbox) {

	// first check if we already have a link for role r
	int i, j;
	void* tmp;
	for (i = 0; i < c->predecessor_r_count; ++i)
		if (c->predecessors[i]->role == r) {
			// yes, we have a link for role r, now check if we already have p in its fillers list
			for (j = 0; j < c->predecessors[i]->filler_count; ++j)
				if (c->predecessors[i]->fillers[j] == p)
					return 0;
			// no we do not have p in this list, add it
			tmp = realloc(c->predecessors[i]->fillers, (c->predecessors[i]->filler_count + 1) * sizeof(Concept*));
			assert(tmp != NULL);
			c->predecessors[i]->fillers = (Concept**) tmp;
			c->predecessors[i]->fillers[c->predecessors[i]->filler_count] = p;
			++c->predecessors[i]->filler_count;
			return 1;
		}
	// no, we do not already have a link for role r, create it, add p to its filler list
	// 1) extend the list for links
	tmp = realloc(c->predecessors, (c->predecessor_r_count + 1) * sizeof(Link*));
	assert(tmp != NULL);
	c->predecessors = (Link**) tmp;
	// 2) create the the r-predecessors list of c
	c->predecessors[c->predecessor_r_count] = calloc(1, sizeof(Link));
	assert(c->predecessors[c->predecessor_r_count] != NULL);
	// 3) fill the fields of the new link
	c->predecessors[c->predecessor_r_count]->role = r;
	c->predecessors[c->predecessor_r_count]->fillers = calloc(1, sizeof(Concept*));
	assert(c->predecessors[c->predecessor_r_count]->fillers != NULL);
	c->predecessors[c->predecessor_r_count]->fillers[0] = p;
	c->predecessors[c->predecessor_r_count]->filler_count = 1;

	// finally increment the r_count
	++c->predecessor_r_count;

	return 1;
	/*
	// first check if r-predecessor of c has already been added
	for (i = 0; i < c->predecessor_counts[r->id]; ++i)
		if (c->predecessors[r->id][i] == p)
			return 0;

	// now add it
	Concept **tmp = realloc(c->predecessors[r->id], (c->predecessor_counts[r->id] + 1) * sizeof(Concept*));
	assert(tmp != NULL);
	c->predecessors[r->id] = tmp;
	c->predecessors[r->id][c->predecessor_counts[r->id]] = p;
	// increment the count
	++c->predecessor_counts[r->id];

	return 1;
	*/
}


// add s to the successors hash of c.
// the key of the successors hash of c is r
/*
int add_successor(Concept* c, Role* r, Concept* s) {
	PPvoid_t successors_bitmap_p;
	int inserted_successor = 0;

	JLI(successors_bitmap_p, c->successors, (Word_t) r);
	if (successors_bitmap_p == PJERR) {
		fprintf(stderr, "could not insert into successor map, aborting\n");
		exit(EXIT_FAILURE);
	}

	// check if we are inserting a successor for this role for the first time
	if (*successors_bitmap_p == 0) {
		*successors_bitmap_p = (Pvoid_t) NULL;
	}

	J1S(inserted_successor, *successors_bitmap_p, (Word_t) s);
	if (inserted_successor == JERR) {
		fprintf(stderr, "could not insert into successor bitmap, aborting\n");
		exit(EXIT_FAILURE);
	}

	return inserted_successor;
}
*/

int add_successor(Concept* c, Role* r, Concept* p, TBox* tbox) {

	// first check if we already have a link for role r
	int i, j;
	void* tmp;
	for (i = 0; i < c->successor_r_count; ++i)
		if (c->successors[i]->role == r) {
			// yes, we have a link for role r, now check if we already have p in its fillers list
			for (j = 0; j < c->successors[i]->filler_count; ++j)
				if (c->successors[i]->fillers[j] == p)
					return 0;
			// no we do not have p in this list, add it
			tmp = realloc(c->successors[i]->fillers, (c->successors[i]->filler_count + 1) * sizeof(Concept*));
			assert(tmp != NULL);
			c->successors[i]->fillers = (Concept**) tmp;
			c->successors[i]->fillers[c->successors[i]->filler_count] = p;
			++c->successors[i]->filler_count;
			return 1;
		}
	// no, we do not already have a link for role r, create it, add p to its filler list
	// 1) extend the list for links
	tmp = realloc(c->successors, (c->successor_r_count + 1) * sizeof(Link*));
	assert(tmp != NULL);
	c->successors = (Link**) tmp;
	// 2) create the the r-successors list of c
	c->successors[c->successor_r_count] = calloc(1, sizeof(Link));
	assert(c->successors[c->successor_r_count] != NULL);
	// 3) fill the fields of the new link
	c->successors[c->successor_r_count]->role = r;
	c->successors[c->successor_r_count]->fillers = calloc(1, sizeof(Concept*));
	assert(c->successors[c->successor_r_count]->fillers != NULL);
	c->successors[c->successor_r_count]->fillers[0] = p;
	c->successors[c->successor_r_count]->filler_count = 1;

	// finally increment the r_count
	++c->successor_r_count;

	return 1;
}
