/*
 * The ELepHant Reasoner
 *
 * Copyright (C) Baris Sertkaya (sertkaya@fb2.fra-uas.de)
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

#include "../utils/set.h"
#include "datatypes.h"
#include "model.h"
#include "utils.h"
#include "limits.h"

// create ontology prefix if it does not already exist
void create_prefix(char* prefix_name, char* prefix, KB* kb) {
	if (GET_ONTOLOGY_PREFIX(prefix_name, kb) != NULL)
		return;

	list_add(prefix_name, &(kb->prefix_names));
	PUT_ONTOLOGY_PREFIX(prefix_name, prefix, kb);
}

/******************************************************************************
 * get/create functions for concepts
 *****************************************************************************/

ClassExpression* get_create_atomic_concept(char* IRI, TBox* tbox) {
	ClassExpression* c;
	ClassExpression** tmp;

	// check if the atomic concept with this name already exists
	// if ((c = get_atomic_concept((unsigned char*) name, tbox)) != NULL)
	if ((c = GET_ATOMIC_CONCEPT(IRI, tbox)) != NULL)
		return c;

	// if an atomic concept with the name does not already exist, create it
	c = (ClassExpression*) malloc(sizeof(ClassExpression));
	assert(c != NULL);

	c->description.atomic.IRI = (char*) malloc((strlen(IRI) + 1) * sizeof(char));
	assert(c->description.atomic.IRI != NULL);

	strcpy(c->description.atomic.IRI, IRI);

	c->type = CLASS_TYPE;
	c->id = tbox->next_class_expression_id++;

	LIST_INIT(&(c->told_subsumers));

	SET_INIT(&(c->subsumers), DEFAULT_SUBSUMERS_HASH_SIZE);

	c->filler_of_negative_exists = NULL;

	// initialize the 2-dim dynamic predecessors array
	c->predecessors = NULL;
	c->predecessor_r_count = 0;

	c->successors = NULL;
	c->successor_r_count = 0;

	LIST_INIT(&(c->first_conjunct_of_list));
	c->first_conjunct_of = NULL;

	LIST_INIT(&(c->second_conjunct_of_list));
	c->second_conjunct_of = NULL;

	PUT_ATOMIC_CONCEPT(c->description.atomic.IRI, c, tbox);

	return c;
}

// get or create the existential restriction with role r and filler f
ClassExpression* get_create_exists_restriction(ObjectPropertyExpression* r, ClassExpression* f, TBox* tbox) {
	ClassExpression* c;

	++tbox->object_some_values_from_exps_count;
	// first check if we already created an existential
	// restriction with the same role and filler
	// if ((c = get_exists_restriction(r->id, f->id, tbox)) != NULL)
	if ((c = GET_EXISTS_RESTRICTION(r->id, f->id, tbox)) != NULL)
		return c;

	// if it does not already exist, create it
	c = (ClassExpression*) malloc(sizeof(ClassExpression));
	assert(c != NULL);

	c->type = OBJECT_SOME_VALUES_FROM_TYPE;

	c->description.exists.role = r;
	c->description.exists.filler = f;
	c->id = tbox->next_class_expression_id++;

	LIST_INIT(&(c->told_subsumers));

	SET_INIT(&(c->subsumers), DEFAULT_SUBSUMERS_HASH_SIZE);

	c->filler_of_negative_exists = NULL;

	// initialize the 2-dim dynamic predecessors array
	c->predecessors = NULL;
	c->predecessor_r_count = 0;

	c->successors = NULL;
	c->successor_r_count = 0;

	LIST_INIT(&(c->first_conjunct_of_list));
	c->first_conjunct_of = NULL;

	LIST_INIT(&(c->second_conjunct_of_list));
	c->second_conjunct_of = NULL;

	PUT_EXISTS_RESTRICTION(r->id, f->id, c, tbox);

	return c;
}

ClassExpression* get_create_conjunction_binary(ClassExpression* c1, ClassExpression* c2, TBox* tbox) {
	ClassExpression* c;

	++tbox->binary_object_intersection_of_exps_count;
	if ((c = GET_CONJUNCTION(c1, c2, tbox)) != NULL)
		return c;

	c = (ClassExpression*) malloc(sizeof(ClassExpression));
	assert(c != NULL);

	c->type = OBJECT_INTERSECTION_OF_TYPE;

	// we order the conjuncts!
	if (c1->id < c2->id) {
		c->description.conj.conjunct1 = c1;
		c->description.conj.conjunct2 = c2;
	}
	else {
		c->description.conj.conjunct1 = c2;
		c->description.conj.conjunct2 = c1;
	}
	c->id = tbox->next_class_expression_id++;

	LIST_INIT(&(c->told_subsumers));

	SET_INIT(&(c->subsumers), DEFAULT_SUBSUMERS_HASH_SIZE);

	c->filler_of_negative_exists = NULL;

	// initialize the 2-dim dynamic predecessors array
	c->predecessors = NULL;
	c->predecessor_r_count = 0;

	c->successors = NULL;
	c->successor_r_count = 0;

	LIST_INIT(&(c->first_conjunct_of_list));
	c->first_conjunct_of = NULL;

	LIST_INIT(&(c->second_conjunct_of_list));
	c->second_conjunct_of = NULL;

	PUT_CONJUNCTION(c, tbox);

	return c;
}

// comparison function for qsorting conjucts based on ids
static int compare_conjunct(const void* c1, const void* c2) {
	if (((ClassExpression*) c1)->id < ((ClassExpression*) c2)->id)
		return -1;
	if (((ClassExpression*) c1)->id == ((ClassExpression*) c2)->id)
		return 0;
	return 1;
}

// returns a binary conjunction from a given list of conjuncts.
// called by the parser.
ClassExpression* get_create_conjunction(int size, ClassExpression** conjuncts, TBox* tbox) {
	int i;
	ClassExpression* conjunction = NULL;

	++tbox->object_intersection_of_exps_count;
	qsort(conjuncts, size, sizeof(ClassExpression*), compare_conjunct);
	conjunction = get_create_conjunction_binary(conjuncts[0], conjuncts[1], tbox);
	for (i = 2; i < size; i++)
		conjunction = get_create_conjunction_binary(conjunction, conjuncts[i], tbox);

	return conjunction;
}

// get or create the nominal for a given individual
ClassExpression* get_create_nominal(Individual* ind, TBox* tbox) {
	ClassExpression* c;

	// check if the nominal with this individual already exists
	if ((c = GET_NOMINAL(ind, tbox)) != NULL)
		return c;

	// if a nominal with the individual does not already exist, create it
	c = (ClassExpression*) malloc(sizeof(ClassExpression));
	assert(c != NULL);

	c->description.nominal.individual = ind;

	c->type = OBJECT_ONE_OF_TYPE;
	c->id = tbox->next_class_expression_id++;

	LIST_INIT(&(c->told_subsumers));

	SET_INIT(&(c->subsumers), DEFAULT_SUBSUMERS_HASH_SIZE);

	c->filler_of_negative_exists = NULL;

	// initialize the 2-dim dynamic predecessors array
	c->predecessors = NULL;
	c->predecessor_r_count = 0;

	c->successors = NULL;
	c->successor_r_count = 0;

	LIST_INIT(&(c->first_conjunct_of_list));
	c->first_conjunct_of = NULL;

	LIST_INIT(&(c->second_conjunct_of_list));
	c->second_conjunct_of = NULL;

	PUT_NOMINAL(c, tbox);

	return c;
}
/******************************************************************************
 * get/create functions for roles
 *****************************************************************************/

ObjectPropertyExpression* get_create_atomic_role(char* IRI, TBox* tbox) {
	ObjectPropertyExpression* r;

	// check if the atomic role already exists
	if ((r = GET_ATOMIC_ROLE(IRI, tbox)) != NULL)
		return r;

	// if it does not already exist, create it
	r = (ObjectPropertyExpression*) malloc(sizeof(ObjectPropertyExpression));
	assert(r != NULL);

	r->type = OBJECT_PROPERTY_TYPE;
	r->description.atomic.IRI = (char*) malloc((strlen(IRI) + 1) * sizeof(char));
	assert(r->description.atomic.IRI != NULL);
	strcpy(r->description.atomic.IRI, IRI);
	r->id = tbox->next_objectproperty_expression_id++;

	SET_INIT(&(r->told_subsumers), DEFAULT_ROLE_TOLD_SUBSUMERS_HASH_SIZE);

	LIST_INIT(&(r->subsumer_list));
	SET_INIT(&(r->subsumers), DEFAULT_ROLE_SUBSUMERS_HASH_SIZE);

	SET_INIT(&(r->subsumees), DEFAULT_ROLE_SUBSUMEES_HASH_SIZE);

	r->first_component_of_count = 0;
	r->first_component_of_list = NULL;
	SET_INIT(&(r->first_component_of), DEFAULT_ROLE_FIRST_COMPONENT_OF_HASH_SIZE);

	r->second_component_of_count = 0;
	r->second_component_of_list = NULL;
	SET_INIT(&(r->second_component_of), DEFAULT_ROLE_SECOND_COMPONENT_OF_HASH_SIZE);

	PUT_ATOMIC_ROLE(r->description.atomic.IRI, r, tbox);

	return r;
}



ObjectPropertyExpression* get_create_role_composition_binary(ObjectPropertyExpression *r1, ObjectPropertyExpression* r2, TBox* tbox) {
	ObjectPropertyExpression* r;

	tbox->binary_role_composition_count++;
	if ((r = GET_ROLE_COMPOSITION(r1, r2, tbox)) != NULL)
		return r;

	r = (ObjectPropertyExpression*) malloc(sizeof(ObjectPropertyExpression));
	assert(r != NULL);

	r->type = OBJECT_PROPERTY_CHAIN_TYPE;
	// we DO assume role1 and role2 to be ordered!
	r->description.object_property_chain.role1 = r1;
	r->description.object_property_chain.role2 = r2;
	r->id = tbox->next_objectproperty_expression_id++;

	SET_INIT(&(r->told_subsumers), DEFAULT_ROLE_TOLD_SUBSUMERS_HASH_SIZE);

	LIST_INIT(&(r->subsumer_list));
	SET_INIT(&(r->subsumers), DEFAULT_ROLE_SUBSUMERS_HASH_SIZE);

	SET_INIT(&(r->subsumees), DEFAULT_ROLE_SUBSUMEES_HASH_SIZE);

	r->first_component_of_count = 0;
	r->first_component_of_list = NULL;
	SET_INIT(&(r->first_component_of), DEFAULT_ROLE_FIRST_COMPONENT_OF_HASH_SIZE);

	r->second_component_of_count = 0;
	r->second_component_of_list = NULL;
	SET_INIT(&(r->second_component_of), DEFAULT_ROLE_SECOND_COMPONENT_OF_HASH_SIZE);

	PUT_ROLE_COMPOSITION(r, tbox);

	return r;
}

// get or create the role composition consisting of the given roles.
// called by the parser.
ObjectPropertyExpression* get_create_role_composition(int size, ObjectPropertyExpression** roles, TBox* tbox) {
	int i;
	ObjectPropertyExpression* composition = NULL;

	tbox->role_composition_count++;
	composition = get_create_role_composition_binary(roles[size-1], roles[size-2], tbox);
	for (i = size - 3 ; i >= 0 ; --i)
		composition = get_create_role_composition_binary(composition, roles[i], tbox);

	return composition;
}


/******************************************************************************
 * create functions for axioms
 *****************************************************************************/
// create the subclass axiom with the given concept descriptions
SubClassOfAxiom* create_subclass_axiom(ClassExpression* lhs, ClassExpression* rhs) {
	SubClassOfAxiom* ax = (SubClassOfAxiom*) malloc(sizeof(SubClassOfAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

// create the equivalent classes axiom with the given concept descriptions
EquivalentClassesAxiom* create_eqclass_axiom(ClassExpression* lhs, ClassExpression* rhs) {
	/*
	if (lhs->type != ATOMIC_CONCEPT) {
		fprintf(stderr,"the lhs of an equivalent classes axiom must be an atomic concept, aborting\n");
		exit(EXIT_FAILURE);
	}
	*/
	EquivalentClassesAxiom* ax = (EquivalentClassesAxiom*) malloc(sizeof(EquivalentClassesAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

// create the disjointclasses axiom with the given concept descriptions
DisjointClassesAxiom* create_disjointclasses_axiom(int class_count, ClassExpression** classes) {
	DisjointClassesAxiom* ax = (DisjointClassesAxiom*) calloc(1, sizeof(DisjointClassesAxiom));
	assert(ax != NULL);
	LIST_INIT(&(ax->classes));
	int i;
	for (i = 0; i < class_count; ++i)
		list_add(classes[i], &(ax->classes));

	return ax;
}

// create the subrole axiom with the given role descriptions
SubObjectPropertyOfAxiom* create_subrole_axiom(ObjectPropertyExpression* lhs, ObjectPropertyExpression* rhs) {
	SubObjectPropertyOfAxiom* ax = (SubObjectPropertyOfAxiom*) malloc(sizeof(SubObjectPropertyOfAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

TransitiveObjectPropertyAxiom* create_transitive_role_axiom(ObjectPropertyExpression* r) {
	TransitiveObjectPropertyAxiom* ax = (TransitiveObjectPropertyAxiom*) malloc(sizeof(TransitiveObjectPropertyAxiom));
	assert(ax != NULL);
	ax->r = r;
	return ax;
}

// create the eqrole axiom with the given role descriptions
EquivalentObjectPropertiesAxiom* create_eqrole_axiom(ObjectPropertyExpression* lhs, ObjectPropertyExpression* rhs) {
	EquivalentObjectPropertiesAxiom* ax = (EquivalentObjectPropertiesAxiom*) malloc(sizeof(EquivalentObjectPropertiesAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

// create an objectproperty domain axiom with the given objectproperty and class expressions
ObjectPropertyDomainAxiom* create_objectproperty_domain_axiom(ObjectPropertyExpression* ope, ClassExpression* ce) {
	ObjectPropertyDomainAxiom* ax = (ObjectPropertyDomainAxiom*) malloc(sizeof(ObjectPropertyDomainAxiom));
	assert(ax != NULL);
	ax->object_property_expression = ope;
	ax->class_expression = ce;
	return ax;
}

// create a SameIndividual axiom with the given list of individuals
SameIndividualAxiom* create_same_individual_axiom(int individual_count, Individual** individuals) {
	SameIndividualAxiom* ax = (SameIndividualAxiom*) calloc(1, sizeof(SameIndividualAxiom));
	assert(ax != NULL);
	LIST_INIT(&(ax->individuals));
	int i;
	for (i = 0; i < individual_count; ++i)
		list_add(individuals[i], &(ax->individuals));

	return ax;
}

// create a DifferentIndividuals axiom with the given list of individuals
DifferentIndividualsAxiom* create_different_individuals_axiom(int individual_count, Individual** individuals) {
	DifferentIndividualsAxiom* ax = (DifferentIndividualsAxiom*) calloc(1, sizeof(DifferentIndividualsAxiom));
	assert(ax != NULL);
	LIST_INIT(&(ax->individuals));
	int i;
	for (i = 0; i < individual_count; ++i)
		list_add(individuals[i], &(ax->individuals));

	return ax;
}
/******************************************************************************
 * get/create functions for ABox
 *****************************************************************************/

Individual* get_create_individual(char* name, ABox* abox) {
	Individual* i;

	// check if an individual with this name already exists in the ABox
	if ((i = (Individual*) GET_INDIVIDUAL(name, abox)) != NULL)
		return i;

	// if an individual with the name does not already exist, create it
	i = (Individual*) malloc(sizeof(Individual));
	assert(i != NULL);

	i->id = abox->last_individual_id++;

	i->IRI = (char*) malloc((strlen(name) + 1) * sizeof(char));
	assert(i->IRI != NULL);
	strcpy(i->IRI, name);

	PUT_INDIVIDUAL(i->IRI, i, abox);

	++abox->individual_count;

	return i;
}

/******************************************************************************
 * create functions for assertions
 *****************************************************************************/
// create the concept assertion with the given individual and concept
ClassAssertion* create_concept_assertion(Individual* ind, ClassExpression* c) {
	ClassAssertion* as = (ClassAssertion*) malloc(sizeof(ClassAssertion));
	assert(as != NULL);
	as->individual = ind;
	as->concept = c;

	return as;
}

// create the role assertion with the given role and individuals
ObjectPropertyAssertion* create_role_assertion(ObjectPropertyExpression* role, Individual* source_ind, Individual* target_ind) {
	ObjectPropertyAssertion* as = (ObjectPropertyAssertion*) malloc(sizeof(ObjectPropertyAssertion));
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
void add_concept_assertion(ClassAssertion* as, ABox* abox) {
	ClassAssertion** tmp;
	tmp = realloc(abox->concept_assertions, (abox->concept_assertion_count + 1) * sizeof(ClassAssertion*));
	assert(tmp != NULL);
	abox->concept_assertions = tmp;
	abox->concept_assertions[abox->concept_assertion_count] = as;
	++abox->concept_assertion_count;
}

// add a given role assertion to a given ABox
void add_role_assertion(ObjectPropertyAssertion* as, ABox* abox) {
	ObjectPropertyAssertion** tmp;
	tmp = realloc(abox->role_assertions, (abox->role_assertion_count + 1) * sizeof(ObjectPropertyAssertion*));
	assert(tmp != NULL);
	abox->role_assertions = tmp;
	abox->role_assertions[abox->role_assertion_count] = as;
	++abox->role_assertion_count;
}
