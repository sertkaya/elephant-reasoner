#include <stdio.h>
#include "../model/datatypes.h"
#include "../model/model.h"
#include "utils.h"

// The list of subclass axioms that are generated during  preprocessing
SubClassAxiom** generated_subclass_axioms = NULL;
int generated_subclass_axiom_count = 0;

// The list of subrole axioms that are generated during  preprocessing
SubRoleAxiom** generated_subrole_axioms = NULL;
int generated_subrole_axiom_count = 0;

// preprocess axioms that are syntactic shortcuts, like equivalent classes/roles, disjoint classes and
// transtive roles
void preprocess_tbox(TBox* tbox) {
	int i;

	// convert eqclass axioms to subclass axioms
	for (i = 0; i < tbox->eqclass_axiom_count; ++i) {
		add_generated_subclass_axiom(create_subclass_axiom(tbox->eqclass_axioms[i]->lhs, tbox->eqclass_axioms[i]->rhs));
		add_generated_subclass_axiom(create_subclass_axiom(tbox->eqclass_axioms[i]->rhs, tbox->eqclass_axioms[i]->lhs));
	}

	// convert eqrole axioms to subrole axioms
	for (i = 0; i < tbox->eqrole_axiom_count; ++i) {
		add_generated_subrole_axiom(create_subrole_axiom(tbox->eqrole_axioms[i]->lhs, tbox->eqrole_axioms[i]->rhs));
		add_generated_subrole_axiom(create_subrole_axiom(tbox->eqrole_axioms[i]->rhs, tbox->eqrole_axioms[i]->lhs));
	}

	// process the transitive role axioms
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
