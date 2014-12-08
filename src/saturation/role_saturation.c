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

static inline void print_saturation_axiom(RoleSaturationAxiom* ax) {
	printf("\n");
	print_role(ax->lhs);
	printf(" <= ");
	print_role(ax->rhs);
	printf("\n");
}


void saturate_roles(TBox* tbox) {
	RoleSaturationAxiom* ax;
	Stack scheduled_axioms;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	MapIterator map_iterator;

	// the role compositions
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->object_property_chains));
	void* composition = MAP_ITERATOR_NEXT(&map_iterator);
	while (composition) {
		push(&scheduled_axioms, create_role_saturation_axiom((ObjectPropertyExpression*) composition, (ObjectPropertyExpression*) composition));
		composition = MAP_ITERATOR_NEXT(&map_iterator);
	}

    // the atomic roles
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->object_properties));
	void* object_property = MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property) {
		push(&scheduled_axioms, create_role_saturation_axiom((ObjectPropertyExpression*) object_property, (ObjectPropertyExpression*) object_property));
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}


    // reflexive transitive closure of role inclusion axioms and complex role inclusion axioms
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax)) {
			print_saturation_axiom(ax);
			// told subsumers
			SetIterator told_subsumers_iterator;
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(ax->rhs->told_subsumers));
			void* told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer) {
			 	push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, (ObjectPropertyExpression*) told_subsumer));
			 	told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			}

			SetIterator first_component_of_iterator;
			SET_ITERATOR_INIT(&first_component_of_iterator, &(ax->rhs->first_component_of));
			void* first_component_of = SET_ITERATOR_NEXT(&first_component_of_iterator);
			while (first_component_of && !IS_SUBSUMED_BY(((ObjectPropertyExpression*) first_component_of), ax->rhs)) {
			// while (first_component_of) {
				ObjectPropertyExpression* composition = get_create_role_composition_binary(ax->lhs,
						((ObjectPropertyExpression*) first_component_of)->description.object_property_chain.role2,
						tbox);
				// actually we do not need to index the composition if it already existed
				index_role(composition);

				SET_ITERATOR_INIT(&told_subsumers_iterator, &(((ObjectPropertyExpression*) first_component_of)->told_subsumers));
				told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
				while (told_subsumer) {
					push(&scheduled_axioms, create_role_saturation_axiom(composition, told_subsumer));
					told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
				}

				// push(&scheduled_axioms, create_role_saturation_axiom(composition, (ObjectPropertyExpression*) first_component_of));
				first_component_of = SET_ITERATOR_NEXT(&first_component_of_iterator);
			}

			SetIterator second_component_of_iterator;
			SET_ITERATOR_INIT(&second_component_of_iterator, &(ax->rhs->second_component_of));
			void* second_component_of = SET_ITERATOR_NEXT(&second_component_of_iterator);
			while (second_component_of && !IS_SUBSUMED_BY(((ObjectPropertyExpression*) second_component_of), ax->rhs)) {
			// while (second_component_of) {
				ObjectPropertyExpression* composition = get_create_role_composition_binary(
						((ObjectPropertyExpression*) second_component_of)->description.object_property_chain.role1,
						ax->lhs,
						tbox);
				// actually we do not need to index the composition if it already existed
				index_role(composition);

				SET_ITERATOR_INIT(&told_subsumers_iterator, &(((ObjectPropertyExpression*) second_component_of)->told_subsumers));
				told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
				while (told_subsumer) {
					push(&scheduled_axioms, create_role_saturation_axiom(composition, told_subsumer));
					told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
				}

				// push(&scheduled_axioms, create_role_saturation_axiom(composition, (ObjectPropertyExpression*) second_component_of));
				second_component_of = SET_ITERATOR_NEXT(&second_component_of_iterator);
			}

			if (ax->lhs->type == OBJECT_PROPERTY_CHAIN_TYPE) {
				SetIterator subsumees_iterator_1;
				SET_ITERATOR_INIT(&subsumees_iterator_1, &(ax->lhs->description.object_property_chain.role1->subsumees));
				void* subsumee_1 = SET_ITERATOR_NEXT(&subsumees_iterator_1);
				while (subsumee_1) {
					SetIterator subsumees_iterator_2;
					SET_ITERATOR_INIT(&subsumees_iterator_2, &(ax->lhs->description.object_property_chain.role2->subsumees));
					void* subsumee_2 = SET_ITERATOR_NEXT(&subsumees_iterator_2);
					while (subsumee_2) {
						ObjectPropertyExpression* composition = get_create_role_composition_binary(
								(ObjectPropertyExpression*) subsumee_1,
								(ObjectPropertyExpression*) subsumee_2,
								tbox);
						// actually we do not need to index the composition if it already existed
						index_role(composition);
						push(&scheduled_axioms, create_role_saturation_axiom(composition, ax->rhs));
						subsumee_2 = SET_ITERATOR_NEXT(&subsumees_iterator_2);
					}
					subsumee_1 = SET_ITERATOR_NEXT(&subsumees_iterator_1);
				}
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}
}

