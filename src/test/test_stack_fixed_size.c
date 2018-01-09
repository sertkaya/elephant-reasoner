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
#include <assert.h>
#include "../utils/stack_fixed_size.h"
#include "../utils/timer.h"

char is_prime_naive(long x) {
	char flag;
	for (int i = 2; i < x ; ++i)
		if ((x % i) == 0)
			flag = 0;
	flag = 1;
	return(flag);
}

int main(int argc, char* argv[]) {
	struct timeval start_time, stop_time;
	FixedSizeStack stack;

	init_stack_fixed_size(&stack, 100000000);
	START_TIMER(start_time);
	for (long i = 1; i < 90000000; ++i)
		push_stack_fixed_size(&stack, (void*) i);

	void* data = pop_stack_fixed_size(&stack);
	int counter = 0;
	while (data != NULL) {
		// ++counter;
		data = pop_stack_fixed_size(&stack);
		is_prime_naive((long) data);
	}
	// printf("Counter:%d\n", counter);

	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);

    return(0);
}
