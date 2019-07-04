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


/**
 * A simple set implementation. Just a macro for hash table.
 */

#ifndef SET_H_
#define SET_H_

#include "../hashing/dynamic_hash_table.h"
#include "../hashing/dynamic_hash_table_64.h"

typedef DynamicHashTable Set;
typedef DynamicHashTable_64 Set_64;
typedef DynamicHashTableIterator SetIterator;
typedef DynamicHashTableIterator_64 SetIterator_64;


/**
 * Create a set with an underlying hash table of the given size.
 * Returns the set created
 */
#define SET_CREATE(size)		dynamic_hash_table_create(size)

/**
 * Initialize a set with an underlying hash table of the given size.
 */
#define SET_INIT(set, size)		dynamic_hash_table_init(set, size)
#define SET_INIT_64(set, size)		dynamic_hash_table_init_64(set, size)

/**
 * Adds element e to the set s. Duplicates are not allowed.
 * Returns 1 if e is successfully added, 0 otherwise.
 */
#define SET_ADD(e, s)			dynamic_hash_table_insert(e, s)
#define SET_ADD_64(e, s)			dynamic_hash_table_insert_64(e, s)

/**
 * Removes the element e if it is present. The set stays unchanged
 * if e does not occur in s.
 * Returns 1 if e is removed, 0 otherwise.
 */
#define SET_REMOVE(e, s)		dynamic_hash_table_remove(e, s)

/**
 * Checks if a given element exists.
 * Returns 1 if this is the case, 0 otherwise.
 */
#define SET_CONTAINS(e, s)		dynamic_hash_table_contains(e, s)

/**
 * Free the space allocated for the elements of the given set
 * and the set itself. Intended for freeing sets that are
 * dynamically created.
 * Returns the number of freed bytes.
 */
#define SET_FREE(s)				dynamic_hash_table_free(s)

/**
 * Free the space allocated for the elements of the given set.
 * The space for the set itself is not freed. Intended for
 * sets that are not dynamically created.
 * Returns the number of freed bytes.
 */
#define SET_RESET(s)			dynamic_hash_table_reset(s)
#define SET_RESET_64(s)			dynamic_hash_table_reset_64(s)

/**
 * Return an array containing the elements of the given set.
 * The caller is responsible for allocating and freeing the space.
 */
// void set_to_list(Set* s, void** l);

/**
 * Create an iterator for the elements of the set.
 * It is the iterator of the underlying hash table.
 */
#define SET_ITERATOR_CREATE(s)		dynamic_hash_table_iterator_create(s)

/**
 * Create an iterator for the elements of the set.
 * It is the iterator of the underlying hash table.
 */
#define SET_ITERATOR_INIT(it, s)	dynamic_hash_table_iterator_init(it, s)
#define SET_ITERATOR_INIT_64(it, s)	dynamic_hash_table_iterator_init_64(it, s)

/**
 * Get the next element in the set.
 * Gets the next element in the underlying hash table.
 * Elements are not necessarily returned in the order of addition.
 */
#define SET_ITERATOR_NEXT(si)			dynamic_hash_table_iterator_next(si)
#define SET_ITERATOR_NEXT_64(si)		dynamic_hash_table_iterator_next_64(si)

/**
 * Free the space for the given set iterator.
 */
#define SET_ITERATOR_FREE(si)		dynamic_hash_table_iterator_free(si)

#endif
