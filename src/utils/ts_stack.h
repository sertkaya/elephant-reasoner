
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

#ifndef SRC_UTILS_TS_STACK_H_
#define SRC_UTILS_TS_STACK_H_

#include <assert.h>
#include <stdlib.h>
#include <stdatomic.h>

typedef struct ts_stack ThreadSafeStack;

struct ts_stack {
	int size;
	void** elements;
	// True if the stack is being accessed, false otherwise
	volatile atomic_flag is_locked;
};

// void init_ts_stack(ThreadSafeStack* s, int capacity);
void init_ts_stack(ThreadSafeStack* s);

int reset_ts_stack(ThreadSafeStack* s);

inline void ts_push(ThreadSafeStack* s, void* e) {
	// Busy-wait if already locked
	// while (atomic_flag_test_and_set(&s->is_locked));

	void** tmp = realloc(s->elements, (s->size + 1) * sizeof(void*));
	assert(tmp != NULL);
	s->elements = tmp;

	s->elements[s->size] = e;
	++(s->size);
	// atomic_flag_clear(&s->is_locked);
}

inline void* ts_pop(ThreadSafeStack* s) {
	// Busy-wait if already locked
	// while (atomic_flag_test_and_set(&s->is_locked));

	void* e;
	void **tmp;

	if (s->size == 0)
		return NULL;

	--(s->size);
	e = s->elements[s->size];
	tmp = realloc(s->elements, (s->size) * sizeof(void*));
	assert(tmp != NULL || s->size == 0);
	s->elements = tmp;
	// atomic_flag_clear(&s->is_locked);

	return e;
}

#endif /* SRC_UTILS_TS_STACK_H_ */
