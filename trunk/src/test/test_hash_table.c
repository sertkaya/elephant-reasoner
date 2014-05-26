/*
 * The Elephant Reasoner
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

#include "../hashing/key_hash_table.h"

int main(int argc, char *argv[]) {
	KeyHashTable* hash_table = create_key_hash_table(10);

	int i;
	for (i = 0; i < 100; ++i)
		insert_key(hash_table, i);

	remove_key(hash_table, 3);
	remove_key(hash_table, 31);
	remove_key(hash_table, 45);
	remove_key(hash_table, 87);
	remove_key(hash_table, 187);

	for (i = 0; i < 100; ++i)
		if (!contains_key(hash_table, i))
			printf("Does not contain %i\n", i);

	return 1;
}


