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

RoleSaturationAxiom* create_role_saturation_axiom(ObjectPropertyExpressionId lhs, ObjectPropertyExpressionId rhs) {
	RoleSaturationAxiom* ax = (RoleSaturationAxiom*) malloc(sizeof(RoleSaturationAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;

	return ax;
}

int mark_role_saturation_axiom_processed(RoleSaturationAxiom* ax, TBox* tbox) {
	int added_to_subsumer_list;

	// add to the subsumer list to mark it as processed
	added_to_subsumer_list = add_to_role_subsumer_list(ax->lhs, ax->rhs, tbox);

	// add to the subsumees hash too
	add_to_role_subsumee_list(ax->rhs, ax->lhs, tbox);

	return added_to_subsumer_list;
}

static inline void print_role_saturation_axiom(KB* kb, RoleSaturationAxiom* ax) {
	char* lhs_str = object_property_expression_to_string(kb, ax->lhs);
	char* rhs_str = object_property_expression_to_string(kb, ax->rhs);
	printf("%s -> %s\n", lhs_str, rhs_str);
	free(lhs_str);
	free(rhs_str);
}
void saturate_roles(KB* kb) {
	RoleSaturationAxiom* ax;
	TBox* tbox = kb->tbox;

	// stack for the reflexive transitive closure of object properties and object property chains
	Stack scheduled_axioms;
	// initialize the stack
	init_stack(&scheduled_axioms);

	MapIterator map_iterator;

   // push the object properties to the stack
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->objectproperties));
	ObjectPropertyExpressionId object_property = MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property != KEY_NOT_FOUND_IN_HASH_MAP) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property, object_property));
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}
	// push the object property chains to the stack
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->objectproperty_chains));
	ObjectPropertyExpressionId object_property_chain;
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property_chain, object_property_chain));
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}

	// compute the reflexive transitive closure
	SetIterator told_subsumers_iterator;
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax, kb->tbox)) {
			// told subsumers
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(OPEXP(ax->rhs).told_subsumers));
			ObjectPropertyExpressionId told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer != HASH_TABLE_KEY_NOT_FOUND) {
				push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, told_subsumer));
				told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}

	// compute the object property chain hierarchy

	// Stack scheduled_object_property_chains;
	Queue scheduled_object_property_chains;
	// initialize the stack
	// init_stack(&scheduled_object_property_chains);
	init_queue(&scheduled_object_property_chains);

	// push the object property chains to the scheduled stack
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->objectproperty_chains));
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		// push(&scheduled_object_property_chains, object_property_chain);
		enqueue(&scheduled_object_property_chains, object_property_chain);
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}

	// object property chain hierarchy computation.
	ObjectPropertyExpressionId subsumee_1, subsumee_2;
	SetIterator subsumees_iterator_1, subsumees_iterator_2;
	// object_property_chain = pop(&scheduled_object_property_chains);
	object_property_chain = dequeue(&scheduled_object_property_chains);
	while (object_property_chain != QUEUE_ELEMENT_NOT_FOUND) {
		printf("object_property_chain:%s\n", object_property_expression_to_string(kb, object_property_chain));
		SET_ITERATOR_INIT(&subsumees_iterator_1, &(OPEXP(OPEXP(object_property_chain).description.object_property_chain.role1).subsumees));
		subsumee_1 =  SET_ITERATOR_NEXT(&subsumees_iterator_1);
		while (subsumee_1 != HASH_TABLE_KEY_NOT_FOUND) {
			SET_ITERATOR_INIT(&subsumees_iterator_2, &(OPEXP(OPEXP(object_property_chain).description.object_property_chain.role2).subsumees));
			subsumee_2 =  SET_ITERATOR_NEXT(&subsumees_iterator_2);
			while (subsumee_2 != HASH_TABLE_KEY_NOT_FOUND) {
				// new object property chain
				ObjectPropertyExpressionId new_composition = get_create_role_composition_binary(subsumee_1, subsumee_2, kb->tbox);
				printf("subsume_1:%s\n", object_property_expression_to_string(kb, subsumee_1));
				printf("subsume_2:%s\n", object_property_expression_to_string(kb, subsumee_2));
				printf("new composition: %s\n", object_property_expression_to_string(kb, new_composition));
				// TODO: Also enqueue new_composition?
				// No, new_composition does not need to be enqueued into the scheduled_object_property_chains.
				// The reason is:
				// Subsumees of role1 of the new_composition are also subsumees of the role1 of  object_property_chain
				// (which is subsumee_1 in this loop). Therefore, they will be processed here anyway.

				// add_to_role_subsumer_list(new_composition, object_property_chain, tbox);
				// instead add the subsumers of the object_property_chain to the subsumers of new_composition?
				/*
				SetIterator subsumers_iterator;
				SET_ITERATOR_INIT(&subsumers_iterator, &(OPEXP(object_property_chain).subsumers));
				ObjectPropertyExpressionId subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				while (subsumer != HASH_TABLE_KEY_NOT_FOUND) {
					add_to_role_subsumer_list(new_composition, subsumer, tbox);
					subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				}
				*/
				// Add object_property_chain to the told subsumers of new_composition. The transitive closure of all
				// object properties + object property chains will be  computed below once more.
				ADD_TOLD_SUBSUMER_OBJECT_PROPERTY_EXPRESSION(object_property_chain, new_composition, kb->tbox);

				// Now index the new_composition for the object property chain in the concept saturation.
				index_role(new_composition, kb->tbox);

				// TODO: Updating the address of the hash_table in the iterator is just an ad-hoc solution. Not elegant!
				// The reason is: tbox->object_property_expressions gets realloced in create_object_property_expression_template in model.c. During realloc, the address of
				// (OPEXP(OPEXP(object_property_chain).description.object_property_chain.role1).subsumees changes but the reference  in the iterator remains.
				// addendum: then iterating over the subsumees is affected by this change
				SET_ITERATOR_UPDATE_HASH_TABLE(&subsumees_iterator_2, &(OPEXP(OPEXP(object_property_chain).description.object_property_chain.role2).subsumees));
				subsumee_2 =  SET_ITERATOR_NEXT(&subsumees_iterator_2);
			}
			// TODO: Updating the address of the hash_table in the iterator is just an ad-hoc solution. Not elegant!
			// The reason is: tbox->object_property_expressions gets realloced in create_object_property_expression_template in model.c. During realloc, the address of
			// (OPEXP(OPEXP(object_property_chain).description.object_property_chain.role1).subsumees changes but the reference  in the iterator remains.
			SET_ITERATOR_UPDATE_HASH_TABLE(&subsumees_iterator_1, &(OPEXP(OPEXP(object_property_chain).description.object_property_chain.role1).subsumees));
			subsumee_1 =  SET_ITERATOR_NEXT(&subsumees_iterator_1);
		}
		// object_property_chain = pop(&scheduled_object_property_chains);
		object_property_chain = dequeue(&scheduled_object_property_chains);
	}

   // push the object properties to the stack
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->objectproperties));
	object_property = MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property != KEY_NOT_FOUND_IN_HASH_MAP) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property, object_property));
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}
	// push the object property chains to the stack
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->objectproperty_chains));
	// ObjectPropertyExpressionId object_property_chain;
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property_chain, object_property_chain));
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}

	// compute the reflexive transitive closure
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax, kb->tbox)) {
			printf("***\n");
			print_role_saturation_axiom(kb, ax);
			printf("***\n");
			// told subsumers
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(OPEXP(ax->rhs).told_subsumers));
			ObjectPropertyExpressionId told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer != HASH_TABLE_KEY_NOT_FOUND) {
				push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, told_subsumer));
				told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}

/*
	// remove the redundant subsumers of object property chains
	// We keep the subsumers to be removed in a map. Key is ObjectPropertyExpressionId,
	// value is Set, which keeps the subsumers to be removed.
	Map_64 subsumers_to_remove_map;
	MAP_INIT_64(&subsumers_to_remove_map, 16);
	Set* subsumers_to_remove_set;
	// printf("*** preparing the subsumers to remove ***\n");
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperty_chains));
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		// printf("%s\n", object_property_expression_to_string(kb, object_property_chain));
		SetIterator subsumers_iterator_1;
		SET_ITERATOR_INIT(&subsumers_iterator_1, &(OPEXP(object_property_chain).subsumers));
		ObjectPropertyExpressionId subsumer_1 = SET_ITERATOR_NEXT(&subsumers_iterator_1);
		while (subsumer_1 != HASH_TABLE_KEY_NOT_FOUND) {
			if (subsumer_1 == object_property_chain) {
				subsumer_1 = SET_ITERATOR_NEXT(&subsumers_iterator_1);
				continue;
			}

			subsumers_to_remove_set = SET_CREATE(8);
			MAP_PUT_64(object_property_chain, subsumers_to_remove_set, &subsumers_to_remove_map);

			// printf("%s:remove:", object_property_expression_to_string(kb, subsumer_1));
			SetIterator subsumers_iterator_2;
			SET_ITERATOR_INIT(&subsumers_iterator_2, &(OPEXP(object_property_chain).subsumers));
			ObjectPropertyExpressionId subsumer_2 =  SET_ITERATOR_NEXT(&subsumers_iterator_2);
			while (subsumer_2 != HASH_TABLE_KEY_NOT_FOUND) {
				if (subsumer_2 == object_property_chain || subsumer_1 == subsumer_2) {
				// if (subsumer_1 == subsumer_2) {
					subsumer_2 = SET_ITERATOR_NEXT(&subsumers_iterator_2);
					continue;
				}
				if (SET_CONTAINS(subsumer_2, &(OPEXP(subsumer_1).subsumers))) {
					// printf("%s:", object_property_expression_to_string(kb, subsumer_2));
					//
					// ObjectPropertyExpressionId* tmp = realloc(subsumers_to_remove[subsumer_1], (subsumers_count[subsumer_1] + 1) * sizeof(ObjectPropertyExpressionId));
					// assert(tmp != NULL);
					// subsumers_to_remove[subsumer_1] = tmp;
					// subsumers_to_remove[subsumer_1][subsumers_count[subsumer_1]] = subsumer_2;
					// ++subsumers_count[subsumer_1];
					//
					SET_ADD(subsumer_2, subsumers_to_remove_set);

					//SET_ADD(subsumer_2, &subsumers_to_remove);
				}
				subsumer_2 =  SET_ITERATOR_NEXT(&subsumers_iterator_2);
			}
			// printf("\n");
			subsumer_1 =  SET_ITERATOR_NEXT(&subsumers_iterator_1);
		}

		// now remove the elements of the set 'remove' from the subsumers of object_property_chain
		//
		// SetIterator remove_iterator;
		// SET_ITERATOR_INIT(&remove_iterator, &subsumers_to_remove);
		// ObjectPropertyExpressionId subsumer_to_remove = (ObjectPropertyExpressionId) SET_ITERATOR_NEXT(&remove_iterator);
		// while (subsumer_to_remove != HASH_TABLE_KEY_NOT_FOUND) {
		// 	// list_remove(subsumer_to_remove, &(object_property_chain->subsumer_list));
		// 	// printf("removed %s from the subsumers of %s\n", object_property_expression_to_string(kb, subsumer_to_remove), object_property_expression_to_string(kb, object_property_chain));
		// 	subsumer_to_remove = (ObjectPropertyExpressionId) SET_ITERATOR_NEXT(&remove_iterator);
		// }
		// SET_RESET(&subsumers_to_remove);
		//

		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}
*/
	/*
	// printf("*** preparing the subsumers to remove ***\n");
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperties));
	object_property =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property != KEY_NOT_FOUND_IN_HASH_MAP) {
		// printf("%s\n", object_property_expression_to_string(kb, object_property));
		SetIterator subsumers_iterator_1;
		SET_ITERATOR_INIT(&subsumers_iterator_1, &(OPEXP(object_property).subsumers));
		ObjectPropertyExpressionId subsumer_1 = SET_ITERATOR_NEXT(&subsumers_iterator_1);
		while (subsumer_1 != HASH_TABLE_KEY_NOT_FOUND) {
			// if (subsumer_1 == object_property) {
			// 	subsumer_1 = SET_ITERATOR_NEXT(&subsumers_iterator_1);
			// 	continue;
			// }

			subsumers_to_remove_set = SET_CREATE(8);
			MAP_PUT_64(object_property, subsumers_to_remove_set, &subsumers_to_remove_map);

			// printf("%s:remove:", object_property_expression_to_string(kb, subsumer_1));
			SetIterator subsumers_iterator_2;
			SET_ITERATOR_INIT(&subsumers_iterator_2, &(OPEXP(object_property).subsumers));
			ObjectPropertyExpressionId subsumer_2 =  SET_ITERATOR_NEXT(&subsumers_iterator_2);
			while (subsumer_2 != HASH_TABLE_KEY_NOT_FOUND) {
				// if (subsumer_2 == object_property|| subsumer_1 == subsumer_2) {
				// 	subsumer_2 = SET_ITERATOR_NEXT(&subsumers_iterator_2);
				// 	continue;
				// }
				if (SET_CONTAINS(subsumer_2, &(OPEXP(subsumer_1).subsumers))) {
					// printf("%s:", object_property_expression_to_string(kb, subsumer_2));
					//
					// ObjectPropertyExpressionId* tmp = realloc(subsumers_to_remove[subsumer_1], (subsumers_count[subsumer_1] + 1) * sizeof(ObjectPropertyExpressionId));
					// assert(tmp != NULL);
					// subsumers_to_remove[subsumer_1] = tmp;
					// subsumers_to_remove[subsumer_1][subsumers_count[subsumer_1]] = subsumer_2;
					// ++subsumers_count[subsumer_1];
					//
					SET_ADD(subsumer_2, subsumers_to_remove_set);

					//SET_ADD(subsumer_2, &subsumers_to_remove);
				}
				subsumer_2 =  SET_ITERATOR_NEXT(&subsumers_iterator_2);
			}
			// printf("\n");
			subsumer_1 =  SET_ITERATOR_NEXT(&subsumers_iterator_1);
		}

		// now remove the elements of the set 'remove' from the subsumers of object_property_chain
		//
		// SetIterator remove_iterator;
		// SET_ITERATOR_INIT(&remove_iterator, &subsumers_to_remove);
		// ObjectPropertyExpressionId subsumer_to_remove = (ObjectPropertyExpressionId) SET_ITERATOR_NEXT(&remove_iterator);
		// while (subsumer_to_remove != HASH_TABLE_KEY_NOT_FOUND) {
		// 	// list_remove(subsumer_to_remove, &(object_property->subsumer_list));
		// 	// printf("removed %s from the subsumers of %s\n", object_property_expression_to_string(kb, subsumer_to_remove), object_property_expression_to_string(kb, object_property));
		// 	subsumer_to_remove = (ObjectPropertyExpressionId) SET_ITERATOR_NEXT(&remove_iterator);
		// }
		// SET_RESET(&subsumers_to_remove);
		//

		object_property= MAP_ITERATOR_NEXT(&map_iterator);
	}
	*/
/*
	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperty_chains));
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		printf("==================\n%s\n", object_property_expression_to_string(kb, object_property_chain));
		subsumers_to_remove_set = MAP_GET_64(object_property_chain, &subsumers_to_remove_map);

		//
		// if (subsumers_to_remove_set == NULL) {
		// 	printf("subsumers EMPTY\n");
		// 	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
		// 	continue;
		// }
		//

		SetIterator subsumers_iterator;
		SET_ITERATOR_INIT(&subsumers_iterator, subsumers_to_remove_set);
		ObjectPropertyExpressionId subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
		while (subsumer != HASH_TABLE_KEY_NOT_FOUND) {
			if (SET_REMOVE(subsumer, &(OPEXP(object_property_chain).subsumers))) {
				printf("%s removed\n", object_property_expression_to_string(kb, subsumer));
				printf("%d\n", SET_CONTAINS(subsumer, &(OPEXP(object_property_chain).subsumers)));
			}
			else{
				printf("%s NOT FOUND in subsumers!\n", object_property_expression_to_string(kb, subsumer));
			}
			subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
		}
		printf("\n\n\n");

		// now free the set of removed subsumers
		SET_FREE(subsumers_to_remove_set);

		object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	}

	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperties));
	object_property =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property != KEY_NOT_FOUND_IN_HASH_MAP) {
		printf("==================\n%s\n", object_property_expression_to_string(kb, object_property));
		subsumers_to_remove_set = MAP_GET_64(object_property, &subsumers_to_remove_map);

		//
		// if (subsumers_to_remove_set == NULL) {
		// 	printf("subsumers EMPTY\n");
		// 	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
		// 	continue;
		// }
		//

		SetIterator subsumers_iterator;
		SET_ITERATOR_INIT(&subsumers_iterator, subsumers_to_remove_set);
		ObjectPropertyExpressionId subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
		while (subsumer != HASH_TABLE_KEY_NOT_FOUND) {
			if (SET_REMOVE(subsumer, &(OPEXP(object_property).subsumers))) {
				printf("%s removed\n", object_property_expression_to_string(kb, subsumer));
				printf("%d\n", SET_CONTAINS(subsumer, &(OPEXP(object_property).subsumers)));
			}
			else{
				printf("%s NOT FOUND in subsumers!\n", object_property_expression_to_string(kb, subsumer));
			}
			subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
		}
		printf("\n\n\n");

		// now free the set of removed subsumers
		SET_FREE(subsumers_to_remove_set);

		object_property=  MAP_ITERATOR_NEXT(&map_iterator);
	}
*/

	// now free the map of subsumers to remove
	// MAP_RESET_64(&(subsumers_to_remove_map));
}
/*
void saturate_roles(KB* kb) {
	RoleSaturationAxiom* ax;
	TBox* tbox = kb->tbox;

	// stack for the reflexive transitive closure of object properties
	Stack scheduled_axioms;
	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	MapIterator map_iterator;

   // the object properties
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->objectproperties));
	ObjectPropertyExpressionId object_property = MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property != KEY_NOT_FOUND_IN_HASH_MAP) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property, object_property));
		object_property = MAP_ITERATOR_NEXT(&map_iterator);
	}

	// reflexive transitive closure of object properties
	SetIterator told_subsumers_iterator;
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax, kb->tbox)) {
			// told subsumers
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(OPEXP(ax->rhs).told_subsumers));
			ObjectPropertyExpressionId told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer != HASH_TABLE_KEY_NOT_FOUND) {
				push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, told_subsumer));
				told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}

	// the object property chains
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->objectproperty_chains));
	ObjectPropertyExpressionId object_property_chain;
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		push(&scheduled_axioms, create_role_saturation_axiom(object_property_chain, object_property_chain));
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}
	// reflexive transitive closure of object property chains
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax, kb->tbox)) {
			// told subsumers
			SET_ITERATOR_INIT(&told_subsumers_iterator, &(OPEXP(ax->rhs).told_subsumers));
			ObjectPropertyExpressionId told_subsumer = SET_ITERATOR_NEXT(&told_subsumers_iterator);
			while (told_subsumer != HASH_TABLE_KEY_NOT_FOUND) {
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
	MAP_ITERATOR_INIT(&map_iterator, &(tbox->objectproperty_chains));
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		// push(&scheduled_object_property_chains, object_property_chain);
		enqueue(&scheduled_object_property_chains, object_property_chain);
		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}

	// object property chain hierarchy computation.
	ObjectPropertyExpressionId subsumee_1, subsumee_2;
	SetIterator subsumees_iterator_1, subsumees_iterator_2;
	// object_property_chain = pop(&scheduled_object_property_chains);
	object_property_chain = dequeue(&scheduled_object_property_chains);
	while (object_property_chain != QUEUE_ELEMENT_NOT_FOUND) {
		// printf("object_property_chain:%s\n", object_property_expression_to_string(kb, object_property_chain));
		SET_ITERATOR_INIT(&subsumees_iterator_1, &(OPEXP(OPEXP(object_property_chain).description.object_property_chain.role1).subsumees));
		subsumee_1 =  SET_ITERATOR_NEXT(&subsumees_iterator_1);
		while (subsumee_1 != HASH_TABLE_KEY_NOT_FOUND) {
			SET_ITERATOR_INIT(&subsumees_iterator_2, &(OPEXP(OPEXP(object_property_chain).description.object_property_chain.role2).subsumees));
			subsumee_2 =  SET_ITERATOR_NEXT(&subsumees_iterator_2);
			while (subsumee_2 != HASH_TABLE_KEY_NOT_FOUND) {
				// new object property chain
				ObjectPropertyExpressionId new_composition = get_create_role_composition_binary(subsumee_1, subsumee_2, kb->tbox);
				printf("subsume_1:%s\n", object_property_expression_to_string(kb, subsumee_1));
				printf("subsume_2:%s\n", object_property_expression_to_string(kb, subsumee_2));
				printf("new composition: %s\n", object_property_expression_to_string(kb, new_composition));
				// TODO: Also enqueue new_composition?
				// add_to_role_subsumer_list(new_composition, object_property_chain, tbox);
				// instead add the subsumers of the object_property_chain to the subsumers of new_composition?

				SetIterator subsumers_iterator;
				SET_ITERATOR_INIT(&subsumers_iterator, &(OPEXP(object_property_chain).subsumers));
				ObjectPropertyExpressionId subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				while (subsumer != HASH_TABLE_KEY_NOT_FOUND) {
					add_to_role_subsumer_list(new_composition, subsumer, tbox);
					subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				}

				index_role(new_composition, kb->tbox);

				// TODO: Updating the address of the hash_table in the iterator is just an ad-hoc solution. Not elegant!
				// The reason is: tbox->object_property_expressions gets realloced in create_object_property_expression_template in model.c. During realloc, the address of
				// (OPEXP(OPEXP(object_property_chain).description.object_property_chain.role1).subsumees changes but the reference  in the iterator remains.
				// addendum: then iterating over the subsumees is affected by this change
				SET_ITERATOR_UPDATE_HASH_TABLE(&subsumees_iterator_2, &(OPEXP(OPEXP(object_property_chain).description.object_property_chain.role2).subsumees));
				subsumee_2 =  SET_ITERATOR_NEXT(&subsumees_iterator_2);
			}
			// TODO: Updating the address of the hash_table in the iterator is just an ad-hoc solution. Not elegant!
			// The reason is: tbox->object_property_expressions gets realloced in create_object_property_expression_template in model.c. During realloc, the address of
			// (OPEXP(OPEXP(object_property_chain).description.object_property_chain.role1).subsumees changes but the reference  in the iterator remains.
			SET_ITERATOR_UPDATE_HASH_TABLE(&subsumees_iterator_1, &(OPEXP(OPEXP(object_property_chain).description.object_property_chain.role1).subsumees));
			subsumee_1 =  SET_ITERATOR_NEXT(&subsumees_iterator_1);
		}
		// object_property_chain = pop(&scheduled_object_property_chains);
		object_property_chain = dequeue(&scheduled_object_property_chains);
	}



	// remove the redundant subsumers of object property chains
	// We keep the subsumers to be removed in a map. Key is ObjectPropertyExpressionId,
	// value is Set, which keeps the subsumers to be removed.
	Map_64 subsumers_to_remove_map;
	MAP_INIT_64(&subsumers_to_remove_map, 16);
	Set* subsumers_to_remove_set;

	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperty_chains));
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		// printf("%s:%d\n", object_property_expression_to_string(kb, object_property_chain), object_property_chain->subsumers.element_count);
		SetIterator subsumers_iterator_1;
		SET_ITERATOR_INIT(&subsumers_iterator_1, &(OPEXP(object_property_chain).subsumers));
		ObjectPropertyExpressionId subsumer_1 = SET_ITERATOR_NEXT(&subsumers_iterator_1);
		while (subsumer_1 != HASH_TABLE_KEY_NOT_FOUND) {
			if (subsumer_1 == object_property_chain) {
				subsumer_1 = SET_ITERATOR_NEXT(&subsumers_iterator_1);
				continue;
			}

			subsumers_to_remove_set = SET_CREATE(8);
			MAP_PUT_64(subsumer_1, subsumers_to_remove_set, &subsumers_to_remove_map);

			// printf("%s:remove:", object_property_expression_to_string(kb, subsumer_1));
			SetIterator subsumers_iterator_2;
			SET_ITERATOR_INIT(&subsumers_iterator_2, &(OPEXP(object_property_chain).subsumers));
			ObjectPropertyExpressionId subsumer_2 =  SET_ITERATOR_NEXT(&subsumers_iterator_2);
			while (subsumer_2 != HASH_TABLE_KEY_NOT_FOUND) {
				if (subsumer_2 == object_property_chain || subsumer_1 == subsumer_2) {
					subsumer_2 = SET_ITERATOR_NEXT(&subsumers_iterator_2);
					continue;
				}
				if (SET_CONTAINS(subsumer_2, &(OPEXP(subsumer_1).subsumers))) {
					// printf("%s:", object_property_expression_to_string(kb, subsumer_2));

					// ObjectPropertyExpressionId* tmp = realloc(subsumers_to_remove[subsumer_1], (subsumers_count[subsumer_1] + 1) * sizeof(ObjectPropertyExpressionId));
					// assert(tmp != NULL);
					// subsumers_to_remove[subsumer_1] = tmp;
					// subsumers_to_remove[subsumer_1][subsumers_count[subsumer_1]] = subsumer_2;
					// ++subsumers_count[subsumer_1];

					SET_ADD(subsumer_2, subsumers_to_remove_set);

					//SET_ADD(subsumer_2, &subsumers_to_remove);
				}
				subsumer_2 =  SET_ITERATOR_NEXT(&subsumers_iterator_2);
			}
			// printf("\n");
			subsumer_1 =  SET_ITERATOR_NEXT(&subsumers_iterator_1);
		}

		// now remove the elements of the set 'remove' from the subsumers of object_property_chain

		// SetIterator remove_iterator;
		// SET_ITERATOR_INIT(&remove_iterator, &subsumers_to_remove);
		// ObjectPropertyExpressionId subsumer_to_remove = (ObjectPropertyExpressionId) SET_ITERATOR_NEXT(&remove_iterator);
		// while (subsumer_to_remove != HASH_TABLE_KEY_NOT_FOUND) {
		// 	// list_remove(subsumer_to_remove, &(object_property_chain->subsumer_list));
		// 	// printf("removed %s from the subsumers of %s\n", object_property_expression_to_string(kb, subsumer_to_remove), object_property_expression_to_string(kb, object_property_chain));
		// 	subsumer_to_remove = (ObjectPropertyExpressionId) SET_ITERATOR_NEXT(&remove_iterator);
		// }
		// SET_RESET(&subsumers_to_remove);


		object_property_chain = MAP_ITERATOR_NEXT(&map_iterator);
	}

	MAP_ITERATOR_INIT(&map_iterator, &(kb->tbox->objectproperty_chains));
	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	while (object_property_chain != KEY_NOT_FOUND_IN_HASH_MAP) {
		printf("==================\n%s\n", object_property_expression_to_string(kb, object_property_chain));
		subsumers_to_remove_set = MAP_GET_64(object_property_chain, &subsumers_to_remove_map);


		// if (subsumers_to_remove_set == NULL) {
		// 	printf("subsumers EMPTY\n");
		// 	object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
		// 	continue;
		// }

		SetIterator subsumers_iterator;
		SET_ITERATOR_INIT(&subsumers_iterator, subsumers_to_remove_set);
		ObjectPropertyExpressionId subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
		while (subsumer != HASH_TABLE_KEY_NOT_FOUND) {
			if (SET_REMOVE(subsumer, &(OPEXP(object_property_chain).subsumers))) {
				printf("%s removed\n", object_property_expression_to_string(kb, subsumer));
			}
			else{
				printf("%s NOT FOUND in subsumers!\n", object_property_expression_to_string(kb, subsumer));
			}
			subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
		}
		printf("\n\n\n");

		// now free the set of removed subsumers
		SET_FREE(subsumers_to_remove_set);

		object_property_chain =  MAP_ITERATOR_NEXT(&map_iterator);
	}

	// now free the map of subsumers to remove
	MAP_RESET_64(&(subsumers_to_remove_map));
}

*/
