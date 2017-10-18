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

#include "../hashing/hash_table.h"

int main(int argc, char *argv[]) {

	HashTable* hash_table = hash_table_create(20);

	int i;
	void* tmp[100];
	for (i = 0; i < 100; ++i) {
		tmp[i] = malloc(sizeof(void));
		assert(tmp[i] != NULL);
		hash_table_insert(hash_table, tmp[i]);
	}

	hash_table_remove(hash_table, tmp[10]);


	/*
	for (i = 0; i < 100; ++i)
		if (!hash_table_contains(hash_table, tmp[i]))
			printf("%d: not found!\n", i);
			*/

	HashTableIterator* it = hash_table_iterator_create(hash_table);
	void* e = hash_table_iterator_next(it);
	while (e != NULL) {
		printf("%p\n", e);
		e = hash_table_iterator_next(it);
	}

	return 1;
}


