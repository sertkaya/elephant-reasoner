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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../model/datatypes.h"
#include "../model/limits.h"
#include "utils.h"
#include "../saturation/utils.h"
#include "../hashing/hash_table.h"


void compute_concept_hierarchy(TBox* tbox) {
	int is_direct_subsumer;

	int i;
	for (i = 0; i < tbox->atomic_concept_count; ++i) {
		SetIterator* subsumers_iterator = SET_ITERATOR_CREATE(tbox->atomic_concept_list[i]->subsumers);
		ClassExpression* subsumer = (ClassExpression*) SET_ITERATOR_NEXT(subsumers_iterator);
		while (subsumer != NULL) {

			if (subsumer->type != CLASS_TYPE) {
				subsumer = (ClassExpression*) SET_ITERATOR_NEXT(subsumers_iterator);
				continue;
			}
			// check if tbox->atomic_concept_list[i] is a subsumer of the 'subsumer'
			// if yes, then they are equivalent
			if (IS_SUBSUMED_BY(subsumer, tbox->atomic_concept_list[i])) {
				if (subsumer != tbox->atomic_concept_list[i])
					ADD_EQUIVALENT_CLASS(subsumer, tbox->atomic_concept_list[i]);
			}
			else {
				is_direct_subsumer = 1;
				SetIterator* direct_subsumers_iterator = SET_ITERATOR_CREATE(tbox->atomic_concept_list[i]->description.atomic->direct_subsumers);
				void* direct_subsumer = SET_ITERATOR_NEXT(direct_subsumers_iterator);

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
						REMOVE_DIRECT_SUBSUMER(direct_subsumer, tbox->atomic_concept_list[i]);

					direct_subsumer = SET_ITERATOR_NEXT(direct_subsumers_iterator);
				}
				SET_ITERATOR_FREE(direct_subsumers_iterator);


				if (is_direct_subsumer)
					ADD_DIRECT_SUBSUMER(subsumer, tbox->atomic_concept_list[i]);
			}
			subsumer = (ClassExpression*) SET_ITERATOR_NEXT(subsumers_iterator);
		}
		SET_ITERATOR_FREE(subsumers_iterator);
	}

}


