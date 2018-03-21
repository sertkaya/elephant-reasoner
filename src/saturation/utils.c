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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../model/datatypes.h"
#include "../model/limits.h"
#include "utils.h"

// add p to the predecessors hash of c
// the key of the predecessors hash is r
int add_predecessor(ClassExpression* c, ObjectPropertyExpression* r, ClassExpression* p, TBox* tbox) {

	// first check if we already have a link for role r
	int i, j;
	void* tmp;

	// Busy-wait if already locked
	while (atomic_flag_test_and_set(&c->is_predecessors_locked));

	Node* node;
	LinkedListIterator* it = linked_list_iterator_create(c->predecessors);
	while ((node = linked_list_iterator_next(it)) != NULL) {
		if (((Link*) node->content)->role == r) {
			// yes, we have a link for role r, now check if we already have p in its fillers list
			int added = SET_ADD(p, &(((Link*) node->content)->fillers));
			atomic_flag_clear(&c->is_predecessors_locked);
			return(added);
		}
	}
	/*
	for (i = 0; i < c->predecessor_r_count; ++i)
		if (c->predecessors[i].role == r) {
			// yes, we have a link for role r, now check if we already have p in its fillers list
			int added = SET_ADD(p, &(c->predecessors[i].fillers));
			atomic_flag_clear(&c->is_predecessors_locked);
			return(added);
		}
	*/
	// no, we do not already have a link for role r, create it, add p to its filler list
	// 1) extend the list for links
	/*
	tmp = realloc(c->predecessors, (c->predecessor_r_count + 1) * sizeof(Link));
	assert(tmp != NULL);
	c->predecessors = (Link*) tmp;
	// 2) fill the fields of the new link
	c->predecessors[c->predecessor_r_count].role = r;
	*/
	Link* new_link = malloc(sizeof(Link));
	assert(new_link != NULL);
	new_link->role = r;
	/*
	SET_INIT(&(c->predecessors[c->predecessor_r_count].fillers), DEFAULT_PREDECESSORS_SET__SIZE);
	SET_ADD(p, &(c->predecessors[c->predecessor_r_count].fillers));
	*/
	SET_INIT(&(new_link->fillers), DEFAULT_PREDECESSORS_SET__SIZE);
	SET_ADD(p, &(new_link->fillers));

	// TODO: improve this !
	if (c->predecessors == NULL)
		c->predecessors = linked_list_create();

	linked_list_append(c->predecessors, new_link);

	// finally increment the r_count
	// ++c->predecessor_r_count;

	atomic_flag_clear(&c->is_predecessors_locked);

	// TODO: return the result of SET_ADD or perhaps linked_list_append??
	return(1);
}


int add_successor(ClassExpression* c, ObjectPropertyExpression* r, ClassExpression* p, TBox* tbox) {

	// first check if we already have a link for role r
	int i, j;
	void* tmp;
	for (i = 0; i < c->successor_r_count; ++i)
		if (c->successors[i].role == r) {
			// yes, we have a link for role r, now check if we already have p in its fillers list
			/*
			for (j = 0; j < c->successors[i].filler_count; ++j)
				if (c->successors[i].fillers[j] == p)
					return 0;
			// no we do not have p in this list, add it
			tmp = realloc(c->successors[i].fillers, (c->successors[i].filler_count + 1) * sizeof(ClassExpression*));
			assert(tmp != NULL);
			c->successors[i].fillers = (ClassExpression**) tmp;
			c->successors[i].fillers[c->successors[i].filler_count] = p;
			++c->successors[i].filler_count;
			return 1;
			*/
			return SET_ADD(p, &(c->successors[i].fillers));
		}
	// no, we do not already have a link for role r, create it, add p to its filler list
	// 1) extend the list for links
	tmp = realloc(c->successors, (c->successor_r_count + 1) * sizeof(Link));
	assert(tmp != NULL);
	c->successors = (Link*) tmp;
	// 2) fill the fields of the new link
	c->successors[c->successor_r_count].role = r;
	/*
	c->successors[c->successor_r_count].fillers = calloc(1, sizeof(ClassExpression*));
	assert(c->successors[c->successor_r_count].fillers != NULL);
	c->successors[c->successor_r_count].fillers[0] = p;
	c->successors[c->successor_r_count].filler_count = 1;
	*/
	SET_INIT(&(c->successors[c->successor_r_count].fillers), DEFAULT_SUCCESSORS_SET__SIZE);
	SET_ADD(p, &(c->successors[c->successor_r_count].fillers));

	// finally increment the r_count
	++c->successor_r_count;

	return 1;
}
