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
#include "../utils/queue.h"
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


int mark_role_saturation_axiom_processed_2(RoleSaturationAxiom* ax) {
	int added_to_subsumer_list;

	// add to the subsumer list to mark it as processed
	added_to_subsumer_list = add_to_role_subsumer_list(ax->lhs, ax->rhs);

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
	Queue scheduled_axioms;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	MapIterator map_iterator;

	/*
	// the role compositions
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->object_property_chains));
	ObjectPropertyExpression* object_property_chain = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain) {
		enqueue(&scheduled_axioms, create_role_saturation_axiom(object_property_chain, object_property_chain));
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}
	*/

    // the atomic roles
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->object_properties));
	ObjectPropertyExpression* object_property = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property) {
		enqueue(&scheduled_axioms, create_role_saturation_axiom(object_property, object_property));
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}

    // reflexive transitive closure of role inclusion axioms and complex role inclusion axioms
	SetIterator told_subsumers_iterator;
	SetIterator subsumees_iterator_1, subsumees_iterator_2, component_of_iterator;
	ObjectPropertyExpression* subsumee_1;
	ObjectPropertyExpression* subsumee_2;
	ax = dequeue(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax)) {
			print_role_saturation_axiom(kb, ax);

			// told subsumers
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(ax->rhs->told_subsumers));
			ObjectPropertyExpression* told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer) {
				enqueue(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, told_subsumer));
				told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			}

			if (ax->lhs->type == OBJECT_PROPERTY_CHAIN_TYPE) {
				SET_ITERATOR_INIT(&subsumees_iterator_1, &(ax->lhs->description.object_property_chain.role1->subsumees));
				subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
				while (subsumee_1) {
					SET_ITERATOR_INIT(&subsumees_iterator_2, &(ax->lhs->description.object_property_chain.role2->subsumees));
					subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
					while (subsumee_2) {
						if (!is_subformula_of(ax->lhs->description.object_property_chain.role1, subsumee_1) ||
								!is_subformula_of(ax->lhs->description.object_property_chain.role2, subsumee_2)) {
							ObjectPropertyExpression* new_composition = get_create_role_composition_binary(
									(ObjectPropertyExpression*) subsumee_1,
									(ObjectPropertyExpression*) subsumee_2,
									kb->tbox);
							printf("new_composition: %s\n", object_property_expression_to_string(kb, new_composition));
							// actually we do not need to index the composition if it already existed
							index_role(new_composition);
							enqueue(&scheduled_axioms, create_role_saturation_axiom(new_composition, ax->rhs));
						}
						subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
					}
					subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
				}
			}

			SET_ITERATOR_INIT(&component_of_iterator, &(ax->rhs->first_component_of));
			ObjectPropertyExpression* component_of = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&component_of_iterator);
			while (component_of) {
				printf("first component_of: %s\n", object_property_expression_to_string(kb, component_of));
				enqueue(&scheduled_axioms, create_role_saturation_axiom(component_of, component_of));
				component_of = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&component_of_iterator);
			}

			SET_ITERATOR_INIT(&component_of_iterator, &(ax->rhs->second_component_of));
			component_of = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&component_of_iterator);
			while (component_of) {
				printf("second component_of: %s\n", object_property_expression_to_string(kb, component_of));
				enqueue(&scheduled_axioms, create_role_saturation_axiom(component_of, component_of));
				component_of = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&component_of_iterator);
			}

		}
		free(ax);
		ax = dequeue(&scheduled_axioms);
	}
}


	/*
	// Make a copy of the object property chains since we are going to traverse and
	// modify it at the same time.
	Set tmp_object_property_chains;
	SET_INIT(&tmp_object_property_chains, 32);
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->object_property_chains));
	object_property_chain = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain) {
		SET_ADD(object_property_chain, &tmp_object_property_chains);
		object_property_chain = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	}

	SetIterator subsumees_iterator_1, subsumees_iterator_2, tmp_object_property_chains_iterator;
	ObjectPropertyExpression* subsumee_1;
	ObjectPropertyExpression* subsumee_2;
	SET_ITERATOR_INIT(&tmp_object_property_chains_iterator, &tmp_object_property_chains);
	object_property_chain = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&tmp_object_property_chains_iterator);
	while (object_property_chain) {
		SET_ITERATOR_INIT(&subsumees_iterator_1, &(object_property_chain->description.object_property_chain.role1->subsumees));
		subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
		while (subsumee_1) {
			SET_ITERATOR_INIT(&subsumees_iterator_2, &(object_property_chain->description.object_property_chain.role2->subsumees));
			subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
			while (subsumee_2) {
				ObjectPropertyExpression* new_composition = get_create_role_composition_binary(
						(ObjectPropertyExpression*) subsumee_1,
						(ObjectPropertyExpression*) subsumee_2,
						kb->tbox);
				// actually we do not need to index the composition if it already existed
				index_role(new_composition);


				// SetIterator subsumers_iterator;
				SET_ITERATOR_INIT(&told_subsumers_iterator, &(object_property_chain->told_subsumers));
				ObjectPropertyExpression* told_subsumer = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&told_subsumers_iterator);
				// subsumer of object_property_chain is a subsumer of the new_composition
				while (told_subsumer) {
					// CAUTION!: from this point on, subsumees and subsumers are not synchronized.
					printf("%s <= %s\n", object_property_expression_to_string(kb, new_composition), object_property_expression_to_string(kb, told_subsumer));
					add_to_role_subsumer_list(new_composition, told_subsumer);
					told_subsumer = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&told_subsumers_iterator);
				}


				// CAUTION!: from this point on, subsumees and subsumers are not synchronized. Subsumees set
				// is incomplete, but it is not used in concept saturation. There the subsumers_list is used.
				// add_to_role_subsumer_list(new_composition, object_property_chain);


				subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
			}
			subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
		}
		object_property_chain = SET_ITERATOR_NEXT(&tmp_object_property_chains_iterator);
	}
	*/

	// TODO: free/reset tmp_object_property_chanins ?

	// FIXME: commented out due to a bug.
	// removes some of the required subsumers in addition to the redundant ones!

	/*
	// optimization via removing the redundant subsumers
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
				}
				subsumer_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_2);
			}
			subsumer_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_1);
		}
		object_property_expression = (ObjectPropertyExpression*) pop(&scheduled_object_property_expressions);
	}
	*/
