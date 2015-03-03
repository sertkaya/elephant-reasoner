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

// Preprocess axioms that are syntactic shortcuts, like equivalent classes/roles, disjoint classes and
// transtive roles. Translate them to subclass axioms for saturation.
void preprocess_tbox(KB* kb) {
	TBox* tbox = kb->tbox;

	// Initialize the hash of existential restrictions that are generated as a result of
	// translating role assertions.
	// TODO: think about the size. maybe take the number of target individuals?
	kb->generated_exists_restrictions = hash_map_create(DEFAULT_EXISTS_RESTRICTIONS_HASH_SIZE);

	// Convert equivalent classes axioms to subclass axioms
	SetIterator iterator;
	SET_ITERATOR_INIT(&iterator, &(tbox->equivalent_classes_axioms));
	void* ax = SET_ITERATOR_NEXT(&iterator);
	while (ax) {
		add_generated_subclass_axiom(kb, create_subclass_axiom(((EquivalentClassesAxiom*) ax)->lhs, ((EquivalentClassesAxiom*) ax)->rhs));
		add_generated_subclass_axiom(kb, create_subclass_axiom(((EquivalentClassesAxiom*) ax)->rhs, ((EquivalentClassesAxiom*) ax)->lhs));
		ax = SET_ITERATOR_NEXT(&iterator);
	}

	// Convert equivalent roles axioms to subrole axioms
	SET_ITERATOR_INIT(&iterator, &(tbox->equivalent_objectproperties_axioms));
	ax = SET_ITERATOR_NEXT(&iterator);
	while (ax) {
		add_generated_subrole_axiom(kb, create_subrole_axiom(((EquivalentObjectPropertiesAxiom*) ax)->lhs, ((EquivalentObjectPropertiesAxiom*) ax)->rhs));
		add_generated_subrole_axiom(kb, create_subrole_axiom(((EquivalentObjectPropertiesAxiom*) ax)->rhs, ((EquivalentObjectPropertiesAxiom*) ax)->lhs));
		ax = SET_ITERATOR_NEXT(&iterator);
	}

	// Process the transitive role axioms
	ObjectPropertyExpression* composition;
	SET_ITERATOR_INIT(&iterator, &(tbox->transitive_objectproperty_axioms));
	ax = SET_ITERATOR_NEXT(&iterator);
	while (ax) {
		composition = get_create_role_composition_binary(((TransitiveObjectPropertyAxiom*) ax)->r, ((TransitiveObjectPropertyAxiom*) ax)->r, tbox);
		add_generated_subrole_axiom(kb, create_subrole_axiom(composition, ((TransitiveObjectPropertyAxiom*) ax)->r));
		ax = SET_ITERATOR_NEXT(&iterator);
	}

	// Process the disjointclasses axioms.
	// We express that the concept pairs imply bottom.
	// Note that this generates n^2 new subclass axioms for a disjointness axiom with n concepts.
	// TODO: optimize!
	int i, j;
	ClassExpression* conjunction;
	SET_ITERATOR_INIT(&iterator, &(tbox->disjoint_classes_axioms));
	ax = SET_ITERATOR_NEXT(&iterator);
	while (ax) {
		for (i = 0; i < ((DisjointClassesAxiom*) ax)->classes.size - 1; ++i)
			for (j = i + 1; j < ((DisjointClassesAxiom*) ax)->classes.size ; ++j) {
				// Possibly a new conjunction is generated here, which modifies the model.
				// This is not a problem since the extended ontology (that is the ontology extended with the
				// new conjunction), will be indexed after the preprocessing step. So the new conjunction
				// will also be considered in the indexing information.
				conjunction = get_create_conjunction_binary(
						(ClassExpression*) ((DisjointClassesAxiom*) ax)->classes.elements[i],
						(ClassExpression*) ((DisjointClassesAxiom*) ax)->classes.elements[j], tbox);
				add_generated_subclass_axiom(kb, create_subclass_axiom(conjunction, tbox->bottom_concept));
			}
		ax = SET_ITERATOR_NEXT(&iterator);
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
