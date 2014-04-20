#include "../model/datatypes.h"
#include "../model/model.h"
#include "utils.h"

// preprocess axioms that are syntactic shortcuts, like equivalent classes/roles, disjoint classes and
// transtive roles
void preprocess_tbox(TBox* tbox) {
	int i;

	// convert eqclass axioms to subclass axioms
	for (i = 0; i < tbox->eqclass_axiom_count; ++i) {
		add_subclass_axiom(create_subclass_axiom(tbox->eqclass_axioms[i]->lhs, tbox->eqclass_axioms[i]->rhs), tbox);
		add_subclass_axiom(create_subclass_axiom(tbox->eqclass_axioms[i]->rhs, tbox->eqclass_axioms[i]->lhs), tbox);
	}

	// convert eqrole axioms to subrole axioms
	for (i = 0; i < tbox->eqrole_axiom_count; ++i) {
		add_subrole_axiom(create_subrole_axiom(tbox->eqrole_axioms[i]->lhs, tbox->eqrole_axioms[i]->rhs), tbox);
		add_subrole_axiom(create_subrole_axiom(tbox->eqrole_axioms[i]->rhs, tbox->eqrole_axioms[i]->lhs), tbox);
	}

	// process the transitive role axioms
	Role* composition;
	for (i = 0; i < tbox->transitive_role_axiom_count; ++i) {
		composition = get_create_role_composition_binary(tbox->transitive_role_axioms[i]->r, tbox->transitive_role_axioms[i]->r, tbox);
		add_subrole_axiom(create_subrole_axiom(composition, tbox->transitive_role_axioms[i]->r), tbox);
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
				add_subclass_axiom(create_subclass_axiom(conjunction, tbox->bottom_concept), tbox);
			}

}
