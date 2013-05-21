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

// add c1 to the list of direct equivalent of c2
void add_equivalent_concept(Concept* c1, Concept* c2) {
	int ret_code;

	J1S(ret_code, c2->equivalent_concepts, (Word_t) c1);
	if (ret_code == JERR) {
		fprintf(stderr, "could not add to the list of equivalent concepts, aborting");
		exit(EXIT_FAILURE);
	}
}

// returns 1 if c1 is a subsumer of c2, otherwise 0
int is_subsumer_of(Concept* c1, Concept* c2) {
	int is_subsumer;

	J1T(is_subsumer, c2->subsumers, (Word_t) c1);
	return is_subsumer;
}

// returns 1 if c1 is a direct subsumer of c2, otherwise 0
int is_direct_subsumer_of(Concept* c1, Concept* c2) {
	int is_direct_subsumer;

	J1T(is_direct_subsumer, c2->direct_subsumers, (Word_t) c1);
	return is_direct_subsumer;
}

// returns 1 if c1 is in the list of equivalent concepts of c2
int is_equivalent_concept_of(Concept* c1, Concept* c2) {
	int is_equivalent;

	J1T(is_equivalent, c2->equivalent_concepts, (Word_t) c1);
	return is_equivalent;
}

// add c1 to the list of direct subsumers of c2
void add_direct_subsumer(Concept* c1, Concept* c2) {
	int ret_code;

	J1S(ret_code, c2->direct_subsumers, (Word_t) c1);
	if (ret_code == JERR) {
		fprintf(stderr, "could not add to the list of direct subsumers, aborting");
		exit(EXIT_FAILURE);
	}
}

// remove c1 from the list of direct subsumers of c2
void remove_direct_subsumer(Concept* c1, Concept* c2) {
	int ret_code;

	J1U(ret_code, c2->direct_subsumers, (Word_t) c1);
	if (ret_code == JERR) {
		fprintf(stderr, "could not remove from the list of direct subsumers, aborting");
		exit(EXIT_FAILURE);
	}
}
