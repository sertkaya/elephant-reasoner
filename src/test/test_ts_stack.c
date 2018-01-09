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
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include "../utils/ts_stack.h"
#include "../utils/timer.h"

struct thread_data {
	ThreadSafeStack* stack;
	int thread_id;
};
typedef struct thread_data ThreadData;

// Just to waste CPU time ;)
char is_prime_naive(long x) {
	char flag;
	for (int i = 2; i < x ; ++i)
		if ((x % i) == 0)
			flag = 0;
	flag = 1;
	return(flag);
}

static void* worker(void* wd) {
	ThreadSafeStack* stack = ((ThreadData*) wd)->stack;
	pthread_mutex_t* mutex = &(stack->mutex);
	pthread_mutex_lock(mutex);
	void* data = ts_pop(stack);
	pthread_mutex_unlock(mutex);
	while (data != NULL) {
		pthread_mutex_lock(mutex);
		data =ts_pop(stack);
		pthread_mutex_unlock(mutex);
		is_prime_naive((long) data);
	}

	return(NULL);
}

int main(int argc, char* argv[]) {
	struct timeval start_time, stop_time;

	int nthreads = sysconf(_SC_NPROCESSORS_ONLN);

	ThreadSafeStack stack;

	// init_ts_stack(&stack, 100000000);
	init_ts_stack(&stack );
	for (long i = 1; i < 90000000; ++i)
		ts_push(&stack, (void*) i);

	pthread_t threads[nthreads];
	printf("Using %d thread%s.\n", nthreads, nthreads == 1 ? "" : "s");


	START_TIMER(start_time);

	ThreadData td;
	td.stack = &stack;
	for (int i = 0; i < nthreads; i++)
		pthread_create(&threads[i], NULL, &worker, &td);

     for (int i = 0; i < nthreads; i++)
    	 pthread_join(threads[i], NULL);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);

    return(0);
}
