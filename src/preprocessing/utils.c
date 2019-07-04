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
#include <string.h>

#include "utils.h"
#include "../model/model.h"
#include "../model/datatypes.h"
#include "../model/limits.h"
#include "../hashing/utils.h"

ClassExpressionId get_create_generated_nominal(TBox* tbox, IndividualId ind_id) {
	ClassExpressionId id;

	// check if the nominal with this individual already exists
	if ((id = GET_GENERATED_NOMINAL(ind_id, tbox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// a class with the IRI not found, create it
	// first create a class expression template
	id  = create_class_expression_template(tbox);

	tbox->class_expressions[id].type = OBJECT_ONE_OF_TYPE;

	tbox->class_expressions[id].description.nominal.individual = ind_id;

	PUT_GENERATED_NOMINAL(ind_id, id, tbox);

	return(id);
}


// get or create generated the existential restriction with role r and filler f
ClassExpressionId get_create_generated_exists_restriction(TBox* tbox, ObjectPropertyExpressionId r, ClassExpressionId f) {
	ClassExpressionId id;

	// first check if we already created an existential
	// restriction with the same role and filler
	if ((id = GET_GENERATED_EXISTS_RESTRICTION(r, f, tbox)) != KEY_NOT_FOUND_IN_HASH_MAP)
		return(id);

	// if it does not already exist, create it
	// first create a class expression template
	id  = create_class_expression_template(tbox);

	tbox->class_expressions[id].type = OBJECT_SOME_VALUES_FROM_TYPE;

	tbox->class_expressions[id].description.exists.role = r;
	tbox->class_expressions[id].description.exists.filler = f;

	PUT_GENERATED_EXISTS_RESTRICTION(r, f, id, tbox);

	return(id);
}
