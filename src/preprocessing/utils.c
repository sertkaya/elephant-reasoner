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
#include <assert.h>
#include <string.h>

#include "../model/datatypes.h"
#include "../model/limits.h"
#include "../hashing/hash_map.h"
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
void add_generated_subclass_axiom(KB* kb, SubClassOfAxiom* ax) {
	SubClassOfAxiom** tmp;
	tmp = realloc(kb->generated_subclass_axioms, (kb->generated_subclass_axiom_count + 1) * sizeof(SubClassOfAxiom*));
	assert(tmp != NULL);
	kb->generated_subclass_axioms = tmp;
	kb->generated_subclass_axioms[kb->generated_subclass_axiom_count] = ax;
	++(kb->generated_subclass_axiom_count);
}

// Add a given subrole axiom the list of subclass axioms generated during
// preprocessing
void add_generated_subrole_axiom(KB* kb, SubObjectPropertyAxiom* ax) {
	SubObjectPropertyAxiom** tmp;
	tmp = realloc(kb->generated_subrole_axioms, (kb->generated_subrole_axiom_count + 1) * sizeof(SubObjectPropertyAxiom*));
	assert(tmp != NULL);
	kb->generated_subrole_axioms = tmp;
	kb->generated_subrole_axioms[kb->generated_subrole_axiom_count] = ax;
	++(kb->generated_subrole_axiom_count);
}


ClassExpression* get_create_generated_nominal(KB* kb, Individual* ind) {
	ClassExpression* c;

	// check if the nominal with this individual already exists
	if ((c = hash_map_get(kb->generated_nominals, ind->id)) != NULL)
		return c;

	// if a nominal with the individual does not already exist, create it
	c = (ClassExpression*) malloc(sizeof(ClassExpression));
	assert(c != NULL);

	c->description.nominal.individual = ind;

	c->type = OBJECT_ONE_OF_TYPE;
	// c->id = ind->id;
	c->id = kb->tbox->last_concept_id++;

	list_init(&(c->told_subsumers));

	SET_INIT(&(c->subsumers), DEFAULT_SUBSUMERS_HASH_SIZE);

	c->filler_of_negative_exists = NULL;

	// initialize the 2-dim dynamic predecessors array
	c->predecessors = NULL;
	c->predecessor_r_count = 0;

	c->successors = NULL;
	c->successor_r_count = 0;

	list_init(&(c->first_conjunct_of_list));
	SET_INIT(&(c->first_conjunct_of), DEFAULT_FIRST_CONJUNCT_OF_HASH_SIZE);

	list_init(&(c->second_conjunct_of_list));
	SET_INIT(&(c->second_conjunct_of), DEFAULT_SECOND_CONJUNCT_OF_HASH_SIZE);

	hash_map_put(kb->generated_nominals, ind->id, c);

	return c;
}


// get or create generated the existential restriction with role r and filler f
ClassExpression* get_create_generated_exists_restriction(KB* kb, ObjectPropertyExpression* r, ClassExpression* f) {
	ClassExpression* c;

	// first check if we already created an existential
	// restriction with the same role and filler
	if ((c = hash_map_get(kb->generated_exists_restrictions, HASH_INTEGERS(r->id, f->id))) != NULL)
		return c;

	// if it does not already exist, create it
	c = (ClassExpression*) malloc(sizeof(ClassExpression));
	assert(c != NULL);

	c->type = OBJECT_SOME_VALUES_FROM_TYPE;

	c->description.exists.role = r;
	c->description.exists.filler = f;
	// c->id = (kb->generated_exists_restriction_count)++;
	c->id = kb->tbox->last_concept_id++;

	list_init(&(c->told_subsumers));

	SET_INIT(&(c->subsumers), DEFAULT_SUBSUMERS_HASH_SIZE);

	c->filler_of_negative_exists = NULL;

	// initialize the 2-dim dynamic predecessors array
	c->predecessors = NULL;
	c->predecessor_r_count = 0;

	c->successors = NULL;
	c->successor_r_count = 0;

	list_init(&(c->first_conjunct_of_list));
	SET_INIT(&(c->first_conjunct_of), DEFAULT_FIRST_CONJUNCT_OF_HASH_SIZE);

	list_init(&(c->second_conjunct_of_list));
	SET_INIT(&(c->second_conjunct_of), DEFAULT_SECOND_CONJUNCT_OF_HASH_SIZE);

	++kb->generated_exists_restriction_count;

	hash_map_put(kb->generated_exists_restrictions, HASH_INTEGERS(r->id, f->id), c);

	return c;
}
