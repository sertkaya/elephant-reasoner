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
#include <assert.h>

#include "stack_fixed_size.h"

extern inline void push_stack_fixed_size(FixedSizeStack*s, void* e);
extern inline void* pop_stack_fixed_size(FixedSizeStack* s);

void init_stack_fixed_size(FixedSizeStack* s, int capacity) {
	s->size = 0;
	s->elements = malloc(sizeof(void*) * capacity);
	assert (s->elements != NULL);
}

