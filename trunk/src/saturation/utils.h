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


#ifndef SATURATION_UTILS_H_
#define SATURATION_UTILS_H_

#include <Judy.h>

#include "datatypes.h"
#include "../model/datatypes.h"
#include "context.h"
#include "locals.h"


// returns 1 if axiom is added, 0 if the axiom already exists
// char add_to_processed_axioms(SubClassAxiom* ax);

// marks the axiom with the premise lhs and conclusion rhs as processed
// void mark_axiom_processed(Concept* lhs, Concept* rhs);

int mark_concept_saturation_axiom_processed(ConceptSaturationAxiom* ax);

// checks whether the axiom with the premise lhs and conclusion rhs has
// already been processed
// int is_axiom_processed(Concept* lhs, Concept* rhs);

// void enqueue(SaturationAxiom* ax);

// SaturationAxiom* dequeue();

int add_predecessor(Concept* c, Concept* ex);

ConceptSaturationAxiom* create_concept_saturation_axiom(Concept* lhs, Concept* rhs, char derived_via_conj_intro, char derived_via_exists_intro,
		enum saturation_axiom_type type);

RoleSaturationAxiom* create_role_saturation_axiom(Role* lhs, Role* rhs);

int mark_role_saturation_axiom_processed(RoleSaturationAxiom* ax);

#endif /* SATURATION_UTILS_H_ */
