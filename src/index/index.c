/*
 * The ELepHant Reasoner
 *
 * Copyright (C) Baris Sertkaya (sertkaya@fb2.fra-uas.de)
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


void index_class_expression(ClassExpressionId id, KB* kb) {
	// ClassExpression* c = kb->tbox->class_expressions[id];
	switch (kb->tbox->class_expressions[id].type) {
	case CLASS_TYPE:
		// check if the top concept occurs on the lhs of an axiom
		if (id == kb->tbox->top_concept) {
			// used in concept saturation in the initialization rule
			kb->top_occurs_on_lhs = 1;
		}
		break;
	case OBJECT_ONE_OF_TYPE:
		break;
	case OBJECT_INTERSECTION_OF_TYPE:
		index_class_expression(kb->tbox->class_expressions[id].description.conj.conjunct1, kb);
		index_class_expression(kb->tbox->class_expressions[id].description.conj.conjunct2, kb);
		add_to_first_conjunct_of_list(kb->tbox->class_expressions[id].description.conj.conjunct1, id, kb->tbox);
		add_to_second_conjunct_of_list(kb->tbox->class_expressions[id].description.conj.conjunct2, id, kb->tbox);
		break;
	case OBJECT_SOME_VALUES_FROM_TYPE:
		// add_to_filler_of_list(c->description.exists->filler, c);
		add_to_negative_exists(id, kb->tbox);
		index_class_expression(kb->tbox->class_expressions[id].description.exists.filler, kb);
		break;
	default:
		fprintf(stderr, "unknown concept type, aborting\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * Recursively checks if bottom is a given class expression contains bottom as a subexpression.
 * Returns 1 if this is the case, 0 otherwise
 */
char class_expression_contains_bottom(ClassExpressionId id, TBox* tbox)  {
	switch (tbox->class_expressions[id].type) {
	case CLASS_TYPE:
		if (id == tbox->bottom_concept) {
			return 1;
		}
		break;
	case OBJECT_ONE_OF_TYPE:
		break;
	case OBJECT_INTERSECTION_OF_TYPE:
		if (class_expression_contains_bottom(tbox->class_expressions[id].description.conj.conjunct1, tbox)) {
			return 1;
		}
		if (class_expression_contains_bottom(tbox->class_expressions[id].description.conj.conjunct2, tbox)) {
			return 1;
		}
		break;
	case OBJECT_SOME_VALUES_FROM_TYPE:
		if (class_expression_contains_bottom(tbox->class_expressions[id].description.exists.filler, tbox)) {
			return 1;
		}
		break;
	default:
		fprintf(stderr, "unknown concept type, aborting\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}


void index_role(ObjectPropertyExpressionId id, TBox* tbox) {
	switch (tbox->object_property_expressions[id].type) {
	case OBJECT_PROPERTY_TYPE:
		break;
	case OBJECT_PROPERTY_CHAIN_TYPE:
		// recursive calls are necessary because role1 or role2 can also be a role composition
		index_role(tbox->object_property_expressions[id].description.object_property_chain.role1, tbox);
		index_role(tbox->object_property_expressions[id].description.object_property_chain.role2, tbox);
		add_role_to_first_component_of_list(tbox->object_property_expressions[id].description.object_property_chain.role1, id, tbox);
		add_role_to_second_component_of_list(tbox->object_property_expressions[id].description.object_property_chain.role2, id, tbox);
		break;
	default:
		fprintf(stderr, "unknown role type, aborting\n");
		exit(EXIT_FAILURE);
	}
}


/*
 * Index a given ontology: Collect information about told subsumer, negative occurrence of exists restrictions,
 * and conjuncts of the conjunctions.
 * Returns:
 * 	-1: if the reasoning task is consistency check, and an atomic concept or the top concept is
 * 	subsumed by bottom. In this case it immediately returns, i.e., the rest of the KB is not indexed!
 * 	1: if the reasoning task is consistency check, and bottom does not appear on the rhs of any axiom.
 * 	The KB cannot be inconsistent in this case.
 * 	0: if none of the above holds. (In this case, we need to saturate the ontology for checking consistency)
 */
char index_tbox(KB* kb, ReasoningTask reasoning_task) {

	SetIterator_64 iterator;
	SET_ITERATOR_INIT_64(&iterator, &(kb->tbox->subclass_of_axioms));
	SubClassOfAxiom* subclass_ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
	while (subclass_ax) {
		// Check if bottom appears on the rhs. Needed for determining inconsistency already during indexing.
		if (subclass_ax->rhs == kb->tbox->bottom_concept) {
			// if the top concept or a nominal is subsumed by bottom, the ontology is inconsistent
			if (kb->tbox->class_expressions[subclass_ax->lhs].type == OBJECT_ONE_OF_TYPE || subclass_ax->lhs == kb->tbox->top_concept)
				// return inconsistent immediately
				return -1;
		}

		// no need to add told subsumers of bottom
		// no need to index the bottom concept
		if (subclass_ax->lhs == kb->tbox->bottom_concept) {
			subclass_ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
			continue;
		}
		// no need to add top as a told subsumer
		if (subclass_ax->rhs == kb->tbox->top_concept) {
			// still index the lhs, but do not add top to the subsumers of lhs
			index_class_expression(subclass_ax->lhs, kb);
			subclass_ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
			continue;
		}
		ADD_TOLD_SUBSUMER_CLASS_EXPRESSION(subclass_ax->rhs, subclass_ax->lhs, kb->tbox);
		index_class_expression(subclass_ax->lhs, kb);
		if (class_expression_contains_bottom(subclass_ax->rhs, kb->tbox) && subclass_ax->lhs != kb->tbox->bottom_concept)
			kb->bottom_occurs_on_rhs = 1;
		subclass_ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
	}
	// Now index the subclass axioms generated during preprocessing
	SET_ITERATOR_INIT_64(&iterator, &(kb->tbox->generated_subclass_of_axioms));
	subclass_ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
	while (subclass_ax) {
		// Check if bottom appears on the rhs. Needed for consistency
		if (subclass_ax->rhs == kb->tbox->bottom_concept)  {
			// if the top concept or a nominal is subsumed by bottom, the kb is inconsistent
			if (kb->tbox->class_expressions[subclass_ax->lhs].type == OBJECT_ONE_OF_TYPE || subclass_ax->lhs == kb->tbox->top_concept)
				// return inconsistent immediately
				return -1;
		}

		// no need to add told subsumers of bottom
		// no need to index the bottom concept
		if (subclass_ax->lhs == kb->tbox->bottom_concept) {
			subclass_ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
			continue;
		}
		// no need to add top as a told subsumer
		if (subclass_ax->rhs == kb->tbox->top_concept) {
			// still index the lhs, but do not add top to the subsumers of lhs
			index_class_expression(subclass_ax->lhs, kb);
			subclass_ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
			continue;
		}
		ADD_TOLD_SUBSUMER_CLASS_EXPRESSION(subclass_ax->rhs, subclass_ax->lhs, kb->tbox);
		index_class_expression(subclass_ax->lhs, kb);
		if (class_expression_contains_bottom(subclass_ax->rhs, kb->tbox) && subclass_ax->lhs != kb->tbox->bottom_concept)
			kb->bottom_occurs_on_rhs = 1;
		subclass_ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
	}

	// If bottom does not appear on the rhs, the KB cannot be inconsistent, i.e., it is consistent
	if (reasoning_task == CONSISTENCY && kb->bottom_occurs_on_rhs == 0)
		return 1;

	// Index subobjectpropertyof axioms
	SET_ITERATOR_INIT_64(&iterator, &(kb->tbox->subobjectproperty_of_axioms));
	SubObjectPropertyOfAxiom* subrole_ax = (SubObjectPropertyOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
	while (subrole_ax) {
		ADD_TOLD_SUBSUMER_OBJECT_PROPERTY_EXPRESSION(subrole_ax->rhs, subrole_ax->lhs, kb->tbox);
		index_role(subrole_ax->lhs, kb->tbox);
		subrole_ax = (SubObjectPropertyOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
	}

	// Index generated subrole axioms
	SET_ITERATOR_INIT_64(&iterator, &(kb->tbox->generated_subobjectproperty_of_axioms));
	subrole_ax = (SubObjectPropertyOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
	while (subrole_ax) {
		ADD_TOLD_SUBSUMER_OBJECT_PROPERTY_EXPRESSION(subrole_ax->rhs, subrole_ax->lhs, kb->tbox);
		index_role(subrole_ax->lhs, kb->tbox);
		subrole_ax = (SubObjectPropertyOfAxiom*) SET_ITERATOR_NEXT_64(&iterator);
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
