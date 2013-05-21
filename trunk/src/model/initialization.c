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

/*
 * Obsolete!
 *
#include <Judy.h>
#include <assert.h>

#include "model.h"

void init_reasoner(TBox* tbox) {

	tbox = (TBox* ) malloc(sizeof(TBox));
	assert(tbox != NULL);

	tbox->last_concept_id = 0;
	tbox->last_role_id = 0;

	tbox->atomic_concept_count = 0;
	tbox->atomic_concept_list = NULL;
	tbox->atomic_concepts = (Pvoid_t) NULL;

	tbox->atomic_role_count = 0;
	tbox->atomic_roles = (Pvoid_t) NULL;

	tbox->exists_restriction_count = 0;
	tbox->unique_exists_restriction_count = 0;
	tbox->exists_restrictions = (Pvoid_t) NULL;

	tbox->conjunction_count = 0;
	// tbox->unique_conjunction_count = 0;
	tbox->binary_conjunction_count = 0;
	tbox->unique_binary_conjunction_count = 0;
	tbox->conjunctions = (Pvoid_t) NULL;

	tbox->role_composition_count = 0;
	tbox->binary_role_composition_count = 0;
	tbox->unique_binary_role_composition_count = 0;

	tbox->subclass_axioms = NULL;
	tbox->subclass_axiom_count = 0;
	tbox->eqclass_axioms = NULL;
	tbox->eqclass_axiom_count = 0;
	tbox->subrole_axioms = NULL;
	tbox->subrole_axiom_count = 0;
}
*/
