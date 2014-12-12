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
	ObjectPropertyExpression* composition = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (composition) {
		push(&scheduled_axioms, create_role_saturation_axiom(composition, composition));
		composition = MAP_ITERATOR_NEXT(&map_iterator);
	}

    // the atomic roles
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->object_properties));
	ObjectPropertyExpression* object_property = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property) {
		push(&scheduled_axioms, create_role_saturation_axiom((ObjectPropertyExpression*) object_property, (ObjectPropertyExpression*) object_property));
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}

    // reflexive transitive closure of role inclusion axioms and complex role inclusion axioms
	SetIterator told_subsumers_iterator;
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax)) {
			print_saturation_axiom(ax);
			// told subsumers
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(ax->rhs->told_subsumers));
			void* told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer) {
			 	push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, (ObjectPropertyExpression*) told_subsumer));
			 	told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}

	// Make a copy of the object property chains since we are going to traverse and
	// modify it at the same time.
	Set tmp_object_property_chains;
	SET_INIT(&tmp_object_property_chains, 32);
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->object_property_chains));
	composition = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (composition) {
		SET_ADD(composition, &tmp_object_property_chains);
		composition = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	}

	printf("=======================================================\n");

	SetIterator subsumees_iterator_1, subsumees_iterator_2, tmp_object_property_chains_iterator;
	ObjectPropertyExpression* subsumee_1;
	ObjectPropertyExpression* subsumee_2;
	SET_ITERATOR_INIT(&tmp_object_property_chains_iterator, &tmp_object_property_chains);
	composition = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&tmp_object_property_chains_iterator);
	while (composition) {

		printf("\ncomposition:");
		print_role((ObjectPropertyExpression*) composition);

		SET_ITERATOR_INIT(&subsumees_iterator_1, &(composition->description.object_property_chain.role1->subsumees));
		subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
		while (subsumee_1) {

			printf("\nsubsumee_1:");
			print_role((ObjectPropertyExpression*) subsumee_1);

			SET_ITERATOR_INIT(&subsumees_iterator_2, &(composition->description.object_property_chain.role2->subsumees));
			subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
			while (subsumee_2) {

				printf("\nsubsumee_2:");
				print_role((ObjectPropertyExpression*) subsumee_2);

				ObjectPropertyExpression* new_composition = get_create_role_composition_binary(
						(ObjectPropertyExpression*) subsumee_1,
						(ObjectPropertyExpression*) subsumee_2,
						tbox);
				// actually we do not need to index the composition if it already existed
				index_role(new_composition);

				printf("\nnew composition:");
				print_role(new_composition);
				/*
				SetIterator subsumers_iterator;
				SET_ITERATOR_INIT(&subsumers_iterator, &(composition->subsumers));
				ObjectPropertyExpression* subsumer = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator);
				// subsumer of composition is a subsumer of the new_composition
				while (subsumer) {

					// printf("\n");
					// print_role(new_composition);
					// printf(" <= ");
					// print_role(subsumer);
					// printf("\n");

					add_to_role_subsumer_list(new_composition, subsumer);
					// CAUTION!: from this point on, subsumees and subsumers are not synchronized.
					// add_to_role_subsumee_list(subsumer, new_composition);
					subsumer = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&told_subsumers_iterator);
				}
				*/
				add_to_role_subsumer_list(new_composition, composition);
				subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
			}
			subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
		}
		composition = SET_ITERATOR_NEXT(&tmp_object_property_chains_iterator);
	}

	// TODO: optimization via removing the redundant subsumers
	// now remove the redundant subsumers

	// push the object properties and chains to the stack
	Stack scheduled_object_property_expressions;
	// initialize the stack
	init_stack(&scheduled_object_property_expressions);
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->object_property_chains));
	composition = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (composition) {
		push(&scheduled_object_property_expressions, composition);
		composition = MAP_ITERATOR_NEXT(&map_iterator);
	}
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->object_properties));
	object_property = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property) {
		push(&scheduled_object_property_expressions,  object_property);
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}

	ObjectPropertyExpression* object_property_expression = (ObjectPropertyExpression*) pop(&scheduled_object_property_expressions);
	SetIterator subsumers_iterator_1;
	while (object_property_expression) {
		SET_ITERATOR_INIT(&subsumers_iterator_1, &(object_property_expression->subsumers));
		ObjectPropertyExpression* subsumer_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_1);
		SetIterator subsumers_iterator_2;
		while (subsumer_1) {
			SET_ITERATOR_INIT(&subsumers_iterator_2, &(object_property_expression->subsumers));
			ObjectPropertyExpression* subsumer_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_2);
			while (subsumer_2) {
				if (subsumer_1 != subsumer_2 && subsumer_1 != object_property_expression && subsumer_2 != object_property_expression && IS_SUBSUMED_BY(subsumer_1, subsumer_2)) {
					SET_REMOVE(subsumer_2, &(object_property_expression->subsumers));
					list_remove(subsumer_2, &(object_property_expression->subsumer_list));
					printf("remove ");
					print_role((ObjectPropertyExpression*) subsumer_2);
					printf(" from ");
					print_role((ObjectPropertyExpression*) object_property_expression);
				}
				subsumer_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_2);
			}
			subsumer_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_1);
		}
		object_property_expression = (ObjectPropertyExpression*) pop(&scheduled_object_property_expressions);
	}

}

