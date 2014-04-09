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
#include <string.h>
#include <stdlib.h>

#include "datatypes.h"
#include "../../hashing/key_value_hash_table.h"



int free_individual(Individual* ind) {
	int total_freed_bytes = 0;

	total_freed_bytes += sizeof(char) * strlen(ind->name);
	free(ind->name);

	// finally free this individual
	total_freed_bytes += sizeof(Individual);
	free(ind);

	return total_freed_bytes;
}

int free_abox(ABox* abox) {
	int i, total_freed_bytes = 0;

	// free class assertions
	for (i = 0; i < abox->concept_assertion_count; ++i)
		free(abox->concept_assertions[i]);
	total_freed_bytes += sizeof(ConceptAssertion) * abox->concept_assertion_count;
	free(abox->concept_assertions);
	total_freed_bytes += sizeof(ConceptAssertion*) * abox->concept_assertion_count;

	return total_freed_bytes;
}
