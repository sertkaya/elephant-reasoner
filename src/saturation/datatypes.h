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

enum saturation_axiom_type {
	SUBSUMPTION_CONJUNCTION_INTRODUCTION, SUBSUMPTION_EXISTENTIAL_INTRODUCTION, SUBSUMPTION_INITIALIZATION, SUBSUMPTION_TOLD_SUBSUMER, SUBSUMPTION_CONJUNCTION_DECOMPOSITION,
	LINK, SUBSUMPTION_BOTTOM
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
