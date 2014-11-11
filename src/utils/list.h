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
inline List* list_create();

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
inline char list_add(void* e, List* l);

/**
 * Removes the first occurrence of the element e if it is present. The list stays unchanged
 * if e does not occur in l.
 * Returns 1 if e is removed, 0 otherwise.
 */
inline char list_remove(void* e, List* l);

/**
 * Free the space allocated for this list.
 */
inline int list_free(List* l);

/**
 * Free the space allocated for the elements of the given list.
 */
inline int list_reset(List* l);

/**
 * Creates an iterator for the given list.
 * Returns the created iterator.
 */
inline ListIterator* list_iterator_create(List* l);


/**
 * Returns the next element of the given iterator.
 * NULL if there is no next element.
 */
inline void* list_iterator_next(ListIterator* it);

#endif
