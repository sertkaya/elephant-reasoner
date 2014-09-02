/*
 * The ELepHant Reasoner
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


#ifndef PREPROCESSING_UTILS_H_
#define PREPROCESSING_UTILS_H_

#include "../model/datatypes.h"

void add_generated_subclass_axiom(KB* kb, SubClassAxiom* ax);

void add_generated_subrole_axiom(KB* kb, SubRoleAxiom* ax);

ClassExpression* get_create_generated_nominal(KB* kb, Individual* ind);

ClassExpression* get_create_generated_exists_restriction(KB* kb, Role* r, ClassExpression* f);

#endif
