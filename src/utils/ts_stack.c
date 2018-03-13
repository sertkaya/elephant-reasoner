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

#include "ts_stack.h"

extern inline void ts_push(ThreadSafeStack*s, void* e);
extern inline void* ts_pop(ThreadSafeStack* s);

// void init_ts_stack(ThreadSafeStack* s, int capacity) {
void init_ts_stack(ThreadSafeStack* s) {
	s->size = 0;
	s->elements = NULL;
	atomic_flag_clear(&s->is_locked);
}


int reset_ts_stack(ThreadSafeStack* s) {
	free(s->elements);
	int freed_bytes =s->size * sizeof(void*);
	s->size = 0;
	s->elements = NULL;
	atomic_flag_clear(&s->is_locked);

	return freed_bytes;
}
