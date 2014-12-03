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

#include <stdio.h>
#include "../model/datatypes.h"
#include "../model/model.h"
#include "../model/limits.h"
#include "../hashing/hash_map.h"
#include "utils.h"

/*
// The list of subclass axioms that are generated during  preprocessing
SubClassAxiom** generated_subclass_axioms = NULL;
int generated_subclass_axiom_count = 0;

// The list of subrole axioms that are generated during  preprocessing
SubRoleAxiom** generated_subrole_axioms = NULL;
int generated_subrole_axiom_count = 0;

// The hash of nominals that are generated during preprocessing.
KeyValueHashTable* generated_nominals = NULL;
// id of the last generated nominal
// int last_generated_nominal_id = 0;

// The hash of existential restrictions that are generated during preprocessing.
// They are generated from preprocessing role assertions.
KeyValueHashTable* generated_exists_restrictions = NULL;
int generated_exists_restriction_count = 0;
*/

// Preprocess axioms that are syntactic shortcuts, like equivalent classes/roles, disjoint classes and
// transtive roles. Translate them to subclass axioms for saturation.
void preprocess_tbox(KB* kb) {
	TBox* tbox = kb->tbox;

	// Initialize the hash of generated nominals
	// TODO: think about the size. maybe take the number of individuals?
	kb->generated_nominals = hash_map_create(DEFAULT_NOMINALS_HASH_SIZE);

	// Initialize the hash of existential restrictions that are generated as a result of
	// translating role assertions.
	// TODO: think about the size. maybe take the number of target individuals?
	kb->generated_exists_restrictions = hash_map_create(DEFAULT_EXISTS_RESTRICTIONS_HASH_SIZE);

	int i;

	// Convert equivalent classes axioms to subclass axioms
	for (i = 0; i < tbox->eqclass_axiom_count; ++i) {
		add_generated_subclass_axiom(kb, create_subclass_axiom(tbox->eqclass_axioms[i]->lhs, tbox->eqclass_axioms[i]->rhs));
		add_generated_subclass_axiom(kb, create_subclass_axiom(tbox->eqclass_axioms[i]->rhs, tbox->eqclass_axioms[i]->lhs));
	}

	// Convert equivalent roles axioms to subrole axioms
	for (i = 0; i < tbox->eqrole_axiom_count; ++i) {
		add_generated_subrole_axiom(kb, create_subrole_axiom(tbox->eqrole_axioms[i]->lhs, tbox->eqrole_axioms[i]->rhs));
		add_generated_subrole_axiom(kb, create_subrole_axiom(tbox->eqrole_axioms[i]->rhs, tbox->eqrole_axioms[i]->lhs));
	}

	// Process the transitive role axioms
	ObjectPropertyExpression* composition;
	for (i = 0; i < tbox->transitive_role_axiom_count; ++i) {
		composition = get_create_role_composition_binary(tbox->transitive_role_axioms[i]->r, tbox->transitive_role_axioms[i]->r, tbox);
		add_generated_subrole_axiom(kb, create_subrole_axiom(composition, tbox->transitive_role_axioms[i]->r));
	}

	// Process the disjointclasses axioms.
	// We express that the concept pairs imply bottom.
	// Note that this generates n^2 new subclass axioms for a disjointness axiom with n concepts.
	// TODO: optimize!
	int j, k;
	ClassExpression* conjunction;
	for (i = 0; i < tbox->disjointclasses_axiom_count; ++i)
		for (j = 0; j < tbox->disjointclasses_axioms[i]->classes.size - 1; ++j)
			for (k = j + 1; k < tbox->disjointclasses_axioms[i]->classes.size ; ++k) {
				// TODO: what about the conjunction created here? It modifies the model!
				conjunction = get_create_conjunction_binary(
						(ClassExpression*) tbox->disjointclasses_axioms[i]->classes.elements[j],
						(ClassExpression*) tbox->disjointclasses_axioms[i]->classes.elements[k], tbox);
				add_generated_subclass_axiom(kb, create_subclass_axiom(conjunction, tbox->bottom_concept));
			}
}

// Preprocess assertions and translate them to subclass axioms for saturation. ABox individuals are translated to
// nominals for this.
void preprocess_abox(KB* kb) {
	ABox* abox = kb->abox;

	// Translate the concept assertions to subclass axioms.
	// Individuals translated to generated nominals.
	int i;
	for (i = 0; i < abox->concept_assertion_count; ++i)
		add_generated_subclass_axiom(
				kb,
				create_subclass_axiom(
						get_create_generated_nominal(kb, abox->concept_assertions[i]->individual), abox->concept_assertions[i]->concept));

	// Translate the role assertions to subclass axioms.
	// Individuals translated to generated nominals.
	for (i = 0; i < abox->role_assertion_count; ++i)
		add_generated_subclass_axiom(
				kb,
				create_subclass_axiom(
						get_create_generated_nominal(kb, abox->role_assertions[i]->source_individual),
						get_create_generated_exists_restriction(kb, abox->role_assertions[i]->role,
								get_create_generated_nominal(kb, abox->role_assertions[i]->target_individual)
						)
				)
		);
}

void preprocess_kb(KB* kb) {
	preprocess_tbox(kb);
	preprocess_abox(kb);
}
