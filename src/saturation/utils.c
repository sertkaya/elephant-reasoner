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
int add_predecessor(ClassExpressionId c, ObjectPropertyExpressionId r, ClassExpressionId p, TBox* tbox) {

	// first check if we already have a link for role r
	int i;
	void* tmp;
	char added;
	for (i = 0; i < CEXP(c).predecessor_r_count; ++i)
		if (CEXP(c).predecessors[i].role == r) {
			// yes, we have a link for role r, now check if we already have p in its fillers list
			/*
			for (j = 0; j < CEXP(c).predecessors[i].filler_count; ++j)
				if (CEXP(c).predecessors[i].fillers[j] == p)
					return 0;
			// no we do not have p in this list, add it
			tmp = realloc(CEXP(c).predecessors[i].fillers, (CEXP(c).predecessors[i].filler_count + 1) * sizeof(ClassExpressionId));
			assert(tmp != NULL);
			CEXP(c).predecessors[i].fillers = (ClassExpressionId*) tmp;
			CEXP(c).predecessors[i].fillers[CEXP(c).predecessors[i].filler_count] = p;
			++CEXP(c).predecessors[i].filler_count;
			return 1;
			 */
			return(SET_ADD(p, &(CEXP(c).predecessors[i].fillers)));
		}
	// no, we do not already have a link for role r, create it, add p to its filler list
	// 1) extend the list for links
	tmp = realloc(CEXP(c).predecessors, (CEXP(c).predecessor_r_count + 1) * sizeof(Link));
	assert(tmp != NULL);
	CEXP(c).predecessors = (Link*) tmp;
	// 2) fill the fields of the new link
	CEXP(c).predecessors[CEXP(c).predecessor_r_count].role = r;
	/*
	CEXP(c).predecessors[CEXP(c).predecessor_r_count].fillers = calloc(1, sizeof(ClassExpressionId));
	assert(CEXP(c).predecessors[CEXP(c).predecessor_r_count].fillers != NULL);
	CEXP(c).predecessors[CEXP(c).predecessor_r_count].fillers[0] = p;
	CEXP(c).predecessors[CEXP(c).predecessor_r_count].filler_count = 1;
	*/
	SET_INIT(&(CEXP(c).predecessors[CEXP(c).predecessor_r_count].fillers), DEFAULT_PREDECESSORS_SET__SIZE);
	SET_ADD(p, &(CEXP(c).predecessors[CEXP(c).predecessor_r_count].fillers));

	// finally increment the r_count
	++(CEXP(c).predecessor_r_count);

	return 1;
}


int add_successor(ClassExpressionId c, ObjectPropertyExpressionId r, ClassExpressionId p, TBox* tbox) {

	// first check if we already have a link for role r
	int i;
	void* tmp;
	for (i = 0; i < CEXP(c).successor_r_count; ++i)
		if (CEXP(c).successors[i].role == r) {
			// yes, we have a link for role r, now check if we already have p in its fillers list
			/*
			for (j = 0; j < CEXP(c).successors[i].filler_count; ++j)
				if (CEXP(c).successors[i].fillers[j] == p)
					return 0;
			// no we do not have p in this list, add it
			tmp = realloc(CEXP(c).successors[i].fillers, (CEXP(c).successors[i].filler_count + 1) * sizeof(ClassExpressionId));
			assert(tmp != NULL);
			CEXP(c).successors[i].fillers = (ClassExpressionId*) tmp;
			CEXP(c).successors[i].fillers[CEXP(c).successors[i].filler_count] = p;
			++CEXP(c).successors[i].filler_count;
			return 1;
			*/
			return SET_ADD(p, &(CEXP(c).successors[i].fillers));
		}
	// no, we do not already have a link for role r, create it, add p to its filler list
	// 1) extend the list for links
	tmp = realloc(CEXP(c).successors, (CEXP(c).successor_r_count + 1) * sizeof(Link));
	assert(tmp != NULL);
	CEXP(c).successors = (Link*) tmp;
	// 2) fill the fields of the new link
	CEXP(c).successors[CEXP(c).successor_r_count].role = r;
	/*
	CEXP(c).successors[CEXP(c).successor_r_count].fillers = calloc(1, sizeof(ClassExpressionId));
	assert(CEXP(c).successors[CEXP(c).successor_r_count].fillers != NULL);
	CEXP(c).successors[CEXP(c).successor_r_count].fillers[0] = p;
	CEXP(c).successors[CEXP(c).successor_r_count].filler_count = 1;
	*/
	SET_INIT(&(CEXP(c).successors[CEXP(c).successor_r_count].fillers), DEFAULT_SUCCESSORS_SET__SIZE);
	SET_ADD(p, &(CEXP(c).successors[CEXP(c).successor_r_count].fillers));

	// finally increment the r_count
	++(CEXP(c).successor_r_count);

	return 1;
}
