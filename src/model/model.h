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


#ifndef MODEL_H_
#define MODEL_H_

#include "datatypes.h"

// Names of the top and bottom concepts
// Concepts created in init_reasoner (reasoner.c)
#define OWL_THING	"owl:Thing"
#define OWL_NOTHING	"owl:Nothing"

/******************************************************************************
 * get/create functions for concepts
 *****************************************************************************/

// returns the atomic concept with given name or creates if it does not
// already exist
Concept* get_create_atomic_concept(char* name, TBox* tbox);

// get or create the existential restriction with role r and filler f
Concept* get_create_exists_restriction(Role* r, Concept* f, TBox* tbox);

// get or create the existential restriction with the given conjuncts
Concept* get_create_conjunction(int conjunct_count, Concept** conjuncts, TBox* tbox);

// get or create the binary existential restriction with the given conjuncts
Concept* get_create_conjunction_binary(Concept* conjunct1, Concept* conjunct2, TBox* tbox);

// get or create the nominal for a given individual
Concept* get_create_nominal(Individual* ind, TBox* tbox);

/******************************************************************************
 * get/create functions for roles
 *****************************************************************************/
// returns the atomic role with given name or creates if it does not
// already exist
Role* get_create_atomic_role(char* name, TBox* tbox);

// get or create the role composition consisting of the given roles.
// called by the parser.
Role* get_create_role_composition(int size, Role** roles, TBox* tbox);

Role* get_create_role_composition_binary(Role *r1, Role* r2, TBox* tbox);
/******************************************************************************
 * create functions for axioms
 *****************************************************************************/
// create the subclass axiom with the given concept descriptions
SubClassAxiom* create_subclass_axiom(Concept* lhs, Concept* rhs);

// create the equivalent class axiom with the given concept descriptions
EqClassAxiom* create_eqclass_axiom(Concept* lhs, Concept* rhs);

// create disjoint classes axiom
DisjointClassesAxiom* create_disjointclasses_axiom(int class_count, Concept** classes);

// create the subrole axiom with the given role descriptions
SubRoleAxiom* create_subrole_axiom(Role* lhs, Role* rhs);

// create transitive role axiom
TransitiveRoleAxiom* create_transitive_role_axiom(Role* r);

// create the eqrole axiom with the given role descriptions
EqRoleAxiom* create_eqrole_axiom(Role* lhs, Role* rhs);

/******************************************************************************
 * add functions for axioms
*****************************************************************************/
// add a given subclass axiom to the TBox
void add_subclass_axiom(SubClassAxiom* ax, TBox* tbox);

// add a given equivalent classes axiom to the TBox
void add_eqclass_axiom(EqClassAxiom* ax, TBox* tbox);

// add a given disjoint classes axiom to the TBox
void add_disjointclasses_axiom(DisjointClassesAxiom* ax, TBox* tbox);

// add a given subrole axiom to the TBox
void add_subrole_axiom(SubRoleAxiom* ax, TBox* tbox);

// add a given transitive role axiom to the TBox
void add_transitive_role_axiom(TransitiveRoleAxiom* ax, TBox* tbox);

// add a given eqrole axiom to a given TBox
void add_eqrole_axiom(EqRoleAxiom* ax, TBox* tbox);
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
ConceptAssertion* create_concept_assertion(Individual* individual, Concept* concept);

// create the role assertion with the given role and individuals
RoleAssertion* create_role_assertion(Role* role, Individual* source_ind, Individual* target_ind);

/******************************************************************************
 * Add functions for ABox assertions
*****************************************************************************/
// add a given concept assertion to the ABox
void add_concept_assertion(ConceptAssertion* as, ABox* abox);

// add a given role assertion to a given ABox
void add_role_assertion(RoleAssertion* as, ABox* abox);

#endif
