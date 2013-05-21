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

int mark_concept_saturation_axiom_processed(ConceptSaturationAxiom* ax) {
	int added_to_subsumer_list;

	/*
	 // (replace with a call to the function add_subsumer, check perf. change. first move the function from package hierarchy to model)
	 J1S(added_to_subsumer_list, ax->lhs->subsumers, (Word_t) ax->rhs);
	 if (added_to_subsumer_list == JERR) {
		 fprintf(stderr, "could not mark axiom as processed, aborting\n");
		 exit(EXIT_FAILURE);
	 }
	*/

	added_to_subsumer_list = add_to_concept_subsumer_list(ax->lhs, ax->rhs);

	/*
	Concept** tmp;
	J1S(added_to_subsumer_list, ax->lhs->subsumers, (Word_t) ax->rhs);
	// if (added_to_subsumer_list == JERR) {
	// 	fprintf(stderr, "could not add to subsumer list, aborting\n");
	// 	exit(EXIT_FAILURE);
	// }
	if (added_to_subsumer_list) {
		tmp = realloc(ax->lhs->subsumer_list, (ax->lhs->subsumer_count + 1) * sizeof(Concept*));
		assert(tmp != NULL);
		ax->lhs->subsumer_list = tmp;
		ax->lhs->subsumer_list[ax->lhs->subsumer_count] = ax->rhs;
		ax->lhs->subsumer_count++;
	}
	*/

	return added_to_subsumer_list;
}

// add c to the predecessors hash of the filler of ex.
// the key of the predecessors hash of filler of ex is ex->description.exists->role.
// the value is a bitmap, whose index is c
int add_predecessor(Concept* c, Concept* ex) {
	// PWord_t predecessors_bitmap_p;
	PPvoid_t predecessors_bitmap_p;
	// Pvoid_t predecessors_bitmap;
	int inserted_predecessor = 0;

	JLI(predecessors_bitmap_p, ex->description.exists->filler->predecessors, (Word_t) ex->description.exists->role);
	if (predecessors_bitmap_p == PJERR) {
		fprintf(stderr, "could not insert into predecessor map, aborting\n");
		exit(EXIT_FAILURE);
	}

	// check if we are inserting a predecessor for this role for the first time
	if (*predecessors_bitmap_p == 0) {
		// predecessors_bitmap = (Pvoid_t) NULL;
		// *predecessors_bitmap_p = (Word_t) predecessors_bitmap;
		*predecessors_bitmap_p = (Pvoid_t) NULL;
	}

	// predecessors_bitmap =  (Pvoid_t) *predecessors_bitmap_p;
	// J1S(inserted_predecessor, predecessors_bitmap, (Word_t) c);
	J1S(inserted_predecessor, *predecessors_bitmap_p, (Word_t) c);
	if (inserted_predecessor == JERR) {
		fprintf(stderr, "could not insert into predecessor bitmap, aborting\n");
		exit(EXIT_FAILURE);
	}

	return inserted_predecessor;
}

ConceptSaturationAxiom* create_concept_saturation_axiom(Concept* lhs, Concept* rhs, char derived_conjunction, char derived_exists, enum saturation_axiom_type type) {
	ConceptSaturationAxiom* ax = (ConceptSaturationAxiom*) malloc(sizeof(ConceptSaturationAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	ax->derived_via_conj_introduction = derived_conjunction;
	ax->derived_via_exists_introduction = derived_exists;
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
