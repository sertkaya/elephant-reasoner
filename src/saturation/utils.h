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
#include "../model/utils.h"
#include "context.h"
#include "locals.h"


// marks the axiom with the premise lhs and conclusion rhs as processed
#define MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)		add_to_concept_subsumer_list(ax->lhs, ax->rhs)
// int mark_concept_saturation_axiom_processed(ConceptSaturationAxiom* ax);

// int add_predecessor(Concept* c, Concept* ex);
// add r-predecessor p to c
int add_predecessor(Concept* c, Role* r, Concept* p);

// int add_successor(Concept* c, Concept* ex);
// add r-successor s to c
int add_successor(Concept* c, Role* r, Concept* s);

ConceptSaturationAxiom* create_concept_saturation_axiom(Concept* lhs, Concept* rhs, Role* role, enum saturation_axiom_type type);

RoleSaturationAxiom* create_role_saturation_axiom(Role* lhs, Role* rhs);

int mark_role_saturation_axiom_processed(RoleSaturationAxiom* ax);

#endif /* SATURATION_UTILS_H_ */
