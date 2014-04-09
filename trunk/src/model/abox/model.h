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


#ifndef ABOX_MODEL_H_
#define ABOX_MODEL_H_

#include "datatypes.h"
#include "../datatypes.h"


/******************************************************************************
 * get/create functions
 *****************************************************************************/

// returns the individual with given name or creates if it does not
// already exist
Individual* get_create_individual(char* name, ABox* abox);

/******************************************************************************
 * create functions for assertions
 *****************************************************************************/
// create the concept assertion axiom with the given concept description and individual
ConceptAssertion* create_concept_assertion(Individual* individual, Concept* concept);


/******************************************************************************
 * add functions for assertions
*****************************************************************************/
// add a given concept assertion to the ABox
void add_concept_assertion(ConceptAssertion* as, ABox* abox);

#endif
