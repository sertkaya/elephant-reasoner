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
    // first the atomic roles
	MapIterator iterator;
	MAP_ITERATOR_INIT(&iterator, &(tbox->object_properties));
	void* object_property = MAP_ITERATOR_NEXT(&iterator);
	while (object_property) {
		push(&scheduled_axioms, create_role_saturation_axiom((ObjectPropertyExpression*) object_property, (ObjectPropertyExpression*) object_property));
		object_property = MAP_ITERATOR_NEXT(&iterator);
	}

    // Now the role compositions.
	MAP_ITERATOR_INIT(&iterator, &(tbox->object_property_chains));
	void* composition = MAP_ITERATOR_NEXT(&iterator);
	while (composition) {
		push(&scheduled_axioms, create_role_saturation_axiom((ObjectPropertyExpression*) composition, (ObjectPropertyExpression*) composition));
		composition = MAP_ITERATOR_NEXT(&iterator);
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
			while (first_component_of) {
				ObjectPropertyExpression* composition = get_create_role_composition_binary(ax->lhs,
						((ObjectPropertyExpression*) first_component_of)->description.object_property_chain.role2,
						tbox);
				// actually we do not need to index the composition if it already existed
				index_role(composition);
				push(&scheduled_axioms, create_role_saturation_axiom(composition, (ObjectPropertyExpression*) first_component_of));
				first_component_of = SET_ITERATOR_NEXT(&first_component_of_iterator);
			}

			SetIterator second_component_of_iterator;
			SET_ITERATOR_INIT(&second_component_of_iterator, &(ax->rhs->second_component_of));
			void* second_component_of = SET_ITERATOR_NEXT(&second_component_of_iterator);
			while (second_component_of) {
				ObjectPropertyExpression* composition = get_create_role_composition_binary(
						((ObjectPropertyExpression*) second_component_of)->description.object_property_chain.role1,
						ax->lhs,
						tbox);
				// actually we do not need to index the composition if it already existed
				index_role(composition);
				push(&scheduled_axioms, create_role_saturation_axiom(composition, (ObjectPropertyExpression*) second_component_of));
				second_component_of = SET_ITERATOR_NEXT(&second_component_of_iterator);
			}

		 	if (ax->lhs->type == OBJECT_PROPERTY_CHAIN_TYPE) {
		 		// SetIterator subsumers_iterator;
		 		// SET_ITERATOR_INIT(&subsumers_iterator, &(ax->rhs->subsumers));
		 		// void* subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
		 		// while (subsumer) {
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
			 				// push(&scheduled_axioms, create_role_saturation_axiom(composition, (ObjectPropertyExpression*) subsumer));
			 				push(&scheduled_axioms, create_role_saturation_axiom(composition, ax->rhs));
			 				subsumee_2 = SET_ITERATOR_NEXT(&subsumees_iterator_2);
			 			}
			 			subsumee_1 = SET_ITERATOR_NEXT(&subsumees_iterator_1);
			 		}
			 		// subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
			 	// }
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}

	// Compute the role composition hierarchies.
	// This is for optimizing the role composition rule in concept saturation
	// (We first make a temporary copy of the  role compositions and work with the copy
	// since new binary role compositions are generated during this process)
	/*
	int original_binary_composition_count = tbox->unique_binary_role_composition_count;
	Role** tmp = (Role**) malloc(original_binary_composition_count * sizeof(Role*));
	assert(tmp != NULL);
	int j = 0;
	// iterate over the role_compositions hash, copy to the tmp
	node = last_node(tbox->role_compositions);
	while (node) {
		tmp[j++] = (Role*) node->value;
		node = previous_node(node);
	}

	int role1_subsumee_count, role2_subsumee_count;
	for (i = 0; i < original_binary_composition_count; ++i)
		for (j = 0; j < tmp[i]->description.role_composition->role1->subsumee_count; ++j) {
			int k;
			for (k = 0; k < tmp[i]->description.role_composition->role2->subsumee_count; ++k) {
				Role* composition = get_create_role_composition_binary(
						tmp[i]->description.role_composition->role1->subsumee_list[j],
						tmp[i]->description.role_composition->role2->subsumee_list[k],
						tbox);
				index_role(composition);
				// now add the subsumers of tmp[i] as the subsumers of composition
				int l;
				for (l = 0; l < tmp[i]->subsumer_count; ++l)
					add_to_role_subsumer_list(composition, tmp[i]->subsumer_list[l]);
			}
		}

	// now free tmp
	free(tmp);
	*/
}

