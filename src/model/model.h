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


#ifndef MODEL_H_
#define MODEL_H_

#include "datatypes.h"

// Names of the top and bottom concepts
// Concepts created in init_reasoner (reasoner.c)
#define OWL_THING	"owl:Thing"
#define OWL_NOTHING	"owl:Nothing"

// create ontology prefix if it does not already exist
void create_prefix(char* prefix_name, char* prefix, KB* kb);

/******************************************************************************
 * get/create functions for concepts
 *****************************************************************************/

// returns the atomic concept with given name or creates if it does not
// already exist
ClassExpression* get_create_atomic_concept(char* name, TBox* tbox);

// get or create the existential restriction with role r and filler f
ClassExpression* get_create_exists_restriction(ObjectPropertyExpression* r, ClassExpression* f, TBox* tbox);

// get or create the existential restriction with the given conjuncts
ClassExpression* get_create_conjunction(int conjunct_count, ClassExpression** conjuncts, TBox* tbox);

// get or create the binary existential restriction with the given conjuncts
ClassExpression* get_create_conjunction_binary(ClassExpression* conjunct1, ClassExpression* conjunct2, TBox* tbox);

// get or create the nominal for a given individual
ClassExpression* get_create_nominal(Individual* ind, TBox* tbox);

/******************************************************************************
 * get/create functions for roles
 *****************************************************************************/
// returns the atomic role with given name or creates if it does not
// already exist
ObjectPropertyExpression* get_create_atomic_role(char* name, TBox* tbox);

// get or create the role composition consisting of the given roles.
// called by the parser.
ObjectPropertyExpression* get_create_role_composition(int size, ObjectPropertyExpression** roles, TBox* tbox);

ObjectPropertyExpression* get_create_role_composition_binary(ObjectPropertyExpression *r1, ObjectPropertyExpression* r2, TBox* tbox);
/******************************************************************************
 * create functions for axioms
 *****************************************************************************/
// create the subclass axiom with the given concept descriptions
SubClassOfAxiom* create_subclass_axiom(ClassExpression* lhs, ClassExpression* rhs);

// create the equivalent class axiom with the given concept descriptions
EquivalentClassesAxiom* create_eqclass_axiom(ClassExpression* lhs, ClassExpression* rhs);

// create disjoint classes axiom
DisjointClassesAxiom* create_disjointclasses_axiom(int class_count, ClassExpression** classes);

// create the subrole axiom with the given role descriptions
SubObjectPropertyOfAxiom* create_subrole_axiom(ObjectPropertyExpression* lhs, ObjectPropertyExpression* rhs);

// create transitive role axiom
TransitiveObjectPropertyAxiom* create_transitive_role_axiom(ObjectPropertyExpression* r);

// create the eqrole axiom with the given role descriptions
EquivalentObjectPropertiesAxiom* create_eqrole_axiom(ObjectPropertyExpression* lhs, ObjectPropertyExpression* rhs);

// create an objectproperty domain axiom with the given objectproperty and class expressions
ObjectPropertyDomainAxiom* create_objectproperty_domain_axiom(ObjectPropertyExpression* ope, ClassExpression* ce);

// create a SameIndividual axiom with the given list of individuals
SameIndividualAxiom* create_same_individual_axiom(int individual_count, Individual** individuals);

// create a DifferentIndividuals axiom with the given list of individuals
DifferentIndividualsAxiom* create_different_individuals_axiom(int individual_count, Individual** individuals);
/******************************************************************************
 * add functions for axioms
*****************************************************************************/
// add a given subclass axiom to a given ontology
#define ADD_SUBCLASS_OF_AXIOM(ax, tbox)						SET_ADD(ax, &(tbox->subclass_of_axioms))

// add a given equivalent classes axiom to a given ontology
#define ADD_EQUIVALENT_CLASSES_AXIOM(ax, tbox)				SET_ADD(ax, &(tbox->equivalent_classes_axioms))

// add a given disjoint classes axiom to the TBox
#define ADD_DISJOINT_CLASSES_AXIOM(ax, tbox)				SET_ADD(ax, &(tbox->disjoint_classes_axioms))

// add a given subobjectpropertyof axiom to a given ontology
#define ADD_SUBOBJECTPROPERTY_OF_AXIOM(ax, tbox)			SET_ADD(ax, &(tbox->subobjectproperty_of_axioms))

// add a given transitive role axiom to a given ontology
#define ADD_TRANSITIVE_OBJECTPROPERTY_AXIOM(ax, tbox)		SET_ADD(ax, &(tbox->transitive_objectproperty_axioms))

// add a given equivalent object properties axiom to a given ontology
#define ADD_EQUIVALENT_OBJECTPROPERTIES_AXIOM(ax, tbox)		SET_ADD(ax, &(tbox->equivalent_objectproperties_axioms))

// add a given object property domain axiom to a given ontology
#define ADD_OBJECTPROPERTY_DOMAIN_AXIOM(ax, tbox)			SET_ADD(ax, &(tbox->objectproperty_domain_axioms))

// add a given same individual axiom to a given ontology
#define ADD_SAME_INDIVIDUAL_AXIOM(ax, tbox)					SET_ADD(ax, &(tbox->same_individual_axioms))

// add a given different individuals axiom to a given ontology
#define ADD_DIFFERENT_INDIVIDUALS_AXIOM(ax, tbox)			SET_ADD(ax, &(tbox->different_individuals_axioms))
/******************************************************************************
 * Get/create functions for ABox
 *****************************************************************************/

// returns the individual with given name or creates if it does not
// already exist
Individual* get_create_individual(char* name, ABox* abox);

/******************************************************************************
 * Create functions for ABox assertions
 *****************************************************************************/
// create the concept assertion axiom with the given concept description and individual
ClassAssertion* create_concept_assertion(Individual* individual, ClassExpression* concept);

// create the role assertion with the given role and individuals
ObjectPropertyAssertion* create_role_assertion(ObjectPropertyExpression* role, Individual* source_ind, Individual* target_ind);

/******************************************************************************
 * Add functions for ABox assertions
*****************************************************************************/
// add a given concept assertion to the ABox
void add_concept_assertion(ClassAssertion* as, ABox* abox);

// add a given role assertion to a given ABox
void add_role_assertion(ObjectPropertyAssertion* as, ABox* abox);

#endif
