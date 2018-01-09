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

#include <stdlib.h>

#ifndef FIXED_SIZE_STACK_H_
#define FIXED_SIZE_STACK_H_

typedef struct fixed_size_stack FixedSizeStack;

struct fixed_size_stack {
	int size;
	void** elements;
};

void init_stack_fixed_size(FixedSizeStack* s, int capacity);

inline void push_stack_fixed_size(FixedSizeStack* s, void* e) {
	s->elements[s->size] = e;
	++s->size;
}

inline void* pop_stack_fixed_size(FixedSizeStack* s) {
	void* e;

	if (s->size == 0)
		return NULL;

	--s->size;
	e = s->elements[s->size];

	return e;
}
#endif /* STACK_H_ */
