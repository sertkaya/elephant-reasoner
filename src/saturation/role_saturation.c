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
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../model/datatypes.h"
#include "../model/model.h"
#include "../model/limits.h"
#include "../model/print_utils.h"
#include "../index/index.h"
#include "../index/utils.h"
#include "../hashing/hash_map.h"
#include "../utils/stack.h"
#include "utils.h"

RoleSaturationAxiom* create_role_saturation_axiom(ObjectPropertyExpression* lhs, ObjectPropertyExpression* rhs) {
	RoleSaturationAxiom* ax = (RoleSaturationAxiom*) malloc(sizeof(RoleSaturationAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;

	return ax;
}

int mark_role_saturation_axiom_processed(RoleSaturationAxiom* ax) {
	int added_to_subsumer_list;

	// add to the subsumer list to mark it as processed
	added_to_subsumer_list = add_to_role_subsumer_list(ax->lhs, ax->rhs);

	// add to the subsumees hash too
	add_to_role_subsumee_list(ax->rhs, ax->lhs);

	return added_to_subsumer_list;
}

static inline void print_role_saturation_axiom(KB* kb, RoleSaturationAxiom* ax) {
	char* lhs_str = object_property_expression_to_string(kb, ax->lhs);
	char* rhs_str = object_property_expression_to_string(kb, ax->rhs);
	printf("%s <= %s\n", lhs_str, rhs_str);
	free(lhs_str);
	free(rhs_str);
}

char is_subformula_of(ObjectPropertyExpression* r1, ObjectPropertyExpression* r2) {
	if (r1 == r2)
		return 1;
	if (r2->type == OBJECT_PROPERTY_CHAIN_TYPE) {
		return is_subformula_of(r1, r2->description.object_property_chain.role1) || is_subformula_of(r1, r2->description.object_property_chain.role2);
	}
	return 0;
}

void saturate_roles(KB* kb) {
	RoleSaturationAxiom* ax;
	Stack scheduled_axioms;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	MapIterator map_iterator;

	SetIterator told_subsumers_iterator;
    // the atomic roles
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->object_properties));
	ObjectPropertyExpression* object_property = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property, object_property));
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}

	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax)) {
			// told subsumers
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(ax->rhs->told_subsumers));
			ObjectPropertyExpression* told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer) {
				push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, told_subsumer));
				told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}


	// the role compositions
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->object_property_chains));
	ObjectPropertyExpression* object_property_chain = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property_chain, object_property_chain));
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}


	// reflexive transitive closure of role inclusion axioms and complex role inclusion axioms
	SetIterator subsumees_iterator_1, subsumees_iterator_2, component_of_iterator;
	ObjectPropertyExpression* subsumee_1;
	ObjectPropertyExpression* subsumee_2;
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax)) {
			// print_role_saturation_axiom(kb, ax);
			SET_ITERATOR_INIT(&subsumees_iterator_1, &(ax->lhs->description.object_property_chain.role1->subsumees));
			subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
			while (subsumee_1) {
				SET_ITERATOR_INIT(&subsumees_iterator_2, &(ax->lhs->description.object_property_chain.role2->subsumees));
				subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
				while (subsumee_2) {
					if (!SET_CONTAINS(ax->lhs, &(subsumee_1->subsumees)) && !SET_CONTAINS(ax->lhs, &(subsumee_2->subsumees))) {
						ObjectPropertyExpression* new_composition = get_create_role_composition_binary(
								(ObjectPropertyExpression*) subsumee_1,
								(ObjectPropertyExpression*) subsumee_2,
								kb->tbox);
						// actually we do not need to index the composition if it already existed
						index_role(new_composition);
						push(&scheduled_axioms, create_role_saturation_axiom(new_composition, ax->rhs));
					}
					subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
				}
				subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
			}

			// told subsumers
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(ax->rhs->told_subsumers));
			ObjectPropertyExpression* told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer) {
				push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, told_subsumer));
				told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}


	// the role compositions
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->object_property_chains));
	object_property_chain = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain) {
		printf("%s:\n", object_property_expression_to_string(kb, object_property_chain));
		SET_ITERATOR_INIT(&subsumees_iterator_1, &(object_property_chain->subsumees));
		subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
		while (subsumee_1) {
			printf("\t%s\n", object_property_expression_to_string(kb, subsumee_1));
			subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
		}
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}


}
