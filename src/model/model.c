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
#include "../utils/list_64.h"
#include "datatypes.h"
#include "model.h"
#include "utils.h"
#include "limits.h"

// create ontology prefix if it does not already exist
void create_prefix(char* prefix_name, char* prefix, KB* kb) {
	if (GET_ONTOLOGY_PREFIX(prefix_name, kb) != NULL)
		return;

	list_add_64(prefix_name, &(kb->prefix_names));
	PUT_ONTOLOGY_PREFIX(prefix_name, prefix, kb);
}

ClassExpressionId create_class_expression_template(TBox* tbox) {
	// first increment the last id
	++tbox->last_class_expression_id;

	if (tbox->last_class_expression_id == (tbox->class_expressions_size -1)) {
		ClassExpression* tmp = realloc(tbox->class_expressions, (tbox->class_expressions_size + DEFAULT_CLASS_EXPRESSION_COUNT_INCREMENT) * sizeof(ClassExpression));
		assert (tmp != NULL);
		tbox->class_expressions = tmp;
		tbox->class_expressions_size += DEFAULT_CLASS_EXPRESSION_COUNT_INCREMENT;
	}

	// now fill this class expression
	ClassExpression* c = &(tbox->class_expressions[tbox->last_class_expression_id]);

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

	// return the id of this class expression
	return(tbox->last_class_expression_id);

}
/******************************************************************************
 * get/create functions for concepts
 *****************************************************************************/

ClassExpressionId get_create_atomic_concept(char* IRI, TBox* tbox) {
	ClassExpressionId id;

	// check if the atomic concept with this name already exists
	if ((id = GET_ATOMIC_CONCEPT(IRI, tbox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// a class with the IRI not found, create it
	// first create a class expression template
	id  = create_class_expression_template(tbox);

	// fill the class expression
	tbox->class_expressions[id].type = CLASS_TYPE;

	tbox->class_expressions[id].description.atomic.IRI = (char*) malloc((strlen(IRI) + 1) * sizeof(char));
	assert(tbox->class_expressions[id].description.atomic.IRI != NULL);
	strcpy(tbox->class_expressions[id].description.atomic.IRI, IRI);

	SET_INIT(&(tbox->class_expressions[id].description.atomic.direct_subsumers), DEFAULT_DIRECT_SUBSUMERS_SET_SIZE);

	SET_INIT(&(tbox->class_expressions[id].description.atomic.equivalent_classes), DEFAULT_EQUIVALENT_CONCEPTS_SET_SIZE);

	PUT_ATOMIC_CONCEPT(tbox->class_expressions[id].description.atomic.IRI, id, tbox);

	return(id);
}

// get or create the existential restriction with role r and filler f
ClassExpressionId get_create_exists_restriction(ObjectPropertyExpressionId r, ClassExpressionId f, TBox* tbox) {
	ClassExpressionId id;

	++tbox->object_some_values_from_exps_count;

	// first check if we already created an existential
	// restriction with the same role and filler
	if ((id = GET_EXISTS_RESTRICTION(r, f, tbox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// if it does not already exist, create it
	// first create a class expression template
	id  = create_class_expression_template(tbox);

	// fill the class expression
	tbox->class_expressions[id].type = OBJECT_SOME_VALUES_FROM_TYPE;

	tbox->class_expressions[id].description.exists.role = r;
	tbox->class_expressions[id].description.exists.filler = f;

	PUT_EXISTS_RESTRICTION(r, f, id, tbox);

	return(id);
}

ClassExpressionId get_create_conjunction_binary(ClassExpressionId c1, ClassExpressionId c2, TBox* tbox) {
	ClassExpressionId id;

	++tbox->binary_object_intersection_of_exps_count;
	if ((id = GET_CONJUNCTION(c1, c2, tbox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// if it does not already exist, create it
	// first create a class expression template
	id  = create_class_expression_template(tbox);

	// fill the class expression
	tbox->class_expressions[id].type = OBJECT_INTERSECTION_OF_TYPE;

	// we order the conjuncts!
	if (c1 < c2) {
		tbox->class_expressions[id].description.conj.conjunct1 = c1;
		tbox->class_expressions[id].description.conj.conjunct2 = c2;
	}
	else {
		tbox->class_expressions[id].description.conj.conjunct1 = c2;
		tbox->class_expressions[id].description.conj.conjunct2 = c1;
	}

	PUT_CONJUNCTION(c1, c2, id, tbox);

	return(id);
}

// comparison function for qsorting conjucts based on ids
 static int compare_conjunct(const void* c1, const void* c2) {
	if (c1 <  c2)
		return -1;
	if ( c1 == c2)
		return 0;
	return 1;
}

// returns a binary conjunction from a given list of conjuncts.
// called by the parser.
ClassExpressionId get_create_conjunction(int size, ClassExpressionId* conjuncts, TBox* tbox) {
	int i;
	// TODO: Does this 0 have a special meaning/reason?
	ClassExpressionId conjunction = 0;

	++tbox->object_intersection_of_exps_count;
	qsort(conjuncts, size, sizeof(ClassExpressionId), compare_conjunct);
	conjunction = get_create_conjunction_binary(conjuncts[0], conjuncts[1], tbox);
	for (i = 2; i < size; i++)
		conjunction = get_create_conjunction_binary(conjunction, conjuncts[i], tbox);

	return conjunction;
}

// get or create the nominal for a given individual
ClassExpressionId get_create_nominal(IndividualId ind, TBox* tbox) {
	ClassExpressionId id;

	// check if the nominal with this individual already exists
	if ((id = GET_NOMINAL(ind, tbox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// if a nominal with the individual does not already exist, create it
	id  = create_class_expression_template(tbox);

	tbox->class_expressions[id].type = OBJECT_ONE_OF_TYPE;
	tbox->class_expressions[id].description.nominal.individual = ind;

	PUT_NOMINAL(ind, id, tbox);

	return(id);
}


/******************************************************************************
 * get/create functions for roles
 *****************************************************************************/

ObjectPropertyExpressionId create_object_property_expression_template(TBox* tbox) {
	// first increment the last id
	++tbox->last_object_property_expression_id;

	if (tbox->last_object_property_expression_id >= tbox->object_property_expressions_size - 1) {
		ObjectPropertyExpression* tmp = realloc(tbox->object_property_expressions, tbox->object_property_expressions_size + DEFAULT_OBJECT_PROPERTY_EXPRESSION_COUNT_INCREMENT);
		assert (tmp != NULL);
		tbox->object_property_expressions = tmp;
	}

	// now fill this object property expression
	ObjectPropertyExpression* r = &(tbox->object_property_expressions[tbox->last_object_property_expression_id]);

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

	// return the id of this object property expression
	return(tbox->last_object_property_expression_id);

}
ObjectPropertyExpressionId get_create_atomic_role(char* IRI, TBox* tbox) {
	ObjectPropertyExpressionId id;

	// check if the atomic role already exists
	if ((id = GET_ATOMIC_ROLE(IRI, tbox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// if it does not already exist, create it
	id = create_object_property_expression_template(tbox);

	tbox->object_property_expressions[id].type = OBJECT_PROPERTY_TYPE;

	tbox->object_property_expressions[id].description.atomic.IRI = (char*) malloc((strlen(IRI) + 1) * sizeof(char));
	assert(tbox->object_property_expressions[id].description.atomic.IRI != NULL);
	strcpy(tbox->object_property_expressions[id].description.atomic.IRI, IRI);

	PUT_ATOMIC_ROLE(tbox->object_property_expressions[id].description.atomic.IRI, id, tbox);

	return(id);
}


ObjectPropertyExpressionId get_create_role_composition_binary(ObjectPropertyExpressionId r1, ObjectPropertyExpressionId r2, TBox* tbox) {
	ObjectPropertyExpressionId id;

	tbox->binary_role_composition_count++;
	if ((id = GET_ROLE_COMPOSITION(r1, r2, tbox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// if it does not already exist, create it
	id = create_object_property_expression_template(tbox);

	tbox->object_property_expressions[id].type = OBJECT_PROPERTY_CHAIN_TYPE;

	// we DO assume role1 and role2 to be ordered!
	// TODO: is it required that they are ordered? As it is, there is no sorting!
	tbox->object_property_expressions[id].description.object_property_chain.role1 = r1;
	tbox->object_property_expressions[id].description.object_property_chain.role2 = r2;

	PUT_ROLE_COMPOSITION(r1, r2, id, tbox);

	return(id);
}

// get or create the role composition consisting of the given roles.
// called by the parser.
ObjectPropertyExpressionId get_create_role_composition(int size, ObjectPropertyExpressionId* roles, TBox* tbox) {
	int i;
	ObjectPropertyExpressionId composition = -1;

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
SubClassOfAxiom* create_subclass_axiom(ClassExpressionId lhs, ClassExpressionId rhs) {
	SubClassOfAxiom* ax = (SubClassOfAxiom*) malloc(sizeof(SubClassOfAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

// create the equivalent classes axiom with the given concept descriptions
EquivalentClassesAxiom* create_eqclass_axiom(ClassExpressionId lhs, ClassExpressionId rhs) {
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
DisjointClassesAxiom* create_disjointclasses_axiom(int class_count, ClassExpressionId* classes) {
	DisjointClassesAxiom* ax = (DisjointClassesAxiom*) calloc(1, sizeof(DisjointClassesAxiom));
	assert(ax != NULL);
	LIST_INIT(&(ax->classes));
	int i;
	for (i = 0; i < class_count; ++i)
		list_add(classes[i], &(ax->classes));

	return ax;
}

// create the subrole axiom with the given role descriptions
SubObjectPropertyOfAxiom* create_subrole_axiom(ObjectPropertyExpressionId lhs, ObjectPropertyExpressionId rhs) {
	SubObjectPropertyOfAxiom* ax = (SubObjectPropertyOfAxiom*) malloc(sizeof(SubObjectPropertyOfAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

TransitiveObjectPropertyAxiom* create_transitive_role_axiom(ObjectPropertyExpressionId r) {
	TransitiveObjectPropertyAxiom* ax = (TransitiveObjectPropertyAxiom*) malloc(sizeof(TransitiveObjectPropertyAxiom));
	assert(ax != NULL);
	ax->r = r;
	return ax;
}

// create the eqrole axiom with the given role descriptions
EquivalentObjectPropertiesAxiom* create_eqrole_axiom(ObjectPropertyExpressionId lhs, ObjectPropertyExpressionId rhs) {
	EquivalentObjectPropertiesAxiom* ax = (EquivalentObjectPropertiesAxiom*) malloc(sizeof(EquivalentObjectPropertiesAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	return ax;
}

// create an objectproperty domain axiom with the given objectproperty and class expressions
ObjectPropertyDomainAxiom* create_objectproperty_domain_axiom(ObjectPropertyExpressionId ope, ClassExpressionId ce) {
	ObjectPropertyDomainAxiom* ax = (ObjectPropertyDomainAxiom*) malloc(sizeof(ObjectPropertyDomainAxiom));
	assert(ax != NULL);
	ax->object_property_expression = ope;
	ax->class_expression = ce;
	return ax;
}

// create a SameIndividual axiom with the given list of individuals
SameIndividualAxiom* create_same_individual_axiom(int individual_count, IndividualId* individuals) {
	SameIndividualAxiom* ax = (SameIndividualAxiom*) calloc(1, sizeof(SameIndividualAxiom));
	assert(ax != NULL);
	LIST_INIT(&(ax->individuals));
	int i;
	for (i = 0; i < individual_count; ++i)
		list_add(individuals[i], &(ax->individuals));

	return ax;
}

// create a DifferentIndividuals axiom with the given list of individuals
DifferentIndividualsAxiom* create_different_individuals_axiom(int individual_count, IndividualId* individuals) {
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

IndividualId get_create_individual(char* name, ABox* abox) {
	IndividualId id;

	// check if an individual with this name already exists in the ABox
	if ((id = GET_INDIVIDUAL(name, abox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// first increment the last id
	id = ++abox->last_individual_id;

	if (abox->last_individual_id >= abox->individual_count) {
		Individual* tmp = realloc(abox->individuals, abox->individual_count + DEFAULT_INDIVIDUAL_COUNT_INCREMENT);
		assert (tmp != NULL);
		abox->individuals = tmp;
		abox->individual_count += DEFAULT_INDIVIDUAL_COUNT_INCREMENT;
	}

	abox->individuals[id].IRI = (char*) malloc((strlen(name) + 1) * sizeof(char));
	assert(abox->individuals[id].IRI != NULL);
	strcpy(abox->individuals[id].IRI, name);

	PUT_INDIVIDUAL(abox->individuals[id].IRI, id, abox);

	++abox->individual_count;

	return(id);
}

/******************************************************************************
 * create functions for assertions
 *****************************************************************************/
// create the concept assertion with the given individual and concept
ClassAssertion* create_concept_assertion(IndividualId ind, ClassExpressionId c) {
	ClassAssertion* as = (ClassAssertion*) malloc(sizeof(ClassAssertion));
	assert(as != NULL);
	as->individual = ind;
	as->concept = c;

	return as;
}

// create the role assertion with the given role and individuals_map
ObjectPropertyAssertion* create_role_assertion(ObjectPropertyExpressionId role, IndividualId source_ind, IndividualId target_ind) {
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
