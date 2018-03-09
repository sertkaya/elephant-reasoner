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

#include "linked_list.h"

LinkedList *linked_list_create() {
	LinkedList *ll = malloc(sizeof(LinkedList()));
	assert(ll != NULL);
	ll->head = NULL;
	ll->tail = NULL;
	return(ll);
}

LinkedListIterator *linked_list_iterator_create(LinkedList *ll) {
	LinkedListIterator *it = malloc(sizeof(LinkedListIterator));
	assert(it != NULL);
	it->current_node = ll->head;
	return(it);
}
