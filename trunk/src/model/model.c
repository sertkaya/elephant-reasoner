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

#include "../hashing/key_hash_table.h"
#include "datatypes.h"
#include "model.h"
#include "utils.h"
#include "limits.h"

/******************************************************************************
 * get/create functions for concepts
 *****************************************************************************/

Concept* get_create_atomic_concept(char* name, TBox* tbox) {
	Concept* c;
	Concept** tmp;

	// check if the atomic concept with this name already exists
	if ((c = get_atomic_concept((unsigned char*) name, tbox)) != NULL)
		return c;

	// if an atomic concept with the name does not already exist, create it
	c = (Concept*) malloc(sizeof(Concept));
	assert(c != NULL);

	c->description.atomic = (AtomicConcept*) malloc(sizeof(AtomicConcept));
	assert(c->description.atomic != NULL);

	c->description.atomic->name = (char*) malloc((strlen(name) + 1) * sizeof(char));
	assert(c->description.atomic->name != NULL);

	strcpy(c->description.atomic->name, name);

	c->description.atomic->direct_subsumers = create_key_hash_table(DEFAULT_DIRECT_SUBSUMERS_HASH_SIZE);
	c->description.atomic->direct_subsumer_list = NULL;
	c->description.atomic->direct_subsumer_count = 0;

	c->description.atomic->equivalent_concepts_list = NULL;
	c->description.atomic->equivalent_concepts_count = 0;

	c->type = ATOMIC_CONCEPT;
	c->id = tbox->last_concept_id++;

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

	put_atomic_concept((unsigned char*) c->description.atomic->name, c, tbox);
	tbox->atomic_concept_count++;
	tmp = realloc(tbox->atomic_concept_list, tbox->atomic_concept_count * sizeof(Concept*));
	assert(tmp != NULL);
	tbox->atomic_concept_list = tmp;
	tbox->atomic_concept_list[tbox->atomic_concept_count - 1] = c;

	return c;
}

// get or create the existential restriction with role r and filler f
Concept* get_create_exists_restriction(Role* r, Concept* f, TBox* tbox) {
	Concept* c;

	tbox->exists_restriction_count++;
	// first check if we already created an existential
	// restriction with the same role and filler
	if ((c = get_exists_restriction(r->id, f->id, tbox)) != NULL)
		return c;

	// if it does not already exist, create it
	c = (Concept*) malloc(sizeof(Concept));
	assert(c != NULL);

	c->type = EXISTENTIAL_RESTRICTION;
	c->description.exists = (Exists*) malloc(sizeof(Exists));
	assert(c->description.exists != NULL);
	c->description.exists->role = r;
	c->description.exists->filler = f;
	c->id = tbox->last_concept_id++;

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

	put_exists_restriction(r->id, f->id, c, tbox);
	tbox->unique_exists_restriction_count++;

	return c;
}

Concept* get_create_conjunction_binary(Concept* c1, Concept* c2, TBox* tbox) {
	Concept* c;

	tbox->binary_conjunction_count++;
	if ((c = get_conjunction(c1, c2, tbox)) != NULL)
		return c;

	c = (Concept*) malloc(sizeof(Concept));
	assert(c != NULL);

	c->type = CONJUNCTION;
	c->description.conj = (Conjunction*) malloc(sizeof(Conjunction));
	assert(c->description.conj != NULL);

	// we order the conjuncts!
	if (c1->id < c2->id) {
		c->description.conj->conjunct1 = c1;
		c->description.conj->conjunct2 = c2;
	}
	else {
		c->description.conj->conjunct1 = c2;
		c->description.conj->conjunct2 = c1;
	}
	c->id = tbox->last_concept_id++;

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

	put_conjunction(c, tbox);
	tbox->unique_binary_conjunction_count++;
	// tbox->unique_conjunction_count++;

	return c;
}

// comparison function for qsorting conjucts based on ids
static int compare_conjunct(const void* c1, const void* c2) {
	if (((Concept*) c1)->id < ((Concept*) c2)->id)
		return -1;
	if (((Concept*) c1)->id == ((Concept*) c2)->id)
		return 0;
	return 1;
}

// returns a binary conjunction from a given list of conjuncts.
// called by the parser.
Concept* get_create_conjunction(int size, Concept** conjuncts, TBox* tbox) {
	int i;
	Concept* conjunction = NULL;

	tbox->conjunction_count++;
	qsort(conjuncts, size, sizeof(Concept*), compare_conjunct);
	conjunction = get_create_conjunction_binary(conjuncts[0], conjuncts[1], tbox);
	for (i = 2; i < size; i++)
		conjunction = get_create_conjunction_binary(conjunction, conjuncts[i], tbox);

	return conjunction;
}

Concept* get_create_nominal(Individual* ind, TBox* tbox) {
	Concept* c;
	Concept** tmp;

	// check if the nominal with this individual already exists
	if ((c = GET_NOMINAL(ind, tbox)) != NULL)
		return c;

	// if a nominal with the individual does not already exist, create it
	c = (Concept*) malloc(sizeof(Concept));
	assert(c != NULL);

	c->description.nominal = (Nominal*) malloc(sizeof(Nominal));
	assert(c->description.nominal != NULL);

	c->description.nominal->individual = ind;

	c->type = NOMINAL;
	c->id = tbox->last_concept_id++;

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

	PUT_NOMINAL(c, tbox);

	/*
	++tbox->nominal_count;
	tmp = realloc(tbox->nominal_list, tbox->nominal_count * sizeof(Concept*));
	assert(tmp != NULL);
	tbox->nominal_list = tmp;
	tbox->nominal_list[tbox->nominal_count - 1] = c;
	 */

	return c;
}
/******************************************************************************
 * get/create functions for roles
 *****************************************************************************/

Role* get_create_atomic_role(char* name, TBox* tbox) {
	Role* r;

	// check if the atomic role already exists
	if ((r = get_atomic_role((unsigned char*) name, tbox)) != NULL)
		return r;

	// if it does not already exist, create it
	r = (Role*) malloc(sizeof(Role));
	assert(r != NULL);
	r->type = ATOMIC_ROLE;
	r->description.atomic = (AtomicRole*) malloc(sizeof(AtomicRole));
	assert(r->description.atomic != NULL);
	r->description.atomic->name = (char*) malloc((strlen(name) + 1) * sizeof(char));
	assert(r->description.atomic->name != NULL);
	strcpy(r->description.atomic->name, name);
	r->id = tbox->last_role_id++;

	r->told_subsumers = create_key_value_hash_table(DEFAULT_ROLE_TOLD_SUBSUMERS_HASH_SIZE);
	// r->told_subsumer_count = 0;
	r->told_subsumees = create_key_value_hash_table(DEFAULT_ROLE_TOLD_SUBSUMEES_HASH_SIZE);

	r->subsumer_list = NULL;
	r->subsumer_count = 0;
	r->subsumers = create_key_hash_table(DEFAULT_ROLE_SUBSUMERS_HASH_SIZE);

	r->subsumee_list = NULL;
	r->subsumee_count = 0;
	r->subsumees = create_key_hash_table(DEFAULT_ROLE_SUBSUMEES_HASH_SIZE);

	r->first_component_of_count = 0;
	r->first_component_of_list = NULL;
	r->first_component_of = create_key_hash_table(DEFAULT_ROLE_FIRST_COMPONENT_OF_HASH_SIZE);

	r->second_component_of_count = 0;
	r->second_component_of_list = NULL;
	r->second_component_of = create_key_hash_table(DEFAULT_ROLE_SECOND_COMPONENT_OF_HASH_SIZE);

	put_atomic_role((unsigned char*) r->description.atomic->name, r, tbox);
	tbox->atomic_role_count++;

	return r;
}



Role* get_create_role_composition_binary(Role *r1, Role* r2, TBox* tbox) {
	Role* r;

	tbox->binary_role_composition_count++;
	if ((r = get_role_composition(r1, r2, tbox)) != NULL)
		return r;

	r = (Role*) malloc(sizeof(Role));
	assert(r != NULL);

	r->type = ROLE_COMPOSITION;
	r->description.role_composition = (RoleComposition*) malloc(sizeof(RoleComposition));
	assert(r->description.role_composition != NULL);
	// we DO assume role1 and role2 to be ordered!
	r->description.role_composition->role1 = r1;
	r->description.role_composition->role2 = r2;
	r->id = tbox->last_role_id++;

	r->told_subsumers = create_key_value_hash_table(DEFAULT_ROLE_TOLD_SUBSUMERS_HASH_SIZE);
	// r->told_subsumers = NULL;
	// r->told_subsumer_count = 0;
	r->told_subsumees = create_key_value_hash_table(DEFAULT_ROLE_TOLD_SUBSUMEES_HASH_SIZE);

	r->subsumer_list = NULL;
	r->subsumer_count = 0;
	r->subsumers = create_key_hash_table(DEFAULT_ROLE_SUBSUMERS_HASH_SIZE);

	r->subsumee_list = NULL;
	r->subsumee_count = 0;
	r->subsumees = create_key_hash_table(DEFAULT_ROLE_SUBSUMEES_HASH_SIZE);

	r->first_component_of_count = 0;
	r->first_component_of_list = NULL;
	r->first_component_of = create_key_hash_table(DEFAULT_ROLE_FIRST_COMPONENT_OF_HASH_SIZE);

	r->second_component_of_count = 0;
	r->second_component_of_list = NULL;
	r->second_component_of = create_key_hash_table(DEFAULT_ROLE_SECOND_COMPONENT_OF_HASH_SIZE);

	put_role_composition(r, tbox);
	tbox->unique_binary_role_composition_count++;

	return r;
}

// get or create the role composition consisting of the given roles.
// called by the parser.
Role* get_create_role_composition(int size, Role** roles, TBox* tbox) {
	int i;
	Role* composition = NULL;

	tbox->role_composition_count++;
	// qsort(roles, size, sizeof(Role*), compare_role);
	composition = get_create_role_composition_binary(roles[size-1], roles[size-2], tbox);
	for (i = 0 ; i < size - 2 ; ++i)
		composition = get_create_role_composition_binary(composition, roles[i], tbox);

	return composition;
}


/******************************************************************************
 * create functions for axioms
 *****************************************************************************/
// create the subclass axiom with the given concept descriptions
SubClassAxiom* create_subclass_axiom(Concept* lhs, Concept* rhs) {
	SubClassAxiom* ax = (SubClassAxiom*) malloc(sizeof(SubClassAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

// create the equivalent classes axiom with the given concept descriptions
EqClassAxiom* create_eqclass_axiom(Concept* lhs, Concept* rhs) {
	/*
	if (lhs->type != ATOMIC_CONCEPT) {
		fprintf(stderr,"the lhs of an equivalent classes axiom must be an atomic concept, aborting\n");
		exit(EXIT_FAILURE);
	}
	*/
	EqClassAxiom* ax = (EqClassAxiom*) malloc(sizeof(EqClassAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

// create the disjointclasses axiom with the given concept descriptions
DisjointClassesAxiom* create_disjointclasses_axiom(int concept_count, Concept** concepts) {
	DisjointClassesAxiom* ax = (DisjointClassesAxiom*) calloc(1, sizeof(DisjointClassesAxiom));
	assert(ax != NULL);
	ax->concepts = (Concept**) calloc(concept_count, sizeof(Concept*));
	assert(ax->concepts != NULL);
	ax->concept_count = concept_count;
	int i;
	for (i = 0; i < ax->concept_count; ++i)
		ax->concepts[i] = concepts[i];

	return ax;
}

// create the subrole axiom with the given role descriptions
SubRoleAxiom* create_subrole_axiom(Role* lhs, Role* rhs) {
	SubRoleAxiom* ax = (SubRoleAxiom*) malloc(sizeof(SubRoleAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

TransitiveRoleAxiom* create_transitive_role_axiom(Role* r) {
	TransitiveRoleAxiom* ax = (TransitiveRoleAxiom*) malloc(sizeof(TransitiveRoleAxiom));
	assert(ax != NULL);
	ax->r = r;
	return ax;
}

// create the eqrole axiom with the given role descriptions
EqRoleAxiom* create_eqrole_axiom(Role* lhs, Role* rhs) {
	EqRoleAxiom* ax = (EqRoleAxiom*) malloc(sizeof(EqRoleAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}


/******************************************************************************
 * add functions for axioms
 *****************************************************************************/

// add a given subclass axiom to a given TBox
void add_subclass_axiom(SubClassAxiom* ax, TBox* tbox) {
	SubClassAxiom** tmp;
	tmp = realloc(tbox->subclass_axioms, (tbox->subclass_axiom_count + 1) * sizeof(SubClassAxiom*));
	assert(tmp != NULL);
	tbox->subclass_axioms = tmp;
	tbox->subclass_axioms[tbox->subclass_axiom_count] = ax;
	tbox->subclass_axiom_count++;
}

// add a given equivalent classes axiom to a given TBox
void add_eqclass_axiom(EqClassAxiom* ax, TBox* tbox) {
	EqClassAxiom** tmp;
	tmp = realloc(tbox->eqclass_axioms, (tbox->eqclass_axiom_count + 1) * sizeof(EqClassAxiom*));
	assert(tmp != NULL);
	tbox->eqclass_axioms = tmp;
	tbox->eqclass_axioms[tbox->eqclass_axiom_count] = ax;
	++tbox->eqclass_axiom_count;
}

void add_disjointclasses_axiom(DisjointClassesAxiom* ax, TBox* tbox) {
	DisjointClassesAxiom** tmp;
	tmp = realloc(tbox->disjointclasses_axioms, (tbox->disjointclasses_axiom_count + 1) * sizeof(DisjointClassesAxiom*));
	assert(tmp != NULL);
	tbox->disjointclasses_axioms = tmp;
	tbox->disjointclasses_axioms[tbox->disjointclasses_axiom_count] = ax;
	++tbox->disjointclasses_axiom_count;
}

// add a given subrole axiom to a given TBox
void add_subrole_axiom(SubRoleAxiom* ax, TBox* tbox) {
	SubRoleAxiom** tmp;
	tmp = realloc(tbox->subrole_axioms, (tbox->subrole_axiom_count + 1) * sizeof(SubRoleAxiom*));
	assert(tmp != NULL);
	tbox->subrole_axioms = tmp;
	tbox->subrole_axioms[tbox->subrole_axiom_count] = ax;
	++tbox->subrole_axiom_count;
}

void add_transitive_role_axiom(TransitiveRoleAxiom* ax, TBox* tbox) {
	TransitiveRoleAxiom** tmp;
	tmp = realloc(tbox->transitive_role_axioms, (tbox->transitive_role_axiom_count + 1) * sizeof(TransitiveRoleAxiom));
	assert(tmp != NULL);
	tbox->transitive_role_axioms = tmp;
	tbox->transitive_role_axioms[tbox->transitive_role_axiom_count] = ax;
	++tbox->transitive_role_axiom_count;
}

// add a given eqrole axiom to a given TBox
void add_eqrole_axiom(EqRoleAxiom* ax, TBox* tbox) {
	EqRoleAxiom** tmp;
	tmp = realloc(tbox->eqrole_axioms, (tbox->eqrole_axiom_count + 1) * sizeof(EqRoleAxiom*));
	assert(tmp != NULL);
	tbox->eqrole_axioms = tmp;
	tbox->eqrole_axioms[tbox->eqrole_axiom_count] = ax;
	++tbox->eqrole_axiom_count;
}



/******************************************************************************
 * get/create functions for ABox
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
