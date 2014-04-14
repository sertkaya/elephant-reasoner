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
#include <string.h>
#include <assert.h>

#include "datatypes.h"
#include "utils.h"

/******************************************************************************
 * get/create functions for concepts
 *****************************************************************************/

// TODO: stub implementation!
Individual* get_create_individual(char* name, ABox* abox) {
	Individual* i;
	Individual** tmp;

	// check if an individual with this name already exists in the ABox
	if ((i = (Individual*) GET_INDIVIDUAL((unsigned char*) name, abox)) != NULL)
		return i;

	// if an individual with the name does not already exist, create it
	i = (Individual*) malloc(sizeof(Individual));
	assert(i != NULL);

	i->id = abox->last_individual_id++;

	i->name = (char*) malloc((strlen(name) + 1) * sizeof(char));
	assert(i->name != NULL);
	strcpy(i->name, name);

	PUT_INDIVIDUAL(i->name, i, abox);

	++abox->individual_count;

	return i;
}

/******************************************************************************
 * create functions for assertions
 *****************************************************************************/
// create the concept assertion with the given individual and concept
ConceptAssertion* create_concept_assertion(Individual* ind, Concept* c) {
	ConceptAssertion* as = (ConceptAssertion*) malloc(sizeof(ConceptAssertion));
	assert(as != NULL);
	as->individual = ind;
	as->concept = c;

	return as;
}

// create the role assertion with the given role and individuals
RoleAssertion* create_role_assertion(Role* role, Individual* source_ind, Individual* target_ind) {
	RoleAssertion* as = (RoleAssertion*) malloc(sizeof(RoleAssertion));
	assert(as != NULL);
	as->role = role;
	as->source_individual = source_ind;
	as->target_individual = target_ind;

	return as;
}

/******************************************************************************
 * add functions for assertions
 *****************************************************************************/

// add a given concept assertion to a given ABox
void add_concept_assertion(ConceptAssertion* as, ABox* abox) {
	ConceptAssertion** tmp;
	tmp = realloc(abox->concept_assertions, (abox->concept_assertion_count + 1) * sizeof(ConceptAssertion*));
	assert(tmp != NULL);
	abox->concept_assertions = tmp;
	abox->concept_assertions[abox->concept_assertion_count] = as;
	++abox->concept_assertion_count;
}

// add a given role assertion to a given ABox
void add_role_assertion(RoleAssertion* as, ABox* abox) {
	RoleAssertion** tmp;
	tmp = realloc(abox->role_assertions, (abox->role_assertion_count + 1) * sizeof(RoleAssertion*));
	assert(tmp != NULL);
	abox->role_assertions = tmp;
	abox->role_assertions[abox->role_assertion_count] = as;
	++abox->role_assertion_count;
}
