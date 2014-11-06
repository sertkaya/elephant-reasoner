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
#include "../utils/map.h"
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

// Object property constructors
typedef struct object_property ObjectProperty;
typedef struct object_property_chain ObjectPropertyChain;
typedef union object_property_description ObjectPropertyDescription;
typedef struct object_property_expression ObjectPropertyExpression;

// Axioms
typedef struct subclass_of_axiom SubClassOfAxiom;
typedef struct equivalent_classes_axiom EquivalentClassesAxiom;
typedef struct disjoint_classes_axiom DisjointClassesAxiom;
typedef struct subobject_property_of_axiom SubObjectPropertyAxiom;
typedef struct equivalent_object_properties_axiom EquivalentObjectPropertiesAxiom;
typedef struct transitive_object_property_axiom TransitiveObjectPropertyAxiom;

// TBox
typedef struct tbox TBox;

// Individual
typedef struct individual Individual;

// ABox assertions
typedef struct class_assertion ClassAssertion;
typedef struct object_property_assertion ObjectPropertyAssertion;

// ABox
typedef struct abox ABox;

// Knowledge base
typedef struct knowledge_base KB;

// Reasoning task
typedef enum reasoning_task ReasoningTask;
/*****************************************************************************/
// Concept description types
enum class_expression_type {
	CLASS_TYPE, OBJECT_INTERSECTION_OF_TYPE, OBJECT_SOME_VALUES_FROM_TYPE, OBJECT_ONE_OF_TYPE
};

// Class
struct class {
	char* IRI;

	// Set of equivalent classes. Elements are ClassExpression*
	Set equivalent_classes;
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
	ObjectPropertyExpression* role;
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

	enum class_expression_type type;
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
	ObjectPropertyExpression* role;
	ClassExpression** fillers;
	int filler_count;
};

/*****************************************************************************/
// Role description types
enum object_property_expression_type {
	OBJECT_PROPERTY_TYPE, OBJECT_PROPERTY_CHAIN_TYPE
};

// ObjectProperty
struct object_property {
	char* IRI;
};

// Object property description
union object_property_description {
	ObjectProperty* atomic;
	ObjectPropertyChain* role_composition;
}; 
	
// Object property chain
struct object_property_chain {
	ObjectPropertyExpression* role1;
	ObjectPropertyExpression* role2;
};

// Object property expression
struct object_property_expression {
	// Unique role id.
	// 32-bit unsigned integer, needed for hashing.
	uint32_t id;

	enum object_property_expression_type type;
	ObjectPropertyDescription description;

	HashMap* told_subsumers;
	HashMap* told_subsumees;

	HashTable* subsumers;
	ObjectPropertyExpression** subsumer_list;
	int subsumer_count;

	// Only necessary for optimizing the processing of role compositions
	// For that we need to access the subsumees
	HashTable* subsumees;
	ObjectPropertyExpression** subsumee_list;
	int subsumee_count;

	// List of role compositions where this role is the first/second component
	ObjectPropertyExpression** first_component_of_list;
	int first_component_of_count;
	ObjectPropertyExpression** second_component_of_list;
	int second_component_of_count;

	// Same as above. The reason is performance in saturation.
	HashTable* first_component_of;
	HashTable* second_component_of;
};

/******************************************************************************/
// SubClassOf axiom
struct subclass_of_axiom {
	ClassExpression* lhs;
	ClassExpression* rhs;
};

// EquivalentClasses axiom
struct equivalent_classes_axiom {
	ClassExpression* lhs;
	ClassExpression* rhs;
};

// DisjointClasses axiom
struct disjoint_classes_axiom {
	int concept_count;
	ClassExpression **concepts;
};

// SubObjectPropertyOf axiom
struct subobject_property_of_axiom {
	ObjectPropertyExpression* lhs;
	ObjectPropertyExpression* rhs;
};

// TransitiveObjectProperty axiom
struct transitive_object_property_axiom {
	ObjectPropertyExpression* r;
};

// EquivalentObjectProperties axiom
struct equivalent_object_properties_axiom {
	ObjectPropertyExpression* lhs;
	ObjectPropertyExpression* rhs;
};

// TBox
struct tbox {
	// Top concept
	ClassExpression* top_concept;
	// Bottom concept
	ClassExpression* bottom_concept;

	// Classes
	// Key: IRI (char*)
	// Value: ClassExpression*
	Map classes;

	// ObjectSomeValuesFrom expressions
	// Key: 64-bit hash value obtained from 32-bit role and filler ids
	// Value: ClassExpression*
	Map object_some_values_from_exps;
	// number of total ObjectSomeValuesFrom expressions in the ontology
	int object_some_values_from_exps_count;

	// ObjectIntersectionOf expressions
	// Key: 64-bit hash value obtained from the 32-bit ids of the two conjuncts
	// Value: ClassExpression*
	Map object_intersection_of_exps;
	// number of total ObjectIntersectionOf expressions in the ontology
	int object_intersection_of_exps_count;
	// number of resulting binary ObjectIntersectionOf expressions
	int binary_object_intersection_of_exps_count;

	// ObjectOneOf expressions
	// Key: the 32-bit id of the underlying individual
	// Value: ClassExpression*
	Map object_one_of_exps;

	// ObjectProperties
	// Key: IRI (char*)
	// Value: ObjectPropertyExpression*
	Map object_properties;

	HashMap* role_compositions;
	int role_composition_count;
	int binary_role_composition_count;
	int unique_binary_role_composition_count;

	uint32_t last_concept_id;
	uint32_t last_role_id;

	SubClassOfAxiom** subclass_axioms;
	int subclass_axiom_count;

	EquivalentClassesAxiom** eqclass_axioms;
	int eqclass_axiom_count;

	SubObjectPropertyAxiom** subrole_axioms;
	int subrole_axiom_count;

	TransitiveObjectPropertyAxiom** transitive_role_axioms;
	int transitive_role_axiom_count;

	EquivalentObjectPropertiesAxiom** eqrole_axioms;
	int eqrole_axiom_count;

	DisjointClassesAxiom** disjointclasses_axioms;
	int disjointclasses_axiom_count;

};

/******************************************************************************/
// Individual
struct individual {
	uint32_t id;
	char* IRI;
};

// Concept assertion
struct class_assertion {
	Individual* individual;
	ClassExpression* concept;
};

// Role assertion
struct object_property_assertion {
	ObjectPropertyExpression* role;
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
	ClassAssertion** concept_assertions;

	int role_assertion_count;
	ObjectPropertyAssertion** role_assertions;
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
	SubClassOfAxiom** generated_subclass_axioms;
	int generated_subclass_axiom_count;

	// The list of subrole axioms that are generated during  preprocessing
	SubObjectPropertyAxiom** generated_subrole_axioms;
	int generated_subrole_axiom_count;

	// The hash of nominals that are generated during preprocessing.
	HashMap* generated_nominals;

	// The hash of existential restrictions that are generated during preprocessing.
	// They are generated from preprocessing role assertions.
	HashMap* generated_exists_restrictions;
	int generated_exists_restriction_count;
};

#endif
