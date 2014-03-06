/*
 * test_hash_table.c
 *
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


