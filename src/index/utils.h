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


#ifndef TBOX_INDEX_UTILS_H_
#define TBOX_INDEX_UTILS_H_

/******************************************************************************
 * functions for adding subsumer/told subsumer to a concept
 *****************************************************************************/
// add concept s to the subsumer list of concept c
// returns 1 if added, 0 otherwise
// note that it maintains both the array and the judy array
// of subsumers. the reason for keeping the subsumers twice
// is performance in saturation
int add_to_concept_subsumer_list(ClassExpression* c, ClassExpression* s);

// add s to the told subsumer list of c
// note that multiple occurrence of s is allowed!
// void add_told_subsumer_concept(ClassExpression* c, ClassExpression* s);

// TODO: what about multiple occurrences? Performance?
// add s to the told subsumer list of c
// note that multiple occurrence of s is allowed!
#define ADD_TOLD_SUBSUMER_CLASS_EXPRESSION(s, c)	list_add(s, &(c->told_subsumers))



/******************************************************************************
 * functions for adding subsumer/told subsumer to a role
 *****************************************************************************/
int add_to_role_subsumer_list(ObjectPropertyExpression* r, ObjectPropertyExpression* s);

int add_to_role_subsumee_list(ObjectPropertyExpression*r, ObjectPropertyExpression* s);

int add_to_role_subsumees(ObjectPropertyExpression* r, ObjectPropertyExpression* s);

#define ADD_TOLD_SUBSUMER_OBJECT_PROPERTY_EXPRESSION(s, r)		SET_ADD(s, &(r->told_subsumers))

#define ADD_TOLD_SUBSUMEE_OBJECT_PROPERTY_EXPRESSION(s, r)		SET_ADD(s, &(r->told_subsumees))


/******************************************************************************
 * functions for adding ...
 *****************************************************************************/
// add ex to the list of existential restrictions that f is a filler of
void add_to_filler_of_list(ClassExpression* f, ClassExpression* ex);

// add 'conjunction' to the list of conjunctions whose first conjunct is 'concept'
void add_to_first_conjunct_of_list(ClassExpression* concept, ClassExpression* conjunction);

// add 'conjunction' to the list of conjunctions whose second conjunct is 'concept'
void add_to_second_conjunct_of_list(ClassExpression* concept, ClassExpression* conjunction);

// add ex to the filler_of_negative_exists hash of the filler of ex.
// key of the hash is ex->description.exists->role, the value is ex.
void add_to_negative_exists(ClassExpression* ex, TBox* tbox);
// #define ADD_TO_NEGATIVE_EXISTS(ex)		insert_key_value(ex->description.exists->filler->filler_of_negative_exists, ex->description.exists->role->id, ex)

// return the negative existential restriction whose role is r and filler is c
// returns NULL is no such existential restriction is found
#define GET_NEGATIVE_EXISTS(c, r)	(c->filler_of_negative_exists == NULL ? NULL : c->filler_of_negative_exists[r->id])


/******************************************************************************
 * functions for adding to role ...
 *****************************************************************************/
void add_role_to_first_component_of_list(ObjectPropertyExpression* role, ObjectPropertyExpression* composition);

void add_role_to_second_component_of_list(ObjectPropertyExpression* role, ObjectPropertyExpression* composition);

#endif
