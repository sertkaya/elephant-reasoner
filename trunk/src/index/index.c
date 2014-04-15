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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "index.h"
#include "../model/tbox/datatypes.h"
#include "../model/tbox/utils.h"
#include "utils.h"




void index_concept(Concept* c, TBox* tbox) {

	// c->occurs_on_lhs = 1;

	switch (c->type) {
	case ATOMIC_CONCEPT:
		break;
	case CONJUNCTION:
		index_concept(c->description.conj->conjunct1, tbox);
		index_concept(c->description.conj->conjunct2, tbox);
		add_to_first_conjunct_of_list(c->description.conj->conjunct1, c);
		add_to_second_conjunct_of_list(c->description.conj->conjunct2, c);
		break;
	case EXISTENTIAL_RESTRICTION:
		// add_to_filler_of_list(c->description.exists->filler, c);
		add_to_negative_exists(c, tbox);
		index_concept(c->description.exists->filler, tbox);
		break;
	default:
		fprintf(stderr, "unknown concept type, aborting\n");
		exit(EXIT_FAILURE);
	}
}

void index_role(Role* r) {
	switch (r->type) {
	case ATOMIC_ROLE:
		break;
	case ROLE_COMPOSITION:
		// recursive calls are necessary because role1 or role2 can also be a role comoposition
		index_role(r->description.role_composition->role1);
		index_role(r->description.role_composition->role2);
		add_role_to_first_component_of_list(r->description.role_composition->role1, r);
		add_role_to_second_component_of_list(r->description.role_composition->role2, r);
		break;
	default:
		fprintf(stderr, "unknown role type, aborting\n");
		exit(EXIT_FAILURE);
	}
}


void index_tbox(TBox* tbox) {
	int i;

	for (i = 0; i < tbox->subclass_axiom_count; i++) {
		// no need to add told subsumers of bottom
		// no need to index the bottom concept
		if (tbox->subclass_axioms[i]->lhs == tbox->bottom_concept)
			continue;
		// no need to add top as a told subsumer
		if (tbox->subclass_axioms[i]->rhs == tbox->top_concept) {
			// still index the lhs, but do not add top to the lhs of rhs
			index_concept(tbox->subclass_axioms[i]->lhs, tbox);
			continue;
		}
		add_told_subsumer_concept(tbox->subclass_axioms[i]->lhs, tbox->subclass_axioms[i]->rhs);
		// add_to_subsumer_list(tbox->subclass_axioms[i]->lhs, tbox->subclass_axioms[i]->rhs);
		index_concept(tbox->subclass_axioms[i]->lhs, tbox);
	}

	/*
	 * now done in preprocessing
	for (i = 0; i < tbox->eqclass_axiom_count; i++) {
		add_told_subsumer_concept(tbox->eqclass_axioms[i]->lhs, tbox->eqclass_axioms[i]->rhs);
		// add_to_subsumer_list(tbox->subclass_axioms[i]->lhs, tbox->subclass_axioms[i]->rhs);
		index_concept(tbox->eqclass_axioms[i]->lhs, tbox);

		add_told_subsumer_concept(tbox->eqclass_axioms[i]->rhs, tbox->eqclass_axioms[i]->lhs);
		// add_to_subsumer_list(tbox->subclass_axioms[i]->rhs, tbox->subclass_axioms[i]->lhs);
		index_concept(tbox->eqclass_axioms[i]->rhs, tbox);
	}
	*/

	for (i = 0; i < tbox->subrole_axiom_count; ++i) {
		add_told_subsumer_role(tbox->subrole_axioms[i]->lhs, tbox->subrole_axioms[i]->rhs);
		add_told_subsumee_role(tbox->subrole_axioms[i]->rhs, tbox->subrole_axioms[i]->lhs);
		index_role(tbox->subrole_axioms[i]->lhs);
	}

	/*
	 * now done in preprocessing
	for (i = 0; i < tbox->eqrole_axiom_count; ++i) {
		add_told_subsumer_role(tbox->eqrole_axioms[i]->lhs, tbox->eqrole_axioms[i]->rhs);
		add_told_subsumee_role(tbox->eqrole_axioms[i]->rhs, tbox->eqrole_axioms[i]->lhs);
		index_role(tbox->subrole_axioms[i]->lhs);

		add_told_subsumer_role(tbox->eqrole_axioms[i]->rhs, tbox->eqrole_axioms[i]->lhs);
		add_told_subsumee_role(tbox->eqrole_axioms[i]->lhs, tbox->eqrole_axioms[i]->rhs);
		index_role(tbox->subrole_axioms[i]->rhs);
	}
	*/

}
