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


#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <stdio.h>

#include "datatypes.h"

char* class_expression_to_string(KB* kb, ClassExpression* c);
char* object_property_expression_to_string(KB* kb, ObjectPropertyExpression* r);

// utility functions for printing concepts, axioms and statistics
void print_exists(ObjectSomeValuesFrom* ex);
void print_concept(ClassExpression* c);
void print_subclass_axiom(SubClassOfAxiom* subclass_ax);
void print_eqclass_axiom(EquivalentClassesAxiom* subclass_ax);
void print_subrole_axiom(SubObjectPropertyOfAxiom* subrole_ax);
void print_conjunctions(TBox* tbox);
void print_tbox(TBox* tbox);
// print the computed concept hierarchy
void print_concept_hierarchy(KB* kb, FILE* taxonomy_fp);
// print the computed individual types
void print_individual_types(KB* kb, FILE* taxonomy_fp);

// void print_short_stats(TBox* tbox, ABox* abox);
void print_short_stats(KB* kb);

#endif
