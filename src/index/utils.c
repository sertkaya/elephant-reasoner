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
#include "../model/limits.h"
#include "../hashing/hash_map.h"



/******************************************************************************
 * functions for adding subsumer/told subsumer to a role
 *****************************************************************************/

// add role s to the subsumer list of r
// returns 1 if added, 0 otherwise
// note that it maintains both the array and the hash table
// of subsumers. the reason for keeping the subsumers twice
// is performance in saturation
int add_to_role_subsumer_list(ObjectPropertyExpressionId r, ObjectPropertyExpressionId s, TBox* tbox) {
	if (SET_ADD(s, &(tbox->object_property_expressions[r].subsumers))) {
		list_add(s, &(tbox->object_property_expressions[r].subsumer_list));

		return 1;
	}

	return 0;
}

int add_to_role_subsumee_list(ObjectPropertyExpressionId r, ObjectPropertyExpressionId s, TBox* tbox) {
	if (SET_ADD(s, &(tbox->object_property_expressions[r].subsumees)))
		return 1;

	return 0;
}

/******************************************************************************
 * functions for adding ...
 *****************************************************************************/
// add 'conjunction' to the list of conjunctions whose first conjunct is 'concept'
// note that for performance reasons in saturation, this information is kept twice:
// once in a hash table, once in a usual array. the hash table is for searching
// during saturation, the usual array is for iteration on the elements. the memory
// overhead is worth the performance gain.

void add_to_first_conjunct_of_list(ClassExpressionId concept, ClassExpressionId conjunction, TBox* tbox) {
	// create the hash if we are adding it for the first time
	if (tbox->class_expressions[concept].first_conjunct_of == NULL)
		tbox->class_expressions[concept].first_conjunct_of = SET_CREATE(DEFAULT_FIRST_CONJUNCT_OF_HASH_SIZE);

	if (SET_ADD(conjunction, tbox->class_expressions[concept].first_conjunct_of)) {
		list_add(conjunction, &(tbox->class_expressions[concept].first_conjunct_of_list));
	}
}

// add 'conjunction' to the list of conjunctions whose second conjunct is 'concept'
// (see the note for the function add_to_first_conjunct_of_list above)
void add_to_second_conjunct_of_list(ClassExpressionId concept, ClassExpressionId conjunction, TBox* tbox) {
	// create the hash if we are adding it for the first time
	if (tbox->class_expressions[concept].second_conjunct_of == NULL)
		tbox->class_expressions[concept].second_conjunct_of = SET_CREATE(DEFAULT_SECOND_CONJUNCT_OF_HASH_SIZE);

	if (SET_ADD(conjunction, tbox->class_expressions[concept].second_conjunct_of)) {
		list_add(conjunction, &(tbox->class_expressions[concept].second_conjunct_of_list));
	}
}

// add ex to the filler_of_negative_exists hash of the filler of ex.
void add_to_negative_exists(ClassExpressionId ex, TBox* tbox) {
	// create the set if we are adding it for the first time
	if (tbox->class_expressions[tbox->class_expressions[ex].description.exists.filler].filler_of_negative_exists == NULL)
		tbox->class_expressions[tbox->class_expressions[ex].description.exists.filler].filler_of_negative_exists = MAP_CREATE(DEFAULT_NEGATIVE_FILLER_OF_SET_SIZE);
	MAP_PUT(tbox->class_expressions[ex].description.exists.role, ex, tbox->class_expressions[tbox->class_expressions[ex].description.exists.filler].filler_of_negative_exists);
}

// add 'composition' to the list of compositions whose first component is 'role'
// note that for performance reasons in saturation, this information is kept twice:
// once in a hash table, once in a usual array. the hash table is for searching
// during saturation, the usual array is for iteration on the elements. the memory
// overhead is worth the performance gain.
void add_role_to_first_component_of_list(ObjectPropertyExpressionId role, ObjectPropertyExpressionId composition, TBox* tbox) {
	ObjectPropertyExpressionId* tmp;

	if (SET_ADD(composition, &(tbox->object_property_expressions[role].first_component_of))) {
		tmp = realloc(tbox->object_property_expressions[role].first_component_of_list, (tbox->object_property_expressions[role].first_component_of_count + 1) * sizeof(ObjectPropertyExpressionId));
		assert(tmp != NULL);
		tbox->object_property_expressions[role].first_component_of_list = tmp;
		tbox->object_property_expressions[role].first_component_of_list[tbox->object_property_expressions[role].first_component_of_count] = composition;
		tbox->object_property_expressions[role].first_component_of_count++;
	}
}

// add 'composition' to the list of compositions whose second component is 'role'
// (see the note for the function add_role_to_second_component_of_list above)
void add_role_to_second_component_of_list(ObjectPropertyExpressionId role, ObjectPropertyExpressionId composition, TBox* tbox) {
	ObjectPropertyExpressionId* tmp;

	if (SET_ADD(composition, &(tbox->object_property_expressions[role].second_component_of))) {
		tmp = realloc(tbox->object_property_expressions[role].second_component_of_list, (tbox->object_property_expressions[role].second_component_of_count + 1) * sizeof(ClassExpressionId));
		assert(tmp != NULL);
		tbox->object_property_expressions[role].second_component_of_list = tmp;
		tbox->object_property_expressions[role].second_component_of_list[tbox->object_property_expressions[role].second_component_of_count] = composition;
		tbox->object_property_expressions[role].second_component_of_count++;
	}
}
