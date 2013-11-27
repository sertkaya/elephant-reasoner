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
#include <assert.h>

#include "../model/model.h"
#include "../model/limits.h"
#include "../model/print_utils.h"
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
	add_to_role_subsumees(ax->rhs, ax->lhs);

	return added_to_subsumer_list;
}


void saturate_roles(TBox* tbox) {
	RoleSaturationAxiom* ax;
	Stack scheduled_axioms;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
    PWord_t key = NULL;
    uint8_t role_index[MAX_ROLE_NAME_LENGTH];

    // first the atomic roles
	// start with the smallest role name
	role_index[0] = '\0';
    JSLF(key, tbox->atomic_roles, role_index);
    while (key != NULL) {
            push(&scheduled_axioms, create_role_saturation_axiom((Role*) *key, (Role*) *key));
            JSLN(key, tbox->atomic_roles, role_index);
    }
    // now the role compositions
	role_index[0] = '\0';
    JSLF(key, tbox->role_compositions, role_index);
    while (key != NULL) {
    		push(&scheduled_axioms, create_role_saturation_axiom((Role*) *key, (Role*) *key));
            JSLN(key, tbox->role_compositions, role_index);
    }

    // reflexive transitive closure of (complex) role inclusion axioms
    int i;
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax)) {
			// told subsumers
			for (i = 0; i < ax->rhs->told_subsumer_count; ++i)
				push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i]));
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}

	// TODO:
    // now new role composition hierarchies using the computed role hierarchies.
	// this is for optimizing the role composition rule in concept saturation
	/*
	role_index[0] = '\0';
    JSLF(key, tbox->role_compositions, role_index);
    while (key != NULL) {
    		// for role compositions we do not need axioms of type role_comp(r,s) -> role_comp(r,s)
    		// in the initialization. the reason is, they can only appear on the lhs of a role inclusion axiom
    		// therefore, we push only axioms with told subsumer on the rhs
    		for (i = 0; i < ((Role*) *key)->told_subsumer_count; ++i)
    			push(&scheduled_axioms, create_role_saturation_axiom((Role*) *key, ((Role*) *key)->told_subsumers[i]));
    		// push(&scheduled_axioms, create_role_saturation_axiom((Role*) *key, (Role*) *key));
            JSLN(key, tbox->role_compositions, role_index);
    }
    */

}

