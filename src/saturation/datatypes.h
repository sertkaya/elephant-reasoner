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


#ifndef SATURATION_DATATYPES_H_
#define SATURATION_DATATYPES_H_

#include "../model/datatypes.h"

typedef struct concept_saturation_axiom ConceptSaturationAxiom;
typedef struct role_saturation_axiom RoleSaturationAxiom;
typedef enum saturation_axiom_type ConceptSaturatuinAxiomType;

enum saturation_axiom_type {
	SUBSUMPTION_CONJUNCTION_INTRODUCTION,		// 0
	SUBSUMPTION_EXISTENTIAL_INTRODUCTION,		// 1
	SUBSUMPTION_EXISTENTIAL_INTRODUCTION_1,		// 2
	SUBSUMPTION_EXISTENTIAL_INTRODUCTION_2,		// 3
	SUBSUMPTION_EXISTENTIAL_DECOMPOSITION,		// 4
	SUBSUMPTION_INITIALIZATION,					// 5
	SUBSUMPTION_TOLD_SUBSUMER,					// 6
	SUBSUMPTION_CONJUNCTION_DECOMPOSITION,		// 7
	FORWARD_LINK,										// 8
	BACKWARD_LINK,										// 8
	SUBSUMPTION_BOTTOM							// 9
};

struct concept_saturation_axiom {
	ClassExpressionId lhs;
	ClassExpressionId rhs;
	ObjectPropertyExpressionId role;
	enum saturation_axiom_type type;
};

struct role_saturation_axiom {
	ObjectPropertyExpressionId lhs;
	ObjectPropertyExpressionId rhs;
};

#endif /* SATURATION_DATATYPES_H_ */
