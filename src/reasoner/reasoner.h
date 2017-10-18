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


#ifndef REASONER_H_
#define REASONER_H_

#include "../model/datatypes.h"

// for saturation statistics
extern int saturation_unique_subsumption_count, saturation_total_subsumption_count;
extern int saturation_unique_link_count, saturation_total_link_count;

// initializes the TBox and the ABox
KB* init_kb();

void read_kb(FILE* input_kb, KB* kb);

void classify(KB* kb);

char check_consistency(KB* kb);

char realize_kb(KB* kb);


#endif
