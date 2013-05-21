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
/*
void compute_concept_hierarchy(TBox* tbox) {
	PWord_t pvalue;
	// Index for iterating over the atomic concepts. The index is the
	// concept name and the value is the pointer to the atomic concept
	uint8_t atomic_concept_index[MAX_CONCEPT_NAME_LENGTH];
	// Index for iterating over the subsumers of an atomic concept. Index is
	// the pointer, value is 0 or 1.
	Word_t subsumer_index;
	int is_subsumer_list_nonempty;

	int is_direct_subsumer;
	Word_t direct_subsumer_index;
	int is_direct_subsumer_list_nonempty;

	// start with the smallest concept name
	atomic_concept_index[0] = '\0';
	JSLF(pvalue, tbox->atomic_concepts, atomic_concept_index);

	while (pvalue != NULL) {
		subsumer_index = 0;
		J1F(is_subsumer_list_nonempty, ((Concept*) *pvalue)->subsumers, subsumer_index);
		while (is_subsumer_list_nonempty) {
			if (((Concept*) subsumer_index)->type != ATOMIC_CONCEPT) {
				J1N(is_subsumer_list_nonempty, ((Concept*) *pvalue)->subsumers, subsumer_index);
				continue;
			}
			// check if *pvalue is a subsumer of subsumer_index.
			// if yes, then they are equivalent
			if (is_subsumer_of((Concept*) *pvalue, (Concept*) subsumer_index))
				add_equivalent_concept((Concept*) subsumer_index, (Concept*) *pvalue);
			else {
				is_direct_subsumer = 1;
				direct_subsumer_index = 0;
				J1F(is_direct_subsumer_list_nonempty, ((Concept*) *pvalue)->direct_subsumers,
						direct_subsumer_index);
				while (is_direct_subsumer_list_nonempty) {
					// check if subsumer_index is a subsumer of the direct_subsumer_index
					// if yes, then subsumer_index is not a direct subsumer of *pvalue
					if (is_subsumer_of((Concept*) subsumer_index, (Concept*) direct_subsumer_index)) {
						is_direct_subsumer = 0;
						break;
					}
					// now check if direct_subsumer_index is a subsumer of subsumer_index
					// if yes, then direct_subsumer_index is not a direct subsumer of *pvalue
					// remove it from the list of direct subsumers
					if (is_subsumer_of((Concept*) direct_subsumer_index, (Concept*) subsumer_index))
						remove_direct_subsumer((Concept*) direct_subsumer_index, (Concept*) *pvalue);

					J1N(is_direct_subsumer_list_nonempty, ((Concept*) *pvalue)->direct_subsumers,
							direct_subsumer_index);
				}
				if (is_direct_subsumer)
					add_direct_subsumer((Concept*) subsumer_index, (Concept*) *pvalue);
			}
			J1N(is_subsumer_list_nonempty, ((Concept*) *pvalue)->subsumers, subsumer_index);
		}
		// printf("\n");
		JSLN(pvalue, tbox->atomic_concepts, atomic_concept_index);
	}
}
*/

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
			if (is_subsumer_of(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i]->subsumer_list[j]))
				add_equivalent_concept(tbox->atomic_concept_list[i]->subsumer_list[j], tbox->atomic_concept_list[i]);
			else {
				is_direct_subsumer = 1;
				direct_subsumer_index = 0;
				J1F(is_direct_subsumer_list_nonempty, tbox->atomic_concept_list[i]->direct_subsumers, direct_subsumer_index);
				while (is_direct_subsumer_list_nonempty) {
					// check if tbox->atomic_concept_list[i]->subsumer_list[j] is a subsumer of the direct_subsumer_index
					// if yes, then tbox->atomic_concept_list[i]->subsumer_list[j] is not a direct subsumer of tbox->atomic_concept_list[i]
					if (is_subsumer_of(tbox->atomic_concept_list[i]->subsumer_list[j], (Concept*) direct_subsumer_index)) {
						is_direct_subsumer = 0;
						break;
					}
					// now check if direct_subsumer_index is a subsumer of tbox->atomic_concept_list[i]->subsumer_list[j]
					// if yes, then direct_subsumer_index is not a direct subsumer of tbox->atomic_concept_list[i]
					// remove it from the list of direct subsumers
					if (is_subsumer_of((Concept*) direct_subsumer_index, tbox->atomic_concept_list[i]->subsumer_list[j]))
						remove_direct_subsumer((Concept*) direct_subsumer_index, tbox->atomic_concept_list[i]);

					J1N(is_direct_subsumer_list_nonempty, tbox->atomic_concept_list[i]->direct_subsumers, direct_subsumer_index);
				}
				if (is_direct_subsumer)
					add_direct_subsumer(tbox->atomic_concept_list[i]->subsumer_list[j], tbox->atomic_concept_list[i]);
			}
		}
	}
}


