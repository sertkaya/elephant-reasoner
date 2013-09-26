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

#include <Judy.h>

typedef struct atomic_concept AtomicConcept;
typedef struct conjunction Conjunction;
typedef struct existential_restriction Exists;
typedef union concept_description ConceptDescription;
typedef struct concept Concept;

typedef struct atomic_role AtomicRole;
typedef struct role_composition RoleComposition;
typedef union role_description RoleDescription;
typedef struct role Role;

typedef struct subclass_axiom SubClassAxiom;
typedef struct eqclass_axiom EqClassAxiom;
typedef struct subrole_axiom SubRoleAxiom;
typedef struct eqrole_axiom EqRoleAxiom;
typedef struct transitive_role_axiom TransitiveRoleAxiom;

typedef struct axiom Axiom;

typedef struct tbox TBox;

// for lexer and parser
typedef union expression Expression;
union expression {
	Concept* concept;
	Role* role;
};


// concept description types
enum concept_description_type {
	ATOMIC_CONCEPT, CONJUNCTION, EXISTENTIAL_RESTRICTION
};

// atomic concept
struct atomic_concept {
	char* name;
};


// conjunction
struct conjunction {
	Concept* conjunct1;
	Concept* conjunct2;
};


// existential restriction
struct existential_restriction {
	Role* role;
	Concept* filler;
};


// concept description
union concept_description {
	AtomicConcept* atomic;
	Conjunction* conj;
	Exists* exists;
}; 
	

// concept description
struct concept {
	int id;

	enum concept_description_type type;
	ConceptDescription description;

	// char occurs_on_lhs;

	// Pvoid_t told_subsumers;
	Concept** told_subsumers;
	int told_subsumer_count;

	Concept** subsumer_list;
	int subsumer_count;

	// accessed via J1* operations
	Pvoid_t subsumers;
	Pvoid_t equivalent_concepts;
	Pvoid_t direct_subsumers;

	// 2 dimensional Judy array
	// key is Role*, value is pointer to J1 array
	// indices of the J1 array are Concept*
	Pvoid_t predecessors;

	// hash of negative existentials whose filler is this concept
	Pvoid_t filler_of_negative_exists;

	// list of conjunctions where this concept is the first/second conjunct
	Concept** first_conjunct_of_list;
	int first_conjunct_of_count;
	Concept** second_conjunct_of_list;
	int second_conjunct_of_count;

	// same as above. the reason is performance in saturation.
	Pvoid_t first_conjunct_of;
	Pvoid_t second_conjunct_of;

};

///////////////////////////////////////////////////////////////////////////////

// role description types
enum role_description_type {
	ATOMIC_ROLE, ROLE_COMPOSITION
};

struct atomic_role {
	char* name;
};

// role description
// (we represent it with union in case we need role constructors later)
union role_description {
	AtomicRole* atomic;
	RoleComposition* role_composition;
}; 
	
// role composition
struct role_composition {
	// int size;
	// Role** roles;
	Role* role1;
	Role* role2;
};

// role 
struct role {
	int id;
	enum role_description_type type;
	RoleDescription description;

	Role** told_subsumers;
	int told_subsumer_count;

	Role** subsumer_list;
	int subsumer_count;

	// accessed via J1* operations
	Pvoid_t subsumers;

	// list of role compositions where this role is the first/second component
	Role** first_component_of_list;
	int first_component_of_count;
	Role** second_component_of_list;
	int second_component_of_count;

	// same as above. the reason is performance in saturation.
	Pvoid_t first_component_of;
	Pvoid_t second_component_of;
};

///////////////////////////////////////////////////////////////////////////////
// axiom types
// enum axiom_type {
// 	SUBCLASS_AX, EQCLASS_AX, SUBROLE_AX
// };

// Subclass axiom
struct subclass_axiom {
	Concept* lhs;
	Concept* rhs;
};

// Equivalent class axiom
struct eqclass_axiom {
	Concept* lhs;
	Concept* rhs;
};

// RI
// struct role_inclusion {
struct subrole_axiom {
	Role* lhs;
	Role* rhs;
};

struct transitive_role_axiom {
	Role* r;
};

struct eqrole_axiom {
	Role* lhs;
	Role* rhs;
};

// Obsolete.
// inclusion axiom
// union axiom_body {
// 	SubClassAxiom* subclass_ax;
// 	EqClassAxiom* eqclass_ax;
// 	SubRoleAxiom* subrole_ax;
// };

// axiom
// struct axiom {
// 	enum axiom_type type;
// 	union axiom_body body;
// };

// TBox
struct tbox {
	// top concept
	Concept* top_concept;
	// bottom concept
	Concept* bottom_concept;

	// atomic concepts hash
	Pvoid_t atomic_concepts;
	Concept** atomic_concept_list;
	int atomic_concept_count;

	// exists restictions hash
	// the key is role id_filler id
	Pvoid_t exists_restrictions;
	int exists_restriction_count;
	int unique_exists_restriction_count;

	// hash for conjunctions
	// key: id of conjuct1_id of conjunct2
	// where conjuct ids are sorted
	Pvoid_t conjunctions;
	int conjunction_count;
	// int unique_conjunction_count;
	int binary_conjunction_count;
	int unique_binary_conjunction_count;

	Pvoid_t atomic_roles;
	int atomic_role_count;

	Pvoid_t role_compositions;
	int role_composition_count;
	int binary_role_composition_count;
	int unique_binary_role_composition_count;

	int last_concept_id;
	int last_role_id;

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
};

#endif /* DATATYPES_H_ */
