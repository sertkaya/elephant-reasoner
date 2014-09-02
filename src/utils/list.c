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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

inline List* list_create() {
	List* l = (List*) malloc(sizeof(List));
	assert(l != NULL);
 	l->size = 0;
	l->elements = NULL;

	return l;
}

inline int list_free(List* l) {
	free(l->elements);
	int freed_bytes = l->size * sizeof(void*);
	free(l);
	freed_bytes+= sizeof(List);

	return freed_bytes;
}

inline char list_add(void* e, List* l) {
	void** tmp = realloc(l->elements, (l->size + 1) * sizeof(void*));
	assert(tmp != NULL);
	l->elements = tmp;
	l->elements[l->size] = e;
	++l->size;

	return 1;
}

/**
 * TODO
 */
inline char list_remove(void* e, List* l) {

	return 0;
}

inline ListIterator* list_iterator_create(List* l) {
	ListIterator* it = (ListIterator*) malloc(sizeof(ListIterator));
	assert(it != NULL);
	it->list = l;
	it->current_index = 0;

	return it;
}

inline void* list_iterator_next(ListIterator* it) {
	void* next = (it->current_index == it->list->size) ? NULL : it->list->elements[it->current_index];
	++it->current_index;

	return next;
}

inline int list_iterator_free(ListIterator* it) {
	free(it);

	return sizeof(ListIterator);
}