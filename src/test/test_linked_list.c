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
#include "../utils/linked_list.h"

int main(int argc, char *argv[]) {

	LinkedList *ll = linked_list_create();

	long int i;
	for (i = 0; i < 100; ++i) {
		Node *tmp = linked_list_append(ll, (void*) i);
		// printf("%p ", tmp->content);
	}

	LinkedListIterator *it = linked_list_iterator_create(ll);

	void *p;
	while ((p = linked_list_iterator_next(it)) != NULL) {
		printf("%p ", ((Node*) p)->content);
	}

	return(0);
}
