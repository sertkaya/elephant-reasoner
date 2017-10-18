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
 */

#ifndef LIST_H_
#define LIST_H_

typedef struct list List;
typedef struct list_iterator ListIterator;

struct list {
	int size;
	void** elements;
};

struct list_iterator {
	List* list;
	int current_index;
};

/**
 * Create list.
 * Returns a pointer to the created list.
 */
List* list_create();

/**
 * Initialize a given list. Intended for not dynamically created
 * lists.
 */
// inline void list_init(List* l);
#define LIST_INIT(l)		do {((List*) l)->size=0; ((List*) l)->elements=NULL;} while(0)
/**
 * Appends element e to list l. Does not check for duplicates.
 * Returns 1.
 */
inline char list_add(void* e, List* l) {
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
inline char list_remove(void* e, List* l) {
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
 * Free the space allocated for this list.
 */
int list_free(List* l);

/**
 * Free the space allocated for the elements of the given list.
 */
int list_reset(List* l);

/**
 * Creates an iterator for the given list.
 * Returns the created iterator.
 */
ListIterator* list_iterator_create(List* l);


/**
 * Returns the next element of the given iterator.
 * NULL if there is no next element.
 */
inline void* list_iterator_next(ListIterator* it) {
	void* next = (it->current_index == it->list->size) ? NULL : it->list->elements[it->current_index];
	++it->current_index;

	return next;
}
#endif
