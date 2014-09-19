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


#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "../utils/set.h"
#include "../utils/list.h"
#include "../hashing/hash_table.h"
#include "../hashing/hash_map.h"


// Class constructors
typedef struct class Class;
typedef struct object_intersection_of ObjectIntersectionOf;
typedef struct object_some_values_from ObjectSomeValuesFrom;
typedef struct object_one_of ObjectOneOf;
typedef union class_description ClassDescription;
typedef struct class_expression ClassExpression;
// Datatype for successor and predecessors
typedef struct link Link;

// Roles, role constructors
typedef struct object_property AtomicRole;
typedef struct object_property_chain RoleComposition;
typedef union object_property_description RoleDescription;
typedef struct object_property_expression Role;

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
	CLASS_TYPE, OBJECT_INTERSECTION_OF_TYPE, OBJECT_SOME_VALUES_FROM_TYPE, OBJECT_ONE_OF_TYPE
};

// Class
struct class {
	char* IRI;

	// List of equivalent classes. Elements are ClassExpression*
	List* equivalent_classes;
	// Set of direct subsumers. Elements are ClassExpression*
	Set* direct_subsumers;
};


// ObjectIntersectionOf
struct object_intersection_of {
	ClassExpression* conjunct1;
	ClassExpression* conjunct2;
};


// ObjectSomeValuesFrom
struct object_some_values_from {
	Role* role;
	ClassExpression* filler;
};

// ObjectOneOf
struct object_one_of {
	Individual* individual;
};

// Class description
union class_description {
	Class* atomic;
	ObjectIntersectionOf* conj;
	ObjectSomeValuesFrom* exists;
	ObjectOneOf* nominal;
}; 
	

// ClassExpression
struct class_expression {
	// Unique id
	// 32-bit unsigned integer. Needed for hashing.
	uint32_t id;

	enum concept_description_type type;
	ClassDescription description;

	// List of told subsumers. Elements are ClassExpression*
	List* told_subsumers;

	// Set of subsumers computed during saturation.
	// Elements are ClassExpression*
	Set* subsumers;

	// 2-dimensional dynamic array for storing predecessors.
	Link** predecessors;
	// Number of roles, for which this concept has a predecessor (the size of predecessors array)
	int predecessor_r_count;

	// the same data structure to store the successors.
	// needed for implementing the role composition rule in saturation
	Link** successors;
	int successor_r_count;

	// list of negative existentials whose filler is this concept
	ClassExpression** filler_of_negative_exists;

	// list of conjunctions where this concept is the first/second conjunct
	ClassExpression** first_conjunct_of_list;
	int first_conjunct_of_count;
	ClassExpression** second_conjunct_of_list;
	int second_conjunct_of_count;

	// same as above. the reason is performance in saturation.
	HashTable* first_conjunct_of;
	HashTable* second_conjunct_of;

};

// For keeping successors and predecessors of a  concept
struct link {
	Role* role;
	ClassExpression** fillers;
	int filler_count;
};

/*****************************************************************************/
// Role description types
enum role_description_type {
	ATOMIC_ROLE, ROLE_COMPOSITION
};

// ObjectProperty
struct object_property {
	char* name;
};

// Object property description
union object_property_description {
	AtomicRole* atomic;
	RoleComposition* role_composition;
}; 
	
// Object property chain
struct object_property_chain {
	Role* role1;
	Role* role2;
};

// Object property expression
struct object_property_expression {
	// Unique role id.
	// 32-bit unsigned integer, needed for hashing.
	uint32_t id;

	enum role_description_type type;
	RoleDescription description;

	HashMap* told_subsumers;
	HashMap* told_subsumees;

	HashTable* subsumers;
	Role** subsumer_list;
	int subsumer_count;

	// Only necessary for optimizing the processing of role compositions
	// For that we need to access the subsumees
	HashTable* subsumees;
	Role** subsumee_list;
	int subsumee_count;

	// List of role compositions where this role is the first/second component
	Role** first_component_of_list;
	int first_component_of_count;
	Role** second_component_of_list;
	int second_component_of_count;

	// Same as above. The reason is performance in saturation.
	HashTable* first_component_of;
	HashTable* second_component_of;
};

/******************************************************************************/
// SubClass axiom
struct subclass_axiom {
	ClassExpression* lhs;
	ClassExpression* rhs;
};

// EquivalentClasses axiom
struct eqclass_axiom {
	ClassExpression* lhs;
	ClassExpression* rhs;
};

// DisjointClasses axiom
struct disjointclasses_axiom {
	int concept_count;
	ClassExpression **concepts;
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
	ClassExpression* top_concept;
	// Bottom concept
	ClassExpression* bottom_concept;

	// Atomic concepts hash
	HashMap* atomic_concepts;
	ClassExpression** atomic_concept_list;
	int atomic_concept_count;

	// Exists restictions hash map
	// The key is role id_filler id
	HashMap* exists_restrictions;
	int exists_restriction_count;
	int unique_exists_restriction_count;

	// Hash for conjunctions
	// Key: id of conjuct1_id of conjunct2
	// where conjuct ids are sorted
	HashMap* conjunctions;
	int conjunction_count;
	int binary_conjunction_count;
	int unique_binary_conjunction_count;

	// Nominals
	HashMap* nominals;

	HashMap* atomic_roles;
	int atomic_role_count;

	HashMap* role_compositions;
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
	ClassExpression* concept;
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

	HashMap* individuals;

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
	// KeyValueHashTable* prefixes;
	int prefix_count;
	char** prefix_names_list;
	char** prefix_list;


	// The list of subclass axioms that result from converting syntactic
	// shortcuts (disjointness axioms, etc.) to subclass axioms. They are generated in preprocessing.
	SubClassAxiom** generated_subclass_axioms;
	int generated_subclass_axiom_count;

	// The list of subrole axioms that are generated during  preprocessing
	SubRoleAxiom** generated_subrole_axioms;
	int generated_subrole_axiom_count;

	// The hash of nominals that are generated during preprocessing.
	HashMap* generated_nominals;

	// The hash of existential restrictions that are generated during preprocessing.
	// They are generated from preprocessing role assertions.
	HashMap* generated_exists_restrictions;
	int generated_exists_restriction_count;
};

#endif
