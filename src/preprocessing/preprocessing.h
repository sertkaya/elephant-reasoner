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

#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include <stdio.h>
#include "../model/datatypes.h"

/*
// The list of subclass axioms that are generated during  preprocessing
SubClassAxiom** generated_subclass_axioms;
int generated_subclass_axiom_count;

// The list of subrole axioms that are generated during  preprocessing
SubRoleAxiom** generated_subrole_axioms = NULL;
int generated_subrole_axiom_count = 0;
*/

void preprocess_kb(KB* kb);

// void preprocess_tbox(TBox* tbox);

// void preprocess_abox(ABox* abox);

#endif
