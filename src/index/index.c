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
#include "../model/utils.h"
#include "utils.h"

/*
// The list of subclass axioms that are generated during  preprocessing
extern SubClassAxiom** generated_subclass_axioms;
extern int generated_subclass_axiom_count;

// The list of subrole axioms that are generated during  preprocessing
extern SubRoleAxiom** generated_subrole_axioms;
extern int generated_subrole_axiom_count;
*/


void index_concept(ClassExpression* c, TBox* tbox) {

	switch (c->type) {
	case CLASS_TYPE:
	case OBJECT_ONE_OF_TYPE:
		break;
	case OBJECT_INTERSECTION_OF_TYPE:
		index_concept(c->description.conj.conjunct1, tbox);
		index_concept(c->description.conj.conjunct2, tbox);
		add_to_first_conjunct_of_list(c->description.conj.conjunct1, c);
		add_to_second_conjunct_of_list(c->description.conj.conjunct2, c);
		break;
	case OBJECT_SOME_VALUES_FROM_TYPE:
		// add_to_filler_of_list(c->description.exists->filler, c);
		add_to_negative_exists(c, tbox);
		index_concept(c->description.exists.filler, tbox);
		break;
	default:
		fprintf(stderr, "unknown concept type, aborting\n");
		exit(EXIT_FAILURE);
	}
}

void index_role(ObjectPropertyExpression* r) {
	switch (r->type) {
	case OBJECT_PROPERTY_TYPE:
		break;
	case OBJECT_PROPERTY_CHAIN_TYPE:
		// recursive calls are necessary because role1 or role2 can also be a role comoposition
		index_role(r->description.object_property_chain.role1);
		index_role(r->description.object_property_chain.role2);
		add_role_to_first_component_of_list(r->description.object_property_chain.role1, r);
		add_role_to_second_component_of_list(r->description.object_property_chain.role2, r);
		break;
	default:
		fprintf(stderr, "unknown role type, aborting\n");
		exit(EXIT_FAILURE);
	}
}


/*
 * Indexes the given TBox.
 * Returns:
 * 	-1: If the reasoning task is consistency check, and an atomic concept has the
 * 	told subsumer bottom. In this case it immediately returns, i.e., the rest of
 * 	the KB is not indexed!
 * 	1: If the reasoning task is consistency check and bottom does not appear on the
 * 	rhs of any axiom. This means the KB is consistent.
 * 	0: Otherwise
 */
char index_tbox(KB* kb, ReasoningTask reasoning_task) {

	TBox* tbox = kb->tbox;
	char bottom_appears_on_rhs = 0;

	int i;
	SetIterator iterator;
	SET_ITERATOR_INIT(&iterator, &(kb->tbox->subclassof_axioms));
	SubClassOfAxiom* ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT(&iterator);
	while (ax) {
		// Check if bottom appears on the rhs. Needed for consistency
		if (ax->rhs == tbox->bottom_concept) {
			if (ax->lhs != tbox->bottom_concept)
				bottom_appears_on_rhs = 1;
			// if the top concept or a nominal is subsumed by bottom, the kb is inconsistent
			if (ax->lhs->type == OBJECT_ONE_OF_TYPE || ax->lhs == tbox->top_concept)
				// return inconsistent immediately
				return -1;
		}

		// no need to add told subsumers of bottom
		// no need to index the bottom concept
		if (ax->lhs == tbox->bottom_concept)
			continue;
		// no need to add top as a told subsumer
		if (ax->rhs == tbox->top_concept) {
			// still index the lhs, but do not add top to the lhs of rhs
			index_concept(ax->lhs, tbox);
			continue;
		}
		ADD_TOLD_SUBSUMER_CLASS_EXPRESSION(ax->rhs, ax->lhs);
		index_concept(ax->lhs, tbox);
		ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT(&iterator);
	}

	// Now index the subclass axioms generated during preprocessing
	for (i = 0; i < kb->generated_subclass_axiom_count; ++i) {

		// Check if bottom appears on the rhs. Needed for consistency
		if (kb->generated_subclass_axioms[i]->rhs == tbox->bottom_concept)  {
			if (kb->generated_subclass_axioms[i]->lhs != tbox->bottom_concept)
				bottom_appears_on_rhs = 1;
			// if the top concept or a nominal is subsumed by bottom, the kb is inconsistent
			if (kb->generated_subclass_axioms[i]->lhs->type == OBJECT_ONE_OF_TYPE || kb->generated_subclass_axioms[i]->lhs == tbox->top_concept)
				// return inconsistent immediately
				return -1;
		}

		// no need to add told subsumers of bottom
		// no need to index the bottom concept
		if (kb->generated_subclass_axioms[i]->lhs == tbox->bottom_concept)
			continue;
		// no need to add top as a told subsumer
		if (kb->generated_subclass_axioms[i]->rhs == tbox->top_concept) {
			// still index the lhs, but do not add top to the lhs of rhs
			index_concept(kb->generated_subclass_axioms[i]->lhs, tbox);
			continue;
		}
		ADD_TOLD_SUBSUMER_CLASS_EXPRESSION(kb->generated_subclass_axioms[i]->rhs, kb->generated_subclass_axioms[i]->lhs);
		index_concept(kb->generated_subclass_axioms[i]->lhs, tbox);
	}

	// If bottom does not appear on the rhs, the KB cannot be inconcsistent, i.e., it is consistent
	if (reasoning_task == CONSISTENCY && bottom_appears_on_rhs == 0)
		return 1;

	// Index subrole axioms
	for (i = 0; i < tbox->subrole_axiom_count; ++i) {
		ADD_TOLD_SUBSUMER_OBJECT_PROPERTY_EXPRESSION(tbox->subrole_axioms[i]->rhs, tbox->subrole_axioms[i]->lhs);
		index_role(tbox->subrole_axioms[i]->lhs);
	}

	// Index generated subrole axioms
	for (i = 0; i < kb->generated_subrole_axiom_count; ++i) {
		ADD_TOLD_SUBSUMER_OBJECT_PROPERTY_EXPRESSION(kb->generated_subrole_axioms[i]->rhs, kb->generated_subrole_axioms[i]->lhs);
		index_role(kb->generated_subrole_axioms[i]->lhs);
	}

	return 0;
}

// TODO
void index_abox(ABox* abox) {
	int i;

	// index the concept assertions
	for (i = 0; i < abox->concept_assertion_count; i++) {
		// TODO
	}

	// index the role assertions
	for (i = 0; i < abox->role_assertion_count; i++) {
		// TODO
	}
}

char index_kb(KB* kb, ReasoningTask reasoning_task) {
	return index_tbox(kb, reasoning_task);
}
