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

/******************************************************************************
 * add functions for axioms
*****************************************************************************/
// add a given subclass axiom to a given ontology
#define ADD_SUBCLASSOF_AXIOM(ax, tbox)				SET_ADD(ax, &(tbox->subclassof_axioms))

// add a given equivalent classes axiom to a given ontology
#define ADD_EQUIVALENTCLASSES_AXIOM(ax, tbox)		SET_ADD(ax, &(tbox->equivalentclasses_axioms))

// add a given disjoint classes axiom to the TBox
void add_disjointclasses_axiom(DisjointClassesAxiom* ax, TBox* tbox);

// add a given subobjectpropertyof axiom to a given ontology
#define ADD_SUBOBJECTPROPERTYOF_AXIOM(ax, tbox)		SET_ADD(ax, &(tbox->subobjectpropertyof_axioms))

// add a given transitive role axiom to the TBox
void add_transitive_role_axiom(TransitiveObjectPropertyAxiom* ax, TBox* tbox);

// add a given eqrole axiom to a given TBox
void add_eqrole_axiom(EquivalentObjectPropertiesAxiom* ax, TBox* tbox);
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
