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


#ifndef SATURATION_DATATYPES_H_
#define SATURATION_DATATYPES_H_

typedef struct concept_saturation_axiom ConceptSaturationAxiom;
typedef struct role_saturation_axiom RoleSaturationAxiom;

enum saturation_axiom_type {
	TOLD_SUBSUMER, INITIALIZATION, CONJUNCTION_DECOMPOSITION, CONJUNCTION_INTRODUCTION, EXISTENTIAL_DECOMPOSITION, EXISTENTIAL_INTRODUCTION, ROLE_CHAIN, LINK
};

struct concept_saturation_axiom {
	Concept* lhs;
	Concept* rhs;
	enum saturation_axiom_type type;
};

struct role_saturation_axiom {
	Role* lhs;
	Role* rhs;
};

#endif /* SATURATION_DATATYPES_H_ */
