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

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../model/datatypes.h"
#include "../model/model.h"
#include "../model/limits.h"
#include "../model/print_utils.h"
#include "../hashing/key_value_hash_table.h"
#include "../utils/stack.h"
#include "utils.h"

RoleSaturationAxiom* create_role_saturation_axiom(Role* lhs, Role* rhs) {
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


void saturate_roles(TBox* tbox) {
	RoleSaturationAxiom* ax;
	Stack scheduled_axioms;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
    // first the atomic roles
	Node* node = last_node(tbox->atomic_roles);
	while (node) {
		push(&scheduled_axioms, create_role_saturation_axiom((Role*) node->value, (Role*) node->value));
		node = previous_node(node);
	}
    // for (i = 0; i < tbox->atomic_role_count + tbox->unique_binary_role_composition_count; ++i)
    // 	push(&scheduled_axioms, create_role_saturation_axiom(tbox->role_list[i], tbox->role_list[i]));
    // Now the role compositions.
	Node* composition = last_node(tbox->role_compositions);
	while (composition) {
		push(&scheduled_axioms, create_role_saturation_axiom((Role*) composition->value, (Role*) composition->value));
		/*
		Node* told_subsumee1 = last_node(((Role*) composition->value)->description.role_composition->role1->told_subsumees);
		while (told_subsumee1) {
			Node* told_subsumee2 = last_node(((Role*) composition->value)->description.role_composition->role2->told_subsumees);
			while (told_subsumee2) {
				Role* new_composition = get_create_role_composition_binary(
						(Role*) told_subsumee1->value,
						(Role*) told_subsumee2->value,
						tbox);
				index_role(new_composition);
				push(&scheduled_axioms, create_role_saturation_axiom(new_composition, (Role*) composition->value));
				told_subsumee2 = previous_node(told_subsumee2);
			}
			told_subsumee1 = previous_node(told_subsumee1);
		}
		*/
		composition = previous_node(composition);
	}

    // reflexive transitive closure of role inclusion axioms and complex role inclusion axioms
	int i;
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax)) {
			// told subsumers
			// for (i = 0; i < ax->rhs->told_subsumer_count; ++i)
			// 	push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i]));
			Node* told_subsumer = last_node(ax->rhs->told_subsumers);
			while (told_subsumer) {
			 	push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, (Role*) told_subsumer->value));

			 	if (ax->lhs->type == ROLE_COMPOSITION) {
			 		Node* told_subsumee1 = last_node(ax->lhs->description.role_composition->role1->told_subsumees);
			 		while (told_subsumee1) {
			 			Node* told_subsumee2 = last_node(ax->lhs->description.role_composition->role2->told_subsumees);
			 			while (told_subsumee2) {
			 				Role* composition = get_create_role_composition_binary(
			 						(Role*) told_subsumee1->value,
			 						(Role*) told_subsumee2->value,
			 						tbox);
			 				index_role(composition);
			 				push(&scheduled_axioms, create_role_saturation_axiom(composition, (Role*) told_subsumer->value));
			 				told_subsumee2 = previous_node(told_subsumee2);
			 			}
			 			told_subsumee1 = previous_node(told_subsumee1);
			 		}
			 	}

			 	told_subsumer = previous_node(told_subsumer);
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

