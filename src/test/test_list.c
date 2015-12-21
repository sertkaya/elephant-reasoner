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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../utils/list.h"

int main(int argc, char *argv[]) {

	List list;
	LIST_INIT(&list);

	int i;
	void* tmp[100];
	for (i = 0; i < 100; ++i) {
		tmp[i] = malloc(sizeof(void));
		assert(tmp[i] != NULL);
		list_add(tmp[i], &list);
	}

	list_remove(tmp[10], &list);
	list_remove(tmp[99], &list);

	for (i = 0; i < list.size; ++i) {
		printf("%d ", ((int*) list.elements)[i]);
	}
	return 1;
}


