/*
 * The Elephant Reasoner
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

#include <stdlib.h>
#include <assert.h>

#include "stack.h"

void init_stack(Stack* s) {
	s->size = 0;
	s->elements = NULL;
}

void push(Stack* s, void* e) {
	void** tmp = realloc(s->elements, (s->size + 1) * sizeof(void*));
	assert(tmp != NULL);
	s->elements = tmp;
	s->elements[s->size] = e;
	++s->size;
}

void* pop(Stack* s) {
	void* e;
	void** tmp;

	if (s->size == 0)
		return NULL;

	--s->size;
	e = s->elements[s->size];
	tmp = realloc(s->elements, (s->size) * sizeof(void*));
	assert(tmp != NULL || s->size == 0);
	s->elements = tmp;

	return e;
}
