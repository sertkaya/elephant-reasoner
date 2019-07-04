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


#ifndef PREPROCESSING_UTILS_H_
#define PREPROCESSING_UTILS_H_

#include "../hashing/utils.h"
#include "../utils/map.h"
#include "../model/datatypes.h"

// return the generated nominal with the given individual if it exists
// NULL if it does not exist
#define GET_GENERATED_NOMINAL(ind_id,tbox)		MAP_GET(ind_id,&(tbox->generated_object_one_of_exps))

// insert the generated nominal with the given individual
#define PUT_GENERATED_NOMINAL(ind_id,n_id,tbox)		MAP_PUT(ind_id,n_id,&(tbox->generated_object_one_of_exps))

// get the existential restriction with role r and filler f from hash
#define GET_GENERATED_EXISTS_RESTRICTION(role_id,filler_id,tbox)		MAP_GET(HASH_INTEGERS(role_id,filler_id),&(tbox->generated_object_some_values_from_exps))

// put the existential restriction with role r and filler f into hash
#define PUT_GENERATED_EXISTS_RESTRICTION(role_id,filler_id,c,tbox)		MAP_PUT(HASH_INTEGERS(role_id,filler_id),c,&(tbox->generated_object_some_values_from_exps))

/******************************************************************************
 * add functions for generated axioms
*****************************************************************************/
// add a given subclass axiom to a given ontology
#define ADD_GENERATED_SUBCLASS_OF_AXIOM(ax, tbox)						SET_ADD_64(ax, &(tbox->generated_subclass_of_axioms))

// add a given subobjectpropertyof axiom to a given ontology
#define ADD_GENERATED_SUBOBJECTPROPERTY_OF_AXIOM(ax, tbox)			SET_ADD_64(ax, &(tbox->generated_subobjectproperty_of_axioms))

ClassExpressionId get_create_generated_nominal(TBox* tbox, IndividualId ind);

ClassExpressionId get_create_generated_exists_restriction(TBox* tbox, ObjectPropertyExpressionId r, ClassExpressionId f);

#endif
