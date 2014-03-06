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


#ifndef INDEX_UTILS_H_
#define INDEX_UTILS_H_

/******************************************************************************
 * functions for adding subsumer/told subsumer to a concept
 *****************************************************************************/
// add concept s to the subsumer list of concept c
// returns 1 if added, 0 otherwise
// note that it maintains both the array and the judy array
// of subsumers. the reason for keeping the subsumers twice
// is performance in saturation
int add_to_concept_subsumer_list(Concept* c, Concept* s);

// add s to the told subsumer list of c
// note that multiple occurrence of s is allowed!
void add_told_subsumer_concept(Concept* c, Concept* s);



/******************************************************************************
 * functions for adding subsumer/told subsumer to a role
 *****************************************************************************/
int add_to_role_subsumer_list(Role* r, Role* s);

int add_to_role_subsumees(Role* r, Role* s);

// add s to the told subsumer list of r
// note that multiple occurrence of s is allowed!
void add_told_subsumer_role(Role* r, Role* s);



/******************************************************************************
 * functions for adding ...
 *****************************************************************************/
// add ex to the list of existential restrictions that f is a filler of
void add_to_filler_of_list(Concept* f, Concept* ex);

// add 'conjunction' to the list of conjunctions whose first conjunct is 'concept'
void add_to_first_conjunct_of_list(Concept* concept, Concept* conjunction);

// add 'conjunction' to the list of conjunctions whose second conjunct is 'concept'
void add_to_second_conjunct_of_list(Concept* concept, Concept* conjunction);

// add ex to the filler_of_negative_exists hash of the filler of ex.
// key of the hash is ex->description.exists->role, the value is ex.
void add_to_negative_exists(Concept* ex, TBox* tbox);
// #define ADD_TO_NEGATIVE_EXISTS(ex)		insert_key_value(ex->description.exists->filler->filler_of_negative_exists, ex->description.exists->role->id, ex)

// return the negative existential restriction whose role is r and filler is c
// returns NULL is no such existential restriction is found
// Concept* get_negative_exists(Concept* c, Role* r);
// #define GET_NEGATIVE_EXISTS(c, r)	((Concept*) get_value(c->filler_of_negative_exists, r->id))
#define GET_NEGATIVE_EXISTS(c, r)	(c->filler_of_negative_exists == NULL ? NULL : c->filler_of_negative_exists[r->id])



#endif /* INDEX_UTILS_H_ */
