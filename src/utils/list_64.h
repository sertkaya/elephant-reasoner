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


/**
 * A simple list implementation that supports adding and removing elements.
 * Elements are 64 bit pointers
 */

#ifndef LIST_64_H_
#define LIST_64_H_

#include <assert.h>
#include <stdlib.h>

typedef struct list_64 List_64;
typedef struct list_iterator_64 ListIterator_64;

struct list_64 {
	int size;
	void** elements;
};

struct list_iterator_64 {
	List_64* list;
	int current_index;
};

/**
 * Create list.
 * Returns a pointer to the created list_64.
 */
List_64* list_create_64();

/**
 * Initialize a given list. Intended for not dynamically created
 * lists.
 */
// inline void list_init(List* l);
#define LIST_INIT_64(l)		do {((List_64*) l)->size=0; ((List*) l)->elements=NULL;} while(0)
/**
 * Appends element e to list l. Does not check for duplicates.
 * Returns 1.
 */
inline char list_add_64(void* e, List_64* l) {
	void** tmp = realloc(l->elements, (l->size + 1) * sizeof(void*));
	assert(tmp != NULL);
	l->elements = tmp;
	l->elements[l->size] = e;
	++l->size;

	return 1;
}

/**
 * Removes the first occurrence of the element e if it is present. The list stays unchanged
 * if e does not occur in l.
 * Returns 1 if e is removed, 0 otherwise.
 */
/**
 * TODO
 */
inline char list_remove_64(void* e, List_64* l) {
	int i;

	for (i = 0; i < l->size; ++i) {
		if (e == l->elements[i])
			// the element is at index i
			break;
	}
	// if we reached the last element and it is not e,
	// then e does not exist in l.
	if ((i == l->size - 1) && (l->elements[i] != e))
		return 1;
	// now shift the elements, overwrite index i
	int j;
	for (j = i; j < l->size - 1; ++j) {
		l->elements[j] = l->elements[j + 1];
	}
	// shrink the allocated space
	void** tmp = realloc(l->elements, (l->size - 1) * sizeof(void*));
	assert(l->size == 1 || tmp != NULL);
	l->elements = tmp;
	// decrement the element count
	--l->size;
	return 0;
}
/**
 * Free the space allocated for this list_64.
 */
int list_free_64(List_64* l);

/**
 * Free the space allocated for the elements of the given list_64.
 */
int list_reset_64(List_64* l);

/**
 * Creates an iterator for the given list.
 * Returns the created iterator.
 */
ListIterator_64* list_iterator_create_64(List_64* l);


/**
 * Returns the next element of the given iterator.
 * NULL if there is no next element.
 */
inline void* list_iterator_next_64(ListIterator_64* it) {
	void* next = (it->current_index == it->list->size) ? NULL : it->list->elements[it->current_index];
	++it->current_index;

	return next;
}

int list_iterator_free_64(ListIterator_64* it);

#endif
