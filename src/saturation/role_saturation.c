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

static inline void print_role_saturation_axiom(KB* kb, RoleSaturationAxiom* ax) {
	char* lhs_str = object_property_expression_to_string(kb, ax->lhs);
	char* rhs_str = object_property_expression_to_string(kb, ax->rhs);
	printf("%s <= %s\n", lhs_str, rhs_str);
	free(lhs_str);
	free(rhs_str);
}

void saturate_roles(KB* kb) {
	RoleSaturationAxiom* ax;

	// stack for the reflexive transitive closure of object properties
	Stack scheduled_axioms;
	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	MapIterator map_iterator;
	ObjectPropertyExpression* object_property_chain;


	// the object property chains
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperty_chains));
	object_property_chain = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property_chain, object_property_chain));
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}


   // the object properties
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperties));
	ObjectPropertyExpression* object_property = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property, object_property));
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}

	// reflexive transitive closure
	SetIterator told_subsumers_iterator;
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

	// stack for the object property chain hierarchy computation
	// Stack scheduled_object_property_chains;
	Queue scheduled_object_property_chains;
	// initialize the stack
	// init_stack(&scheduled_object_property_chains);
	init_queue(&scheduled_object_property_chains);

	// push the object property chains to the scheduled stack
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperty_chains));
	object_property_chain = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain) {
		// push(&scheduled_object_property_chains, object_property_chain);
		enqueue(&scheduled_object_property_chains, object_property_chain);
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}

	// object property chain hierarchy computation.
	ObjectPropertyExpression *subsumee_1, *subsumee_2;
	SetIterator subsumees_iterator_1, subsumees_iterator_2, component_of_iterator;
	// object_property_chain = pop(&scheduled_object_property_chains);
	object_property_chain = dequeue(&scheduled_object_property_chains);
	while (object_property_chain != NULL) {
		SET_ITERATOR_INIT(&subsumees_iterator_1, &(object_property_chain->description.object_property_chain.role1->subsumees));
		subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
		while (subsumee_1) {
			SET_ITERATOR_INIT(&subsumees_iterator_2, &(object_property_chain->description.object_property_chain.role2->subsumees));
			subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
			while (subsumee_2) {
					// new object property chain
					ObjectPropertyExpression* new_composition = get_create_role_composition_binary(
							(ObjectPropertyExpression*) subsumee_1,
							(ObjectPropertyExpression*) subsumee_2,
							kb->tbox);

					add_to_role_subsumer_list(new_composition, object_property_chain);
					index_role(new_composition);
				subsumee_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_2);
			}
			subsumee_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumees_iterator_1);
		}
		// object_property_chain = pop(&scheduled_object_property_chains);
		object_property_chain = dequeue(&scheduled_object_property_chains);
	}

	// remove the redundant subsumers of object property chains
	Set subsumers_to_remove;
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperty_chains));
	object_property_chain = (ObjectPropertyExpression*) MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain) {
		// printf("%s:%d\n", object_property_expression_to_string(kb, object_property_chain), object_property_chain->subsumers.element_count);
		SET_INIT(&subsumers_to_remove, 16);
		SetIterator subsumers_iterator_1;
		SET_ITERATOR_INIT(&subsumers_iterator_1, &(object_property_chain->subsumers));
		ObjectPropertyExpression* subsumer_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_1);
		while (subsumer_1 && subsumer_1 != object_property_chain) {
			if (subsumer_1 == object_property_chain) {
				subsumer_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_1);
				continue;
			}
			// printf("%s:remove:", object_property_expression_to_string(kb, subsumer_1));
			SetIterator subsumers_iterator_2;
			SET_ITERATOR_INIT(&subsumers_iterator_2, &(object_property_chain->subsumers));
			ObjectPropertyExpression* subsumer_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_2);
			while (subsumer_2) {
				if (subsumer_2 == object_property_chain || subsumer_1 == subsumer_2) {
					subsumer_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_2);
					continue;
				}
				if (SET_CONTAINS(subsumer_2, &(subsumer_1->subsumers))) {
					// printf("%s:", object_property_expression_to_string(kb, subsumer_2));
					SET_ADD(subsumer_2, &subsumers_to_remove);
				}
				subsumer_2 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_2);
			}
			// printf("\n");
			subsumer_1 = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&subsumers_iterator_1);
		}

		// now remove the elements of the set 'remove' from the subsumers of object_property_chain
		SetIterator remove_iterator;
		SET_ITERATOR_INIT(&remove_iterator, &subsumers_to_remove);
		ObjectPropertyExpression* subsumer_to_remove = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&remove_iterator);
		while (subsumer_to_remove) {
			// list_remove(subsumer_to_remove, &(object_property_chain->subsumer_list));
			// printf("removed %s from the subsumers of %s\n", object_property_expression_to_string(kb, subsumer_to_remove), object_property_expression_to_string(kb, object_property_chain));
			subsumer_to_remove = (ObjectPropertyExpression*) SET_ITERATOR_NEXT(&remove_iterator);
		}
		SET_RESET(&subsumers_to_remove);
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}
}
