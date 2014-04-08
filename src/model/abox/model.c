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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "datatypes.h"

/******************************************************************************
 * get/create functions for concepts
 *****************************************************************************/

// TODO: stub implementation!
Individual* get_create_individual(char* name, ABox* abox) {
	Individual* i;
	Individual** tmp;

	// check if an individual with this name already exists
	if ((i = get_individual((unsigned char*) name, abox)) != NULL)
		return i;

	// if an individual with the name does not already exist, create it
	i = (Individual*) malloc(sizeof(Individual));
	assert(i != NULL);

	i->name = (char*) malloc((strlen(name) + 1) * sizeof(char));
	assert(i->name != NULL);

	strcpy(i->name, name);


	return i;
}
