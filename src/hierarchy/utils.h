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


#ifndef _HIERARCHY_UTILS_H_
#define _HIERARCHY_UTILS_H_

// Add concept c1 to the list of equivalent concepts of c2
void add_equivalent_concept(Concept* c1, Concept* c2);

// Add concept c1 to the list of direct subsumers of c2
int add_direct_subsumer(Concept* c1, Concept* c2);
// #define ADD_DIRECT_SUBSUMER(c1,c2)			insert_key(c2->description.atomic->direct_subsumers, c1->id)

// returns 1 if c1 is a direct subsumer of c2, otherwise 0
int is_direct_subsumer_of(Concept* c1, Concept* c2);

// returns 1 if c1 is a subsumer of c2, otherwise 0
int is_subsumer_of(Concept* c1, Concept* c2);

// remove c1 from the list of direct subsumers of c2
int remove_direct_subsumer(Concept* c1, Concept* c2);
// #define REMOVE_DIRECT_SUBSUMER(c1,c2)		remove_key(c2->description.atomic->direct_subsumers, c1->id)


#endif /* _HIERARCHY_UTILS_H_ */
