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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../model/datatypes.h"

// add c1 to the list of equivalent concepts of c2
int add_equivalent_concept(Concept* c1, Concept* c2) {

	Concept** tmp = realloc(c2->description.atomic->equivalent_concepts_list,
			sizeof(Concept*) * (c2->description.atomic->equivalent_concepts_count + 1));
	assert(tmp != NULL);
	c2->description.atomic->equivalent_concepts_list = tmp;
	c2->description.atomic->equivalent_concepts_list[c2->description.atomic->equivalent_concepts_count] = c1;
	++c2->description.atomic->equivalent_concepts_count;

	return 1;
}

// add c1 to the list of direct subsumers of c2
int add_direct_subsumer(Concept* c1, Concept* c2) {
	int added = insert_key(c2->description.atomic->direct_subsumers, c1->id);

	if (added) {
		Concept** tmp = realloc(c2->description.atomic->direct_subsumer_list,
				sizeof(Concept*) * (c2->description.atomic->direct_subsumer_count + 1));
		assert(tmp != NULL);
		c2->description.atomic->direct_subsumer_list = tmp;
		c2->description.atomic->direct_subsumer_list[c2->description.atomic->direct_subsumer_count] = c1;
		++c2->description.atomic->direct_subsumer_count;
	}

	return added;
}


// remove c1 from the list of direct subsumers of c2
int remove_direct_subsumer(Concept* c1, Concept* c2) {
	int removed = remove_key(c2->description.atomic->direct_subsumers, c1->id);

	if (removed) {
		int i;
		for (i = 0; i < c2->description.atomic->direct_subsumer_count; ++i)
			if (c2->description.atomic->direct_subsumer_list[i] == c1)
				break;

		// i is the index of the concept to be removed from the direct subsumers list
		int j;
		for (j = i; j < c2->description.atomic->direct_subsumer_count - 1; ++j)
			c2->description.atomic->direct_subsumer_list[j] = c2->description.atomic->direct_subsumer_list[j + 1];

		Concept** tmp = realloc(c2->description.atomic->direct_subsumer_list, sizeof(Concept*) * (c2->description.atomic->direct_subsumer_count - 1));
		if (c2->description.atomic->direct_subsumer_count > 1)
			assert(tmp != NULL);
		c2->description.atomic->direct_subsumer_list = tmp;
		--c2->description.atomic->direct_subsumer_count;
	}
	return removed;
}
