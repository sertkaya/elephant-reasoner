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


#ifndef ABOX_DATATYPES_H_
#define ABOX_DATATYPES_H_

#include "../datatypes.h"
#include "../../hashing/key_value_hash_table.h"

typedef struct individual Individual;

typedef struct concept_assertion ConceptAssertion;
typedef struct role_assertion RoleAssertion;

typedef struct abox ABox;

struct individual {
	uint32_t id;
	char* name;
};

struct concept_assertion {
	Individual* individual;
	Concept* concept;
};

struct role_assertion {
	Role* role;
	Individual* source_individual;
	Individual* target_individual;
};

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

#endif
