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


#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include "../model/datatypes.h"

// utility functions for printing concepts, axioms and statistics
void print_exists(Exists* ex);
void print_concept(Concept* c);
void print_subclass_axiom(SubClassAxiom* subclass_ax);
void print_eqclass_axiom(EqClassAxiom* subclass_ax);
void print_subrole_axiom(SubRoleAxiom* subrole_ax);
void print_short_stats(TBox* tbox);
void print_conjunctions(TBox* tbox);
void print_axioms(Axiom** axioms, int count);
void print_tbox(TBox* tbox);
// print the computed concept hierarchy
void print_concept_hierarchy(TBox* tbox, FILE* taxonomy_fp);

#endif