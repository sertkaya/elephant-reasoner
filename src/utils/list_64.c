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

#include "list_64.h"

List_64* list_create_64() {
	List_64* l = (List_64*) malloc(sizeof(List_64));
	assert(l != NULL);
 	l->size = 0;
	l->elements = NULL;

	return l;
}

int list_free_64(List_64* l) {
	free(l->elements);
	int freed_bytes = l->size * sizeof(void*);
	free(l);
	freed_bytes+= sizeof(List_64);

	return freed_bytes;
}

int list_reset_64(List_64* l) {
	free(l->elements);
	int freed_bytes = l->size * sizeof(void*);
	l->size = 0;
	l->elements = NULL;

	return freed_bytes;
}

extern inline char list_add_64(void* e, List_64* l);


extern inline char list_remove_64(void* e, List_64* l);

ListIterator_64* list_iterator_create_64(List_64* l) {
	ListIterator_64* it = (ListIterator_64*) malloc(sizeof(ListIterator_64));
	assert(it != NULL);
	it->list = l;
	it->current_index = 0;

	return it;
}


extern inline void* list_iterator_next_64(ListIterator_64* it);

int list_iterator_free_64(ListIterator_64* it) {
	free(it);

	return sizeof(ListIterator_64);
}
