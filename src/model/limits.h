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


#ifndef LIMITS_H_
#define LIMITS_H_

// configuration parameters

// Maximum lengths/sizes for the parser
#define MAX_CONJUNCT_COUNT				1024 /* max number of conjuncts in an ObjectIntersectionOf expression */
#define MAX_EQ_CLASS_EXP_COUNT			1024 /* max number of class expressions in an EquivalentClasses axiom */
#define MAX_EQ_ROLE_EXP_COUNT			1024 /* max number of object property expressions in an EquivalentObjectProperties axiom */
#define MAX_ROLE_COMPOSITION_SIZE		1024 /* max size of an ObjectPropertyChain expression */
// #define MAX_CLASS_NAME_LENGTH			1024 /* max size for class names */
// #define MAX_OBJECT_PROPERTY_NAME_LENGTH	1024 /* max size for object property names */
#define MAX_DISJ_CLASS_EXP_COUNT		1024 /* max number of disjoint classes in a DisjointClasses axiom */
#define MAX_SAME_INDIVIDUAL_COUNT		1024 /* max number of individuals in a SameIndividual axiom */
#define MAX_DIFFERENT_INDIVIDUALS_COUNT	1024 /* max number of individuals in a DifferentIndividuals axiom */

// Default set/map sizes (per ontology)
#define DEFAULT_PREFIXES_HASH_SIZE					32
#define DEFAULT_ATOMIC_CONCEPTS_HASH_SIZE			150000
#define DEFAULT_ATOMIC_ROLES_HASH_SIZE				200
#define DEFAULT_INDIVIDUALS_HASH_SIZE				100000
#define DEFAULT_EXISTS_RESTRICTIONS_HASH_SIZE		80000
#define DEFAULT_CONJUNCTIONS_HASH_SIZE				100000
#define DEFAULT_NOMINALS_HASH_SIZE					10000
#define DEFAULT_ROLE_COMPOSITIONS_HASH_SIZE			200

// Default set sizes for axioms (per ontology)
#define INITIAL_DEFAULT_CLASS_EXPRESSION_COUNT                      100000
#define DEFAULT_CLASS_EXPRESSION_COUNT_INCREMENT            100000
#define INITIAL_DEFAULT_OBJECT_PROPERTY_EXPRESSION_COUNT            100
#define DEFAULT_OBJECT_PROPERTY_EXPRESSION_COUNT_INCREMENT  100
#define DEFAULT_INDIVIDUAL_COUNT                            100
#define DEFAULT_INDIVIDUAL_COUNT_INCREMENT                  100

#define DEFAULT_SUBCLASS_OF_AXIOMS_SET_SIZE                 1024
#define DEFAULT_EQUIVALENT_CLASSES_AXIOMS_SET_SIZE          1024
#define DEFAULT_DISJOINT_CLASSES_AXIOMS_SET_SIZE            1024
#define DEFAULT_SUBOBJECTPROPERTY_OF_AXIOMS_SET_SIZE        128
#define DEFAULT_TRANSITIVE_OBJECTPROPERTY_AXIOMS_SET_SIZE		128
#define DEFAULT_EQUIVALENT_OBJECTPROPERTIES_AXIOMS_SET_SIZE 128
#define DEFAULT_OBJECTPROPERTY_DOMAIN_AXIOMS_SET_SIZE       128
#define DEFAULT_SAME_INDIVIDUAL_AXIOMS_SET_SIZE             128
#define DEFAULT_DIFFERENT_INDIVIDUALS_AXIOMS_SET_SIZE       128

// Default set/map sizes (per class expression)
#define DEFAULT_NEGATIVE_FILLER_OF_SET_SIZE			32
#define DEFAULT_FIRST_CONJUNCT_OF_HASH_SIZE			32
#define DEFAULT_SECOND_CONJUNCT_OF_HASH_SIZE		32
#define DEFAULT_SUBSUMERS_HASH_SIZE					32
#define DEFAULT_DIRECT_SUBSUMERS_SET_SIZE			32
#define DEFAULT_EQUIVALENT_CONCEPTS_SET_SIZE		32
#define DEFAULT_PREDECESSORS_SET__SIZE		32
#define DEFAULT_SUCCESSORS_SET__SIZE		32

// Default set/map sizes (per objectproperty expression)
#define DEFAULT_ROLE_TOLD_SUBSUMERS_HASH_SIZE		16
#define DEFAULT_ROLE_SUBSUMERS_HASH_SIZE			16
#define DEFAULT_ROLE_SUBSUMEES_HASH_SIZE			16
#define DEFAULT_ROLE_FIRST_COMPONENT_OF_HASH_SIZE	16
#define DEFAULT_ROLE_SECOND_COMPONENT_OF_HASH_SIZE	16


#endif
