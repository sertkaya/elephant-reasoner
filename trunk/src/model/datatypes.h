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


#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "../hashing/key_hash_table.h"
#include "../hashing/key_value_hash_table.h"


// Concepts, concept constructors
typedef struct atomic_concept AtomicConcept;
typedef struct conjunction Conjunction;
typedef struct existential_restriction Exists;
typedef struct nominal Nominal;
typedef union concept_description ConceptDescription;
typedef struct concept Concept;
// Datatype for successor and predecessors
typedef struct link Link;

// Roles, role constructors
typedef struct atomic_role AtomicRole;
typedef struct role_composition RoleComposition;
typedef union role_description RoleDescription;
typedef struct role Role;

// Axioms
typedef struct subclass_axiom SubClassAxiom;
typedef struct eqclass_axiom EqClassAxiom;
typedef struct disjointclasses_axiom DisjointClassesAxiom;
typedef struct subrole_axiom SubRoleAxiom;
typedef struct eqrole_axiom EqRoleAxiom;
typedef struct transitive_role_axiom TransitiveRoleAxiom;
// TBox
typedef struct tbox TBox;

// Individual
typedef struct individual Individual;

// ABox assertions
typedef struct concept_assertion ConceptAssertion;
typedef struct role_assertion RoleAssertion;

// ABox
typedef struct abox ABox;

// Knowledge base
typedef struct knowledge_base KB;

// Reasoning task
typedef enum reasoning_task ReasoningTask;
/*****************************************************************************/
// Concept description types
enum concept_description_type {
	ATOMIC_CONCEPT, CONJUNCTION, EXISTENTIAL_RESTRICTION, NOMINAL
};

// Atomic concept
struct atomic_concept {
	char* name;

	Concept** equivalent_concepts_list;
	int equivalent_concepts_count;
	KeyHashTable* direct_subsumers;
	Concept** direct_subsumer_list;
	int direct_subsumer_count;
};


// Conjunction
struct conjunction {
	Concept* conjunct1;
	Concept* conjunct2;
};


// Existential restriction
struct existential_restriction {
	Role* role;
	Concept* filler;
};

// Nominal
struct nominal {
	Individual* individual;
};

// Concept description
union concept_description {
	AtomicConcept* atomic;
	Conjunction* conj;
	Exists* exists;
	Nominal* nominal;
}; 
	

// Concept description
struct concept {
	// Unique concept id.
	// 32-bit unsigned integer, needed for hashing.
	uint32_t id;

	enum concept_description_type type;
	ConceptDescription description;

	Concept** told_subsumers;
	int told_subsumer_count;

	Concept** subsumer_list;
	int subsumer_count;

	KeyHashTable* subsumers;

	// 2-dimensional dynamic array for storing predecessors.
	Link** predecessors;
	// Number of roles, for which this concept has a predecessor (the size of predecessors array)
	int predecessor_r_count;

	// the same data structure to store the successors.
	// needed for implementing the role composition rule in saturation
	Link** successors;
	int successor_r_count;

	// list of negative existentials whose filler is this concept
	Concept** filler_of_negative_exists;

	// list of conjunctions where this concept is the first/second conjunct
	Concept** first_conjunct_of_list;
	int first_conjunct_of_count;
	Concept** second_conjunct_of_list;
	int second_conjunct_of_count;

	// same as above. the reason is performance in saturation.
	KeyHashTable* first_conjunct_of;
	KeyHashTable* second_conjunct_of;

};

// For keeping successors and predecessors of a  concept
struct link {
	Role* role;
	Concept** fillers;
	int filler_count;
};

/*****************************************************************************/
// Role description types
enum role_description_type {
	ATOMIC_ROLE, ROLE_COMPOSITION
};

// Atomic role
struct atomic_role {
	char* name;
};

// Role description
union role_description {
	AtomicRole* atomic;
	RoleComposition* role_composition;
}; 
	
// Role composition
struct role_composition {
	Role* role1;
	Role* role2;
};

// Role
struct role {
	// Unique role id.
	// 32-bit unsigned integer, needed for hashing.
	uint32_t id;

	enum role_description_type type;
	RoleDescription description;

	KeyValueHashTable* told_subsumers;
	KeyValueHashTable* told_subsumees;

	KeyHashTable* subsumers;
	Role** subsumer_list;
	int subsumer_count;

	// Only necessary for optimizing the processing of role compositions
	// For that we need to access the subsumees
	KeyHashTable* subsumees;
	Role** subsumee_list;
	int subsumee_count;

	// List of role compositions where this role is the first/second component
	Role** first_component_of_list;
	int first_component_of_count;
	Role** second_component_of_list;
	int second_component_of_count;

	// Same as above. The reason is performance in saturation.
	KeyHashTable* first_component_of;
	KeyHashTable* second_component_of;
};

/******************************************************************************/
// SubClass axiom
struct subclass_axiom {
	Concept* lhs;
	Concept* rhs;
};

// EquivalentClasses axiom
struct eqclass_axiom {
	Concept* lhs;
	Concept* rhs;
};

// DisjointClasses axiom
struct disjointclasses_axiom {
	int concept_count;
	Concept **concepts;
};

// RI
struct subrole_axiom {
	Role* lhs;
	Role* rhs;
};

// Transitive role
struct transitive_role_axiom {
	Role* r;
};

// Equivalent roles
struct eqrole_axiom {
	Role* lhs;
	Role* rhs;
};

// TBox
struct tbox {
	// Top concept
	Concept* top_concept;
	// Bottom concept
	Concept* bottom_concept;

	// Atomic concepts hash
	KeyValueHashTable* atomic_concepts;
	Concept** atomic_concept_list;
	int atomic_concept_count;

	// Exists restictions hash
	// The key is role id_filler id
	KeyValueHashTable* exists_restrictions;
	int exists_restriction_count;
	int unique_exists_restriction_count;

	// Hash for conjunctions
	// Key: id of conjuct1_id of conjunct2
	// where conjuct ids are sorted
	KeyValueHashTable* conjunctions;
	int conjunction_count;
	int binary_conjunction_count;
	int unique_binary_conjunction_count;

	// Nominals
	KeyValueHashTable* nominals;

	KeyValueHashTable* atomic_roles;
	int atomic_role_count;

	KeyValueHashTable* role_compositions;
	int role_composition_count;
	int binary_role_composition_count;
	int unique_binary_role_composition_count;

	uint32_t last_concept_id;
	uint32_t last_role_id;

	SubClassAxiom** subclass_axioms;
	int subclass_axiom_count;

	EqClassAxiom** eqclass_axioms;
	int eqclass_axiom_count;

	SubRoleAxiom** subrole_axioms;
	int subrole_axiom_count;

	TransitiveRoleAxiom** transitive_role_axioms;
	int transitive_role_axiom_count;

	EqRoleAxiom** eqrole_axioms;
	int eqrole_axiom_count;

	DisjointClassesAxiom** disjointclasses_axioms;
	int disjointclasses_axiom_count;

};

/******************************************************************************/
// Individual
struct individual {
	uint32_t id;
	char* name;
};

// Concept assertion
struct concept_assertion {
	Individual* individual;
	Concept* concept;
};

// Role assertion
struct role_assertion {
	Role* role;
	Individual* source_individual;
	Individual* target_individual;
};

// ABox
struct abox {
	int last_individual_id;

	int individual_count;
	// Individual** individual_list;

	KeyValueHashTable* individuals;

	int concept_assertion_count;
	ConceptAssertion** concept_assertions;

	int role_assertion_count;
	RoleAssertion** role_assertions;
};

/******************************************************************************/
// Reasoning tasks
enum reasoning_task {
	CLASSIFICATION, CONSISTENCY, REALISATION
};

// Knowledge base
struct knowledge_base {
	TBox* tbox;
	ABox* abox;
	// flag for inconsistency
	char inconsistent;
	KeyValueHashTable* prefixes;
};

#endif
