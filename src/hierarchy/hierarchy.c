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
	void* atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	while (atomic_concept) {
		// Initialize the set of direct subsumers. It is not initialized during creation of the classes in model.c
		SET_INIT(&(((ClassExpression*) atomic_concept)->description.atomic.direct_subsumers), DEFAULT_DIRECT_SUBSUMERS_SET_SIZE);
		// Initialize the set of equivalent classes. It is not initialized during creation of the classes in model.c
		SET_INIT(&(((ClassExpression*) atomic_concept)->description.atomic.equivalent_classes), DEFAULT_EQUIVALENT_CONCEPTS_SET_SIZE);
		SET_ADD(kb->tbox->top_concept, &(((ClassExpression*) atomic_concept)->subsumers));
		atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	}

	MAP_ITERATOR_INIT(&map_it, &(kb->tbox->classes));
	atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	while (atomic_concept) {
		SET_ITERATOR_INIT(&subsumers_iterator, &(((ClassExpression*) atomic_concept)->subsumers));
		ClassExpression* subsumer = (ClassExpression*) SET_ITERATOR_NEXT(&subsumers_iterator);
		while (subsumer != NULL) {

			if (subsumer->type != CLASS_TYPE) {
				subsumer = (ClassExpression*) SET_ITERATOR_NEXT(&subsumers_iterator);
				continue;
			}
			// check if tbox->atomic_concept_list[i] is a subsumer of the 'subsumer'
			// if yes, then they are equivalent
			if (IS_SUBSUMED_BY(subsumer, (ClassExpression*) atomic_concept)) {
				if (subsumer != (ClassExpression*) atomic_concept)
					ADD_EQUIVALENT_CLASS(subsumer, ((ClassExpression*) atomic_concept));
			}
			else {
				is_direct_subsumer = 1;
				SET_ITERATOR_INIT(&direct_subsumers_iterator,  &(((ClassExpression*) atomic_concept)->description.atomic.direct_subsumers));
				void* direct_subsumer = SET_ITERATOR_NEXT(&direct_subsumers_iterator);

				while (direct_subsumer != NULL) {
					// check if the 'subsumer' is a subsumer of the 'direct_subsumer'
					// if yes, then the 'subsumer' is not a direct subsumer of tbox->atomic_concept_list[i]
					if (IS_SUBSUMED_BY(((ClassExpression*) direct_subsumer), subsumer)) {
						is_direct_subsumer = 0;
						break;
					}
					// now check if the 'direct_subsumer' is a subsumer of the 'subsumer'
					// if yes, then the 'direct_subsumer' is not a direct subsumer of tbox->atomic_concept_list[i]
					// remove it from the list of direct subsumers
					if (IS_SUBSUMED_BY(subsumer, direct_subsumer))
						REMOVE_DIRECT_SUBSUMER(direct_subsumer, ((ClassExpression*) atomic_concept));

					direct_subsumer = SET_ITERATOR_NEXT(&direct_subsumers_iterator);
				}

				if (is_direct_subsumer)
					ADD_DIRECT_SUBSUMER(subsumer, ((ClassExpression*) atomic_concept));
			}
			subsumer = (ClassExpression*) SET_ITERATOR_NEXT(&subsumers_iterator);
		}
		atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	}

}


