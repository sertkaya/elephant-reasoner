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


#ifndef LIMITS_H_
#define LIMITS_H_

// configuration parameters

// max number of conjuncts for a conjunction
#define MAX_CONJUNCT_COUNT				1024

// max number of class expressions that can occur in an EquivalentClasses axiom
#define MAX_EQ_CLASS_EXP_COUNT			1024

// max size of a role composition
#define MAX_ROLE_COMPOSITION_SIZE		1024

#define MAX_CONCEPT_NAME_LENGTH			1024
#define MAX_ROLE_NAME_LENGTH			1024

// default size of hash tables
#define DEFAULT_ATOMIC_CONCEPTS_HASH_SIZE		100000
#define DEFAULT_ATOMIC_ROLES_HASH_SIZE			100

#define DEFAULT_FIRST_CONJUNCT_OF_HASH_SIZE		10
#define DEFAULT_SECOND_CONJUNCT_OF_HASH_SIZE	10
#define DEFAULT_SUBSUMERS_HASH_SIZE				10
#define DEFAULT_DIRECT_SUBSUMERS_HASH_SIZE		10
#define DEFAULT_EQUIVALENT_CONCEPTS_HASH_SIZE	10
#define DEFAULT_ROLE_SUBSUMERS_HASH_SIZE		10
#define DEFAULT_ROLE_SUBSUMEES_HASH_SIZE		10
#define DEFAULT_ROLE_FIRST_COMPONENT_OF_HASH_SIZE		10
#define DEFAULT_ROLE_SECOND_COMPONENT_OF_HASH_SIZE		10

#endif
