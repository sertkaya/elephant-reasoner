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

#ifndef SRC_UTILS_LINKED_LIST_H_
#define SRC_UTILS_LINKED_LIST_H_

#include <stdlib.h>
#include <assert.h>

typedef struct node Node;
typedef struct  linked_list LinkedList;
typedef struct  linked_list_iterator LinkedListIterator;

struct node {
	void *content;
	Node *next;
};

struct linked_list {
	Node *head;
	Node *tail;
};

struct linked_list_iterator {
	Node *current_node;
};

LinkedList *linked_list_create();

LinkedListIterator *linked_list_iterator_create(LinkedList* ll);

int linked_list_free(LinkedList* ll);

static inline Node *linked_list_append(LinkedList *ll, void *p) {
	Node *new_node  = malloc(sizeof(Node));
	assert(new_node != NULL);

	new_node->content = p;
	new_node->next = NULL;

	if (ll->tail == NULL)
		ll->head = ll->tail = new_node;
	else {
		ll->tail->next = new_node;
		ll->tail = new_node;
	}

	return(new_node);
}

#define LINKED_LIST_ITERATOR_HAS_NEXT(it) !(it->current_node->next == NULL)

static inline Node *linked_list_iterator_next(LinkedListIterator *it) {
	if (it == NULL || it->current_node == NULL)
		return(NULL);
	Node *tmp = it->current_node;
	it->current_node = it->current_node->next;
	return(tmp);
}

#endif /* SRC_UTILS_LINKED_LIST_H_ */
