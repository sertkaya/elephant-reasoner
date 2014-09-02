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
#include <sys/time.h>

#include "../hashing/hash_table.h"
#include "../utils/timer.h"


int main(int argc, char *argv[]) {
	struct timeval start_time, stop_time;
	size_t i;
	int test_size = atoi(argv[1]);
	int hash_table_size = atoi(argv[2]);

	int* tmp = malloc(test_size * sizeof(int));
	assert(tmp != NULL);

	HashTable* hash_table = hash_table_create(hash_table_size);
	printf("%d insert operations: ", test_size);
	fflush(stdout);
	START_TIMER(start_time);
	for (i = 0; i < test_size; ++i) {
		tmp[i] = i+1;
		hash_table_insert(hash_table, (void*) tmp[i]);
	}
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);

	printf("Iterating over the hash table: ");
	fflush(stdout);
	START_TIMER(start_time);
	HashTableIterator* it = hash_table_iterator_create(hash_table);
	void* e = hash_table_iterator_next(it);
	while (e) {
		e = hash_table_iterator_next(it);
	}
	STOP_TIMER(stop_time);
	printf("done in %.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);

	printf("Iterating over the array: ");
	fflush(stdout);
	START_TIMER(start_time);
	int j;
	for (i = 0; i < test_size; ++i) {
		j = tmp[i];
	}
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);

	return 1;
}


