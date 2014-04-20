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
#include "datatypes.h"


/******************************************************************************
 * add functions for axioms
 *****************************************************************************/

// add a given subclass axiom to the list of subclass axioms generated during
// preprocessing
void add_generated_subclass_axiom(SubClassAxiom* ax) {
	SubClassAxiom** tmp;
	tmp = realloc(generated_subclass_axioms, (generated_subclass_axiom_count + 1) * sizeof(SubClassAxiom*));
	assert(tmp != NULL);
	generated_subclass_axioms = tmp;
	generated_subclass_axioms[generated_subclass_axiom_count] = ax;
	generated_subclass_axiom_count++;
}
