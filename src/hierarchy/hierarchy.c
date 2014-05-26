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
#include "../hashing/key_hash_table.h"


void compute_concept_hierarchy(TBox* tbox) {

	int is_direct_subsumer;

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
				int k;

				// make a temporary copy of the direct_subsumer_list since we are going to iterate on it and
				// simultaneously remove elements from it.
				int original_direct_subsumer_count = tbox->atomic_concept_list[i]->description.atomic->direct_subsumer_count;
				Concept** tmp = malloc(original_direct_subsumer_count * sizeof(Concept*));
				assert(tmp != NULL);
				memcpy(tmp, tbox->atomic_concept_list[i]->description.atomic->direct_subsumer_list, original_direct_subsumer_count * sizeof(Concept*));

				for (k = 0; k < original_direct_subsumer_count; ++k) {
					// check if tbox->atomic_concept_list[i]->subsumer_list[j] is a subsumer of the direct_subsumer
					// if yes, then tbox->atomic_concept_list[i]->subsumer_list[j] is not a direct subsumer of tbox->atomic_concept_list[i]
					if (IS_SUBSUMED_BY(tmp[k], tbox->atomic_concept_list[i]->subsumer_list[j])) {
						is_direct_subsumer = 0;
						break;
					}
					// now check if direct_subsumer is a subsumer of tbox->atomic_concept_list[i]->subsumer_list[j]
					// if yes, then direct_subsumer_index is not a direct subsumer of tbox->atomic_concept_list[i]
					// remove it from the list of direct subsumers
					if (IS_SUBSUMED_BY(tbox->atomic_concept_list[i]->subsumer_list[j], tmp[k]))
						remove_direct_subsumer(tmp[k],
								tbox->atomic_concept_list[i]);
				}
				// free the temporary copy
				free(tmp);

				if (is_direct_subsumer)
					add_direct_subsumer(tbox->atomic_concept_list[i]->subsumer_list[j], tbox->atomic_concept_list[i]);
			}
		}
	}
}


