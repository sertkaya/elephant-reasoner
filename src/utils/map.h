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


/**
 * A simple map interface. Just a macro for hash map.
 */

#ifndef MAP_H_
#define MAP_H_

#include "../hashing/hash_map.h"

typedef HashMap Map;
typedef HashMapIterator MapIterator;


/**
 * Create a map with an underlying hash map of the given size.
 * Returns the map created.
 */
#define MAP_CREATE(size)				hash_map_create(size)

/**
 * Initialize a map with an underlying hash map of the given size.
 */
#define MAP_INIT(map, size)				hash_map_init(map, size)

/**
 * Adds the pair (key,value) to the map m.
 * Returns 1 if e is successfully added, 0 otherwise.
 */
#define MAP_PUT(key, value, map)		hash_map_put(map, key, value)

/**
 * Removes the element with the given key if it is present. The map stays unchanged
 * if the key does not occur in map.
 * Returns 1 if key is removed, 0 otherwise.
 */
#define MAP_REMOVE(key, map)			hash_map_remove(key, map)

/**
 * Get the value associated with the given key.
 * Returns 1 if the key occurs, 0 otherwise
 */
#define MAP_GET(key, map)				hash_map_get(map, key)

/**
 * Free the space allocated for the given map.
 * Returns the number of freed bytes.
 */
#define MAP_FREE(map)					hash_map_free(map)

/**
 * Create an iterator for the elements of the map.
 * It is the iterator of the underlying hash map.
 */
#define MAP_ITERATOR_CREATE(map)		hash_map_iterator_create(map)

/**
 * Create an iterator for the elements of the map.
 * It is the iterator of the underlying hash map.
 */
#define MAP_ITERATOR_INIT(map_it, map)	hash_map_iterator_init(map_it, map)

/**
 * Get the next element in the map.
 * Gets the next element in the underlying hash map.
 * Elements are not necessarily returned in the order of addition.
 */
#define MAP_ITERATOR_NEXT(map_it)		hash_map_iterator_next(map_it)

/**
 * Free the space for the given map iterator.
 */
#define MAP_ITERATOR_FREE(map_it)		hash_map_iterator_free(map_it)

/*
#define MAP_FIRST_ELEMENT(map)			HASH_MAP_FIRST_ELEMENT(map)

#define MAP_NEXT_ELEMENT(current_element)		HASH_MAP_NEXT_ELEMENT(current_element)
*/

#endif
