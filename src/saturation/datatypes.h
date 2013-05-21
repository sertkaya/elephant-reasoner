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
	INPUT_KB_SUBCLASS, INPUT_KB_EQCLASS, CONJ_INTRO_1, CONJ_INTRO_2, CONJ_DECOMP_1, CONJ_DECOMP_2,
	EXISTS_DECOMP, INIT, EXISTS_INTRO, TOLD_SUBSUMER
};

struct concept_saturation_axiom {
	Concept* lhs;
	Concept* rhs;
	char derived_via_conj_introduction;
	char derived_via_exists_introduction;
	enum saturation_axiom_type type;
};

struct role_saturation_axiom {
	Role* lhs;
	Role* rhs;
};

#endif /* SATURATION_DATATYPES_H_ */
