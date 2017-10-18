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

#include <stdlib.h>
#include <assert.h>

#include "list.h"

List* list_create() {
	List* l = (List*) malloc(sizeof(List));
	assert(l != NULL);
 	l->size = 0;
	l->elements = NULL;

	return l;
}

int list_free(List* l) {
	free(l->elements);
	int freed_bytes = l->size * sizeof(void*);
	free(l);
	freed_bytes+= sizeof(List);

	return freed_bytes;
}

int list_reset(List* l) {
	free(l->elements);
	int freed_bytes = l->size * sizeof(void*);
	l->size = 0;
	l->elements = NULL;

	return freed_bytes;
}

extern inline char list_add(void* e, List* l);


extern inline char list_remove(void* e, List* l);

ListIterator* list_iterator_create(List* l) {
	ListIterator* it = (ListIterator*) malloc(sizeof(ListIterator));
	assert(it != NULL);
	it->list = l;
	it->current_index = 0;

	return it;
}


extern inline void* list_iterator_next(ListIterator* it);

int list_iterator_free(ListIterator* it) {
	free(it);

	return sizeof(ListIterator);
}
