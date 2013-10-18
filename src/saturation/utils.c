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

#include "locals.h"
#include "datatypes.h"
#include "../model/datatypes.h"
#include "../model/utils.h"

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

// add p to the predecessors hash of c
// the key of the predecessors hash is r
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

// add s to the successors hash of c.
// the key of the successors hash of c is r
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


ConceptSaturationAxiom* create_concept_saturation_axiom(Concept* lhs, Concept* rhs, enum saturation_axiom_type type) {
	ConceptSaturationAxiom* ax = (ConceptSaturationAxiom*) malloc(sizeof(ConceptSaturationAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	ax->type = type;
	return ax;
}

RoleSaturationAxiom* create_role_saturation_axiom(Role* lhs, Role* rhs) {
	RoleSaturationAxiom* ax = (RoleSaturationAxiom*) malloc(sizeof(RoleSaturationAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;

	return ax;
}

int mark_role_saturation_axiom_processed(RoleSaturationAxiom* ax) {
	int added_to_subsumer_list;

	added_to_subsumer_list = add_to_role_subsumer_list(ax->lhs, ax->rhs);
	return added_to_subsumer_list;
}
