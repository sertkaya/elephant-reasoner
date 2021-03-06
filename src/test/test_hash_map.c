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

#include "../hashing/hash_map.h"

int main(int argc, char *argv[]) {

	HashMap* hash_table = hash_map_create(20);

	/*
	size_t i;
	int tmp[100];
	for (i = 0; i < 100; ++i) {
		tmp[i] = i;
		hash_map_put(hash_table,  i, (void*) tmp[i]);
	}

	tmp[21] = -1;

	for (i = 0; i < 100; ++i) {
		int val = (int) hash_map_get(hash_table,  i);
		if (val != tmp[i])
			printf("%d different!\n", i);
	}
	*/

	return 1;
}


