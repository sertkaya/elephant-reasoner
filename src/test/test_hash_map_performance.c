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
#include <sys/time.h>

#include "../hashing/hash_map.h"
#include "../utils/timer.h"


int main(int argc, char *argv[]) {
	struct timeval start_time, stop_time;
	uint64_t i;
	int test_size = atoi(argv[1]);
	int hash_map_size = atoi(argv[2]);

	uint64_t* tmp = malloc(test_size * sizeof(uint64_t));
	assert(tmp != NULL);

	HashMap* hash_map = hash_map_create(hash_map_size);
	printf("%d put operations .........................: ", test_size);
	fflush(stdout);
	START_TIMER(start_time);
	for (i = 0; i < test_size; ++i) {
		tmp[i] = i;
		hash_map_put(hash_map,  i, (void*) tmp[i]);
	}
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);

	HashMapElement* e = HASH_MAP_LAST_ELEMENT(hash_map);
	printf("Iterating .........................: ");
	fflush(stdout);
	START_TIMER(start_time);
	while (e) {
		e = HASH_MAP_PREVIOUS_ELEMENT(e);
	}
	STOP_TIMER(stop_time);
	printf("done in %.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);


	return 1;
}


