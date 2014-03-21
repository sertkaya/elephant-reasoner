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

#include "datatypes.h"
#include "../model/datatypes.h"
#include "../model/utils.h"
#include "../hashing/key_hash_table.h"


#define IS_SUBSUMED_BY(c1,c2)						contains_key(c1->subsumers, c2->id)

// unsigned long hash_it(int x, int y);
// long long hash_it(int x, int y);

// int is_subsumed_by(Concept* c1, Concept* c2);

// int add_predecessor(Concept* c, Concept* ex);
// add r-predecessor p to c
int add_predecessor(Concept* c, Role* r, Concept* p, TBox* tbox);

// int add_successor(Concept* c, Concept* ex);
// add r-successor s to c
int add_successor(Concept* c, Role* r, Concept* s, TBox* tbox);

#endif /* SATURATION_UTILS_H_ */
