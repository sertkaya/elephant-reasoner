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
#include <string.h>

#include "../model/datatypes.h"

// The list of subclass axioms that are generated during  preprocessing
extern SubClassAxiom** generated_subclass_axioms;
extern int generated_subclass_axiom_count;

// The list of subrole axioms that are generated during  preprocessing
extern SubRoleAxiom** generated_subrole_axioms;
extern int generated_subrole_axiom_count;

/******************************************************************************
 * Add functions for axioms
 *****************************************************************************/

// Add a given subclass axiom to the list of subclass axioms generated during
// preprocessing
void add_generated_subclass_axiom(SubClassAxiom* ax) {
	SubClassAxiom** tmp;
	tmp = realloc(generated_subclass_axioms, (generated_subclass_axiom_count + 1) * sizeof(SubClassAxiom*));
	assert(tmp != NULL);
	generated_subclass_axioms = tmp;
	generated_subclass_axioms[generated_subclass_axiom_count] = ax;
	++generated_subclass_axiom_count;
}

// Add a given subrole axiom the list of subclass axioms generated during
// preprocessing
void add_generated_subrole_axiom(SubRoleAxiom* ax) {
	SubRoleAxiom** tmp;
	tmp = realloc(generated_subrole_axioms, (generated_subrole_axiom_count + 1) * sizeof(SubRoleAxiom*));
	assert(tmp != NULL);
	generated_subrole_axioms = tmp;
	generated_subrole_axioms[generated_subrole_axiom_count] = ax;
	++generated_subrole_axiom_count;
}
