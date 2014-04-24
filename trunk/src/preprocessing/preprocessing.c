#include <stdio.h>
#include "../model/datatypes.h"
#include "../model/model.h"
#include "../model/limits.h"
#include "../hashing/key_value_hash_table.h"
#include "utils.h"

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

// Preprocess axioms that are syntactic shortcuts, like equivalent classes/roles, disjoint classes and
// transtive roles. Translate them to subclass axioms for saturation.
void preprocess_tbox(TBox* tbox) {
	// Initialize the hash of generated nominals
	// TODO: think about the size. maybe take the number of individuals?
	generated_nominals = create_key_value_hash_table(DEFAULT_NOMINALS_HASH_SIZE);

	// Initialize the hash of existential restrictions that are generated as a result of
	// translating role assertions.
	// TODO: think about the size. maybe take the number of target individuals?
	generated_exists_restrictions = create_key_value_hash_table(DEFAULT_EXISTS_RESTRICTIONS_HASH_SIZE);

	int i;

	// Convert equivalent classes axioms to subclass axioms
	for (i = 0; i < tbox->eqclass_axiom_count; ++i) {
		add_generated_subclass_axiom(create_subclass_axiom(tbox->eqclass_axioms[i]->lhs, tbox->eqclass_axioms[i]->rhs));
		add_generated_subclass_axiom(create_subclass_axiom(tbox->eqclass_axioms[i]->rhs, tbox->eqclass_axioms[i]->lhs));
	}

	// Convert equivalent roles axioms to subrole axioms
	for (i = 0; i < tbox->eqrole_axiom_count; ++i) {
		add_generated_subrole_axiom(create_subrole_axiom(tbox->eqrole_axioms[i]->lhs, tbox->eqrole_axioms[i]->rhs));
		add_generated_subrole_axiom(create_subrole_axiom(tbox->eqrole_axioms[i]->rhs, tbox->eqrole_axioms[i]->lhs));
	}

	// Process the transitive role axioms
	Role* composition;
	for (i = 0; i < tbox->transitive_role_axiom_count; ++i) {
		composition = get_create_role_composition_binary(tbox->transitive_role_axioms[i]->r, tbox->transitive_role_axioms[i]->r, tbox);
		add_generated_subrole_axiom(create_subrole_axiom(composition, tbox->transitive_role_axioms[i]->r));
	}

	// Process the disjointclasses axioms.
	// We express that the concept pairs imply bottom.
	// Note that this generates n^2 new subclass axioms for a disjointness axiom with n concepts.
	// TODO: optimize!
	int j, k;
	Concept* conjunction;
	for (i = 0; i < tbox->disjointclasses_axiom_count; ++i)
		for (j = 0; j < tbox->disjointclasses_axioms[i]->concept_count - 1; ++j)
			for (k = j + 1; k < tbox->disjointclasses_axioms[i]->concept_count ; ++k) {
				conjunction = get_create_conjunction_binary(tbox->disjointclasses_axioms[i]->concepts[j], tbox->disjointclasses_axioms[i]->concepts[k], tbox);
				add_generated_subclass_axiom(create_subclass_axiom(conjunction, tbox->bottom_concept));
			}
}

// Preprocess assertions and translate them to subclass axioms for saturation. ABox individuals are translated to
// nominals for this.
void preprocess_abox(ABox* abox) {


	// Translate the concept assertions to subclass axioms.
	// Individuals translated to generated nominals.
	int i;
	for (i = 0; i < abox->concept_assertion_count; ++i)
		add_generated_subclass_axiom(
				create_subclass_axiom(
						get_create_generated_nominal(abox->concept_assertions[i]->individual), abox->concept_assertions[i]->concept));

	// Translate the role assertions to subclass axioms.
	// Individuals translated to generated nominals.
	for (i = 0; i < abox->role_assertion_count; ++i)
		add_generated_subclass_axiom(
				create_subclass_axiom(
						get_create_generated_nominal(abox->role_assertions[i]->source_individual),
						get_create_generated_exists_restriction(abox->role_assertions[i]->role,
								get_create_generated_nominal(abox->role_assertions[i]->target_individual)
						)
				)
		);
}
