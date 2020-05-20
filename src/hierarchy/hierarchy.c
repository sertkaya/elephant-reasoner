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
#include <string.h>
#include <assert.h>

#include "../model/datatypes.h"
#include "../model/print_utils.h"
#include "../model/limits.h"
#include "utils.h"
#include "../saturation/utils.h"
#include "../hashing/hash_table.h"


void compute_concept_hierarchy(KB* kb) {
	int is_direct_subsumer;

	MapIterator map_it;
	SetIterator direct_subsumers_iterator;
	SetIterator subsumers_iterator;

	// Add the top class to the subsumers of every atomic concept.
	// Whether top is a direct subsumer or not will be computed below
	MAP_ITERATOR_INIT(&map_it, &(kb->tbox->classes));
	ClassExpressionId atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	while (atomic_concept != KEY_NOT_FOUND_IN_HASH_MAP) {
		SET_ADD(kb->tbox->top_concept, &(kb->tbox->class_expressions[atomic_concept].subsumers));
		atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	}

	MAP_ITERATOR_INIT(&map_it, &(kb->tbox->classes));
	atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	while (atomic_concept != KEY_NOT_FOUND_IN_HASH_MAP) {
		SET_ITERATOR_INIT(&subsumers_iterator, &(kb->tbox->class_expressions[atomic_concept].subsumers));
		ClassExpressionId subsumer = (ClassExpressionId) SET_ITERATOR_NEXT(&subsumers_iterator);
		while (subsumer != HASH_TABLE_KEY_NOT_FOUND) {

			if (kb->tbox->class_expressions[subsumer].type != CLASS_TYPE) {
				subsumer = (ClassExpressionId) SET_ITERATOR_NEXT(&subsumers_iterator);
				continue;
			}
			// check if tbox->atomic_concept_list[i] is a subsumer of the 'subsumer'
			// if yes, then they are equivalent
			if (IS_SUBSUMED_BY(subsumer,  atomic_concept,kb->tbox)) {
				if (subsumer != (ClassExpressionId) atomic_concept)
					ADD_EQUIVALENT_CLASS(subsumer,  atomic_concept, kb->tbox);
			}
			else {
				is_direct_subsumer = 1;
				SET_ITERATOR_INIT(&direct_subsumers_iterator,  &(kb->tbox->class_expressions[atomic_concept].description.atomic.direct_subsumers));
				ClassExpressionId direct_subsumer = SET_ITERATOR_NEXT(&direct_subsumers_iterator);

				while (direct_subsumer != HASH_TABLE_KEY_NOT_FOUND) {
					// check if the 'subsumer' is a subsumer of the 'direct_subsumer'
					// if yes, then the 'subsumer' is not a direct subsumer of tbox->atomic_concept_list[i]
					if (IS_SUBSUMED_BY(direct_subsumer, subsumer, kb->tbox)) {
						is_direct_subsumer = 0;
						break;
					}
					// now check if the 'direct_subsumer' is a subsumer of the 'subsumer'
					// if yes, then the 'direct_subsumer' is not a direct subsumer of tbox->atomic_concept_list[i]
					// remove it from the list of direct subsumers
					if (IS_SUBSUMED_BY(subsumer, direct_subsumer,kb->tbox)) {
						// printf("%s|%s\n", class_expression_to_string(kb, atomic_concept), class_expression_to_string(kb, direct_subsumer));
						REMOVE_DIRECT_SUBSUMER(direct_subsumer,  atomic_concept, kb->tbox);
					}

					direct_subsumer = SET_ITERATOR_NEXT(&direct_subsumers_iterator);
				}

				if (is_direct_subsumer)
					ADD_DIRECT_SUBSUMER(subsumer,  atomic_concept, kb->tbox);
			}
			subsumer = (ClassExpressionId) SET_ITERATOR_NEXT(&subsumers_iterator);
		}
		atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	}

}


