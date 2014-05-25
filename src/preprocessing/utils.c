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
#include "../model/limits.h"
#include "../hashing/key_value_hash_table.h"
#include "../hashing/utils.h"

/*
// The list of subclass axioms that are generated during  preprocessing
extern SubClassAxiom** generated_subclass_axioms;
extern int generated_subclass_axiom_count;

// The list of subrole axioms that are generated during  preprocessing
extern SubRoleAxiom** generated_subrole_axioms;
extern int generated_subrole_axiom_count;

// The hash of nominals that are generated during preprocessing.
extern KeyValueHashTable* generated_nominals;

// The hash of existential restrictions that are generated during preprocessing.
// They are generated from preprocessing role assertions.
extern KeyValueHashTable* generated_exists_restrictions;
extern int generated_exists_restriction_count;
*/

/******************************************************************************
 * Add functions for axioms
 *****************************************************************************/

// Add a given subclass axiom to the list of subclass axioms generated during
// preprocessing
void add_generated_subclass_axiom(KB* kb, SubClassAxiom* ax) {
	SubClassAxiom** tmp;
	tmp = realloc(kb->generated_subclass_axioms, (kb->generated_subclass_axiom_count + 1) * sizeof(SubClassAxiom*));
	assert(tmp != NULL);
	kb->generated_subclass_axioms = tmp;
	kb->generated_subclass_axioms[kb->generated_subclass_axiom_count] = ax;
	++(kb->generated_subclass_axiom_count);
}

// Add a given subrole axiom the list of subclass axioms generated during
// preprocessing
void add_generated_subrole_axiom(KB* kb, SubRoleAxiom* ax) {
	SubRoleAxiom** tmp;
	tmp = realloc(kb->generated_subrole_axioms, (kb->generated_subrole_axiom_count + 1) * sizeof(SubRoleAxiom*));
	assert(tmp != NULL);
	kb->generated_subrole_axioms = tmp;
	kb->generated_subrole_axioms[kb->generated_subrole_axiom_count] = ax;
	++(kb->generated_subrole_axiom_count);
}


Concept* get_create_generated_nominal(KB* kb, Individual* ind) {
	Concept* c;

	// check if the nominal with this individual already exists
	if ((c = get_value(kb->generated_nominals, ind->id)) != NULL)
		return c;

	// if a nominal with the individual does not already exist, create it
	c = (Concept*) malloc(sizeof(Concept));
	assert(c != NULL);

	c->description.nominal = (Nominal*) malloc(sizeof(Nominal));
	assert(c->description.nominal != NULL);

	c->description.nominal->individual = ind;

	c->type = NOMINAL;
	// c->id = ind->id;
	c->id = kb->tbox->last_concept_id++;

	c->told_subsumers = NULL;
	c->told_subsumer_count = 0;

	c->subsumer_list = NULL;
	c->subsumer_count = 0;
	c->subsumers = create_key_hash_table(DEFAULT_SUBSUMERS_HASH_SIZE);

	c->filler_of_negative_exists = NULL;

	// initialize the 2-dim dynamic predecessors array
	c->predecessors = NULL;
	c->predecessor_r_count = 0;

	c->successors = NULL;
	c->successor_r_count = 0;

	c->first_conjunct_of_count = 0;
	// space will be allocated in indexing
	c->first_conjunct_of_list = NULL;
	c->first_conjunct_of = NULL;

	c->second_conjunct_of_count = 0;
	// space will be allocated in indexing
	c->second_conjunct_of_list = NULL;
	c->second_conjunct_of = NULL;

	insert_key_value(kb->generated_nominals, ind->id, c);

	/*
	++tbox->nominal_count;
	tmp = realloc(tbox->nominal_list, tbox->nominal_count * sizeof(Concept*));
	assert(tmp != NULL);
	tbox->nominal_list = tmp;
	tbox->nominal_list[tbox->nominal_count - 1] = c;
	 */

	return c;
}


// get or create generated the existential restriction with role r and filler f
Concept* get_create_generated_exists_restriction(KB* kb, Role* r, Concept* f) {
	Concept* c;

	// first check if we already created an existential
	// restriction with the same role and filler
	if ((c = get_value(kb->generated_exists_restrictions, HASH_INTEGERS(r->id, f->id))) != NULL)
		return c;

	// if it does not already exist, create it
	c = (Concept*) malloc(sizeof(Concept));
	assert(c != NULL);

	c->type = EXISTENTIAL_RESTRICTION;
	c->description.exists = (Exists*) malloc(sizeof(Exists));
	assert(c->description.exists != NULL);
	c->description.exists->role = r;
	c->description.exists->filler = f;
	// c->id = (kb->generated_exists_restriction_count)++;
	c->id = kb->tbox->last_concept_id++;

	c->told_subsumers = NULL;
	c->told_subsumer_count = 0;

	c->subsumer_list = NULL;
	c->subsumer_count = 0;
	c->subsumers = create_key_hash_table(DEFAULT_SUBSUMERS_HASH_SIZE);

	c->filler_of_negative_exists = NULL;

	// initialize the 2-dim dynamic predecessors array
	c->predecessors = NULL;
	c->predecessor_r_count = 0;

	c->successors = NULL;
	c->successor_r_count = 0;

	c->first_conjunct_of_count = 0;
	// space will be allocated in indexing
	c->first_conjunct_of_list = NULL;
	c->first_conjunct_of = NULL;

	c->second_conjunct_of_count = 0;
	// space will be allocated in indexing
	c->second_conjunct_of_list = NULL;
	c->second_conjunct_of = NULL;

	++kb->generated_exists_restriction_count;

	insert_key_value(kb->generated_exists_restrictions, HASH_INTEGERS(r->id, f->id), c);

	return c;
}
