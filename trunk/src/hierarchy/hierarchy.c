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


#include <Judy.h>

#include "../model/datatypes.h"
#include "../model/limits.h"
#include "utils.h"
#include "../saturation/utils.h"
#include "../hashing/key_hash_table.h"


void compute_concept_hierarchy(TBox* tbox) {

	int is_direct_subsumer;
	Word_t direct_subsumer_index;
	int is_direct_subsumer_list_nonempty;

	int i, j;
	for (i = 0; i < tbox->atomic_concept_count; ++i) {
		for (j = 0; j < tbox->atomic_concept_list[i]->subsumer_count; ++j) {
			if (tbox->atomic_concept_list[i]->subsumer_list[j]->type != ATOMIC_CONCEPT)
				continue;
			// check if tbox->atomic_concept_list[i] is a subsumer of tbox->atomic_concept_list[i]->subsumer_list[j]
			// if yes, then they are equivalent
			if (IS_SUBSUMED_BY(tbox->atomic_concept_list[i]->subsumer_list[j], tbox->atomic_concept_list[i])) {
				if (tbox->atomic_concept_list[i]->subsumer_list[j] != tbox->atomic_concept_list[i])
					add_equivalent_concept(tbox->atomic_concept_list[i]->subsumer_list[j], tbox->atomic_concept_list[i]);
			}
			else {
				is_direct_subsumer = 1;
				int k,l;
				for (k = 0; k < tbox->atomic_concept_list[i]->description.atomic->direct_subsumer_count; ++k) {
					// check if tbox->atomic_concept_list[i]->subsumer_list[j] is a subsumer of the direct_subsumer
					// if yes, then tbox->atomic_concept_list[i]->subsumer_list[j] is not a direct subsumer of tbox->atomic_concept_list[i]
					if (IS_SUBSUMED_BY(tbox->atomic_concept_list[i]->description.atomic->direct_subsumer_list[k],
							tbox->atomic_concept_list[i]->subsumer_list[j])) {
						is_direct_subsumer = 0;
						break;
					}
					// now check if direct_subsumer is a subsumer of tbox->atomic_concept_list[i]->subsumer_list[j]
					// if yes, then direct_subsumer_index is not a direct subsumer of tbox->atomic_concept_list[i]
					// remove it from the list of direct subsumers
					if (IS_SUBSUMED_BY(tbox->atomic_concept_list[i]->subsumer_list[j],
							tbox->atomic_concept_list[i]->description.atomic->direct_subsumer_list[k]))
						remove_direct_subsumer(tbox->atomic_concept_list[i]->description.atomic->direct_subsumer_list[k],
								tbox->atomic_concept_list[i]);
				}
				/*
				direct_subsumer_index = 0;
				J1F(is_direct_subsumer_list_nonempty, tbox->atomic_concept_list[i]->description.atomic->direct_subsumers, direct_subsumer_index);
				while (is_direct_subsumer_list_nonempty) {
					// check if tbox->atomic_concept_list[i]->subsumer_list[j] is a subsumer of the direct_subsumer_index
					// if yes, then tbox->atomic_concept_list[i]->subsumer_list[j] is not a direct subsumer of tbox->atomic_concept_list[i]
					if (IS_SUBSUMED_BY(((Concept*) direct_subsumer_index), tbox->atomic_concept_list[i]->subsumer_list[j])) {
						is_direct_subsumer = 0;
						break;
					}
					// now check if direct_subsumer_index is a subsumer of tbox->atomic_concept_list[i]->subsumer_list[j]
					// if yes, then direct_subsumer_index is not a direct subsumer of tbox->atomic_concept_list[i]
					// remove it from the list of direct subsumers
					if (IS_SUBSUMED_BY(tbox->atomic_concept_list[i]->subsumer_list[j], ((Concept*) direct_subsumer_index)))
						REMOVE_DIRECT_SUBSUMER(((Concept*) direct_subsumer_index), tbox->atomic_concept_list[i]);

					J1N(is_direct_subsumer_list_nonempty, tbox->atomic_concept_list[i]->description.atomic->direct_subsumers, direct_subsumer_index);
				}
				*/
				if (is_direct_subsumer)
					add_direct_subsumer(tbox->atomic_concept_list[i]->subsumer_list[j], tbox->atomic_concept_list[i]);
			}
		}
	}
}


