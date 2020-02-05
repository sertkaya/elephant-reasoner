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


#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "../utils/set.h"
#include "../utils/map.h"
#include "../utils/list.h"
#include "../utils/list_64.h"
#include "../hashing/hash_table.h"
#include "../hashing/hash_map.h"


typedef uint32_t expression_id_t;
typedef expression_id_t ClassExpressionId;
typedef expression_id_t ObjectPropertyExpressionId;
typedef expression_id_t IndividualId;

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
typedef struct subobject_property_of_axiom SubObjectPropertyOfAxiom;
typedef struct equivalent_object_properties_axiom EquivalentObjectPropertiesAxiom;
typedef struct transitive_object_property_axiom TransitiveObjectPropertyAxiom;
typedef struct objectproperty_domain_axiom ObjectPropertyDomainAxiom;
typedef struct same_individual_axiom SameIndividualAxiom;
typedef struct different_individuals_axiom DifferentIndividualsAxiom;

#define EXPRESSION_ID_NULL 0

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

	// Set of equivalent classes. Elements are ClassExpressionId
	Set equivalent_classes;
	// Set of direct subsumers. Elements are ClassExpressionId
	Set direct_subsumers;
};


// ObjectIntersectionOf
struct object_intersection_of {
	ClassExpressionId conjunct1;
	ClassExpressionId conjunct2;
};


// ObjectSomeValuesFrom
struct object_some_values_from {
	ObjectPropertyExpressionId role;
	ClassExpressionId filler;
};

// ObjectOneOf
struct object_one_of {
	IndividualId individual;
};

// Class description
union class_description {
	Class atomic;
	ObjectIntersectionOf conj;
	ObjectSomeValuesFrom exists;
	ObjectOneOf nominal;
}; 
	

// ClassExpression
struct class_expression {
	// Unique id
	// 32-bit unsigned integer. Needed for hashing.
	// uint32_t id;

	enum class_expression_type type;
	ClassDescription description;

	// List of told subsumers. Elements are ClassExpressionId
	List told_subsumers;

	// Set of subsumers computed during saturation.
	// Elements are ClassExpressionId
	Set subsumers;

	// 2-dimensional dynamic array for storing predecessors.
	Link* predecessors;
	// Number of roles, for which this concept has a predecessor (the size of predecessors array)
	int predecessor_r_count;

	// the same data structure to store the successors.
	// needed for implementing the role composition rule in saturation
	Link* successors;
	int successor_r_count;

	// set of negative existentials whose filler is this concept
	Map* filler_of_negative_exists;

	// List of conjunctions where this concept is the first/second conjunct.
	// Elements are ClassExpressionId
	List first_conjunct_of_list;
	List second_conjunct_of_list;

	// The set of conjunctions where this concept is the first/second conjunct.
	// The reason for storing them once in a list and once in a set is performance
	// in saturation.
	Set* first_conjunct_of;
	Set* second_conjunct_of;

};

// For keeping successors and predecessors of a  concept
struct link {
	ObjectPropertyExpressionId role;
	// ClassExpressionId* fillers;
	// int filler_count;
	// Set of all role fillers
	Set fillers;
	// Set of role fillers that are not obtained via the existential introduction rule
	Set fillers_not_exist_introduction;
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

// Object property chain
struct object_property_chain {
	ObjectPropertyExpressionId role1;
	ObjectPropertyExpressionId role2;
};

// Object property description
union object_property_description {
	ObjectProperty atomic;
	ObjectPropertyChain object_property_chain;
}; 

// Object property expression
struct object_property_expression {
	// Unique role id.
	// 32-bit unsigned integer, needed for hashing.
	// uint32_t id;

	enum object_property_expression_type type;
	ObjectPropertyDescription description;

	Set told_subsumers;

	Set subsumers;
	// List of subsumers of this object property.
	// Elements are ObjectPropertyExpressionId
	List subsumer_list;

	// Only necessary for optimizing the processing of role compositions
	// For that we need to access the subsumees
	Set subsumees;

	// List of role compositions where this role is the first/second component
	ObjectPropertyExpressionId* first_component_of_list;
	int first_component_of_count;
	ObjectPropertyExpressionId* second_component_of_list;
	int second_component_of_count;

	// Same as above. The reason is performance in saturation.
	Set first_component_of;
	Set second_component_of;
};

/******************************************************************************/
// SubClassOf axiom
struct subclass_of_axiom {
	ClassExpressionId lhs;
	ClassExpressionId rhs;
};

// EquivalentClasses axiom
struct equivalent_classes_axiom {
	ClassExpressionId lhs;
	ClassExpressionId rhs;
};

// DisjointClasses axiom
struct disjoint_classes_axiom {
	// List of classes in this axiom.
	// Elements are of type ClassExpressionId
	List classes;
};

// SubObjectPropertyOf axiom
struct subobject_property_of_axiom {
	ObjectPropertyExpressionId lhs;
	ObjectPropertyExpressionId rhs;
};

// TransitiveObjectProperty axiom
struct transitive_object_property_axiom {
	ObjectPropertyExpressionId r;
};

// EquivalentObjectProperties axiom
struct equivalent_object_properties_axiom {
	ObjectPropertyExpressionId lhs;
	ObjectPropertyExpressionId rhs;
};

struct objectproperty_domain_axiom {
	ObjectPropertyExpressionId object_property_expression;
	ClassExpressionId class_expression;
};

struct same_individual_axiom {
	// List of individuals in this axiom
	// Elements are of type IndividualId
	List individuals;
};

struct different_individuals_axiom {
	// List of individuals in this axiom
	// Elements are of type IndividualId
	List individuals;
};

// TBox
struct tbox {
	// Each class expression has unique  id,
	// which is equivalent to its index in the
	// class expressions array.
	// Stores the last assigned class expression id. Increment before using
	ClassExpressionId last_class_expression_id;

	// Each object property expression  has unique  id,
	// which is equivalent to its index in the
	// object property expression array.
	// Stores the last assigned object property expression id. Increment before using
	ClassExpressionId last_object_property_expression_id;

	// Array of class expressions
	// Also contains the class expressions generated during preprocessing
	ClassExpression* class_expressions;

	// Size of the class expressions array
	unsigned int class_expressions_size;

	// Array of object property expressions
	ObjectPropertyExpression* object_property_expressions;

	// Number of object property expressions in the tbox
	unsigned int object_property_expressions_size;

	// Top concept
	ClassExpressionId top_concept;
	// Bottom concept
	ClassExpressionId bottom_concept;

	// Classes
	// Key: IRI (char*)
	// Value: ClassExpressionId
	Map classes;

	// ObjectSomeValuesFrom expressions
	// Key: 64-bit hash value obtained from 32-bit role and filler ids
	// Value: ClassExpressionId
	Map object_some_values_from_exps;
	// number of total ObjectSomeValuesFrom expressions in the ontology
	int object_some_values_from_exps_count;

	// Binary ObjectIntersectionOf expressions
	// Key: 64-bit hash value obtained from the 32-bit ids of the two conjuncts
	// Value: ClassExpressionId
	Map object_intersection_of_exps;
	// number of total ObjectIntersectionOf expressions in the ontology
	int object_intersection_of_exps_count;
	// number of resulting binary ObjectIntersectionOf expressions
	int binary_object_intersection_of_exps_count;

	// ObjectOneOf expressions
	// Key: the 32-bit id of the underlying individual
	// Value: ClassExpressionId
	Map object_one_of_exps;

	// ObjectProperties
	// Key: IRI (char*)
	// Value: ObjectPropertyExpressionId
	Map objectproperties;

	// Binary ObjectPropertyChains
	// Key: 64-bit hash value obtained from the 32-bit ids of the two components
	// Value: ObjectPropertyExpressionId
	Map objectproperty_chains;
	int role_composition_count;
	int binary_role_composition_count;

	// The set of SubClassOf axioms. The members are
	// of type SubClassOfAxiom*.
	Set_64 subclass_of_axioms;

	// The set of EquivalentClasses axioms. The members
	// are of type EquivalentClassesAxiom*.
	Set_64 equivalent_classes_axioms;

	// The set of SubObjectPropertyOf axioms. The members
	// are of type SubObjectPropertyOfAxiom*.
	Set_64 subobjectproperty_of_axioms;

	// The set of TransitiveObjectProperty axioms. The
	// members are of type TransitiveObjectPropertyAxiom*.
	Set_64 transitive_objectproperty_axioms;

	// The set of EquivalentObjectProperties axioms. The
	// members are of type EquivalentObjectPropertiesAxiom*.
	Set_64 equivalent_objectproperties_axioms;

	// The set of DisjointClasses axioms. The
	// members are of type DisjointClassesAxiom*.
	Set_64 disjoint_classes_axioms;

	// The set of ObjectPropertyDomain axioms.
	// The members are of type ObjectPropertyDomainAxiom*.
	Set_64 objectproperty_domain_axioms;

	// The set of SameIndividual axioms
	// The members are of type SameIndividualAxiom*
	Set_64 same_individual_axioms;

	// The set of DifferentIndividuals axioms
	// The members are of type DifferentIndividualsAxiom*
	Set_64 different_individuals_axioms;

	// Class Expressions and axioms resulting from syntactic conversions in prepocessing

	// The list of subclass axioms that result from converting syntactic
	// shortcuts (disjointness axioms, etc.) to subclass axioms. They are generated in preprocessing.
	Set_64 generated_subclass_of_axioms;

	// The list of subrole axioms that are generated during  preprocessing
	Set_64 generated_subobjectproperty_of_axioms;

	// The hash of nominals that are generated during preprocessing.
	Map generated_object_one_of_exps;

	// The hash of existential restrictions that are generated during preprocessing.
	// They are generated from preprocessing role assertions.
	Map generated_object_some_values_from_exps;
	int generated_object_some_values_from_count;

};

/******************************************************************************/
// Individual
struct individual {
	// uint32_t id;
	char* IRI;
};

// Concept assertion
struct class_assertion {
	IndividualId individual;
	ClassExpressionId concept;
};

// Role assertion
struct object_property_assertion {
	ObjectPropertyExpressionId role;
	IndividualId source_individual;
	IndividualId target_individual;
};

// ABox
struct abox {
	int last_individual_id;

	int individual_count;

	 Individual* individuals;

	 int individuals_size;

	HashMap* individuals_map;

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

	// List for the prefix names
	List_64 prefix_names;
	// Map for prefixes.
	// Key: prefix name, Value: prefix
	Map_64 prefixes;


	// occurrence of owl:Thing on the lhs of an axiom
	char top_occurs_on_lhs;
	// occurrence of owl:Nothing on the rhs of an axiom
	char bottom_occurs_on_rhs;

};

#endif
