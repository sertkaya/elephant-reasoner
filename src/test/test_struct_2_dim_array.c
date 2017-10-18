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
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>


int main(int argc, char *argv[]) {

	struct element {
		uint64_t key;
		void* value;
		struct element* previous;
	};


	struct element** arr = calloc(100, sizeof(struct element*));
	assert(arr != NULL);

	struct element* tmp = (struct element*) realloc(arr[0], 1 * sizeof(struct element));
	assert(tmp != NULL);
	arr[0] = tmp;

	arr[0][0].key = 0;
	arr[0][0].value = 0;
	arr[0][0].previous = NULL;

	int i;
	for (i = 1; i < 10; ++i) {
		printf("%d\n", i);
		tmp = (struct element*) realloc(arr[0], (i + 1) * sizeof(struct element));
		assert(tmp != NULL);
		arr[0] = tmp;
		arr[0][i].key = i;
		arr[0][i].value = (void*) (2 * i);
		arr[0][i].previous = &arr[0][i - 1];
	}

	struct element* e = &arr[0][9];
	while (e) {
		printf("%" PRIu64 "\t%p\n", e->key, e->value);
		e = e->previous;
	}

	return 1;
}


