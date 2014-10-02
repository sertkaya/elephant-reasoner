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

#include "../utils/set.h"

int main(int argc, char *argv[]) {

	Set* set = SET_CREATE(20);

	int i;
	void* tmp[100];
	for (i = 0; i < 100; ++i) {
		tmp[i] = malloc(sizeof(void));
		assert(tmp[i] != NULL);
		printf("%d: add %p\n", i, tmp[i]);
		SET_ADD(tmp[i], set);
	}

	SET_REMOVE(tmp[10], set);


	for (i = 0; i < 100; ++i)
		if (!SET_CONTAINS(tmp[i], set))
			printf("%d: not found!\n", i);
		else
			printf("%d: found!\n", i);

	SetIterator* it = SET_ITERATOR_CREATE(set);
	void* e = SET_ITERATOR_NEXT(it);
	while (e != NULL) {
		printf("%p\n", e);
		e = SET_ITERATOR_NEXT(it);
	}
	SET_ITERATOR_FREE(it);

	return 1;
}


