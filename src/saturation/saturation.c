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


#include <string.h>
#include <assert.h>

#include "../model/model.h"
#include "../model/utils.h"
#include "../model/memory_utils.h"
#include "../model/print_utils.h"
#include "../model/limits.h"
#include "../utils/stack.h"
// #include "../utils/queue.h"
#include "../index/utils.h"

// #include "locals.h"
#include "utils.h"


void saturate_roles(TBox* tbox) {
	RoleSaturationAxiom* ax;
	Stack scheduled_axioms;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
    PWord_t key = NULL;
    uint8_t atomic_role_index[MAX_ROLE_NAME_LENGTH];

	// start with the smallest role name
	atomic_role_index[0] = '\0';
    JSLF(key, tbox->atomic_roles, atomic_role_index);
    while (key != NULL) {
            push(&scheduled_axioms, create_role_saturation_axiom((Role*) *key, (Role*) *key));
            JSLN(key, tbox->atomic_roles, atomic_role_index);
    }

    int i;
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		if (mark_role_saturation_axiom_processed(ax)) {
			// told subsumers
			for (i = 0; i < ax->rhs->told_subsumer_count; ++i)
				push(&scheduled_axioms, create_role_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i]));
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}
}

void saturate_concepts(TBox* tbox) {
	ConceptSaturationAxiom* ax;
	Stack scheduled_axioms;
	int unique_derivation_count = 0, total_derivation_count = 0;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	int i,j;
	for (i = 0; i < tbox->atomic_concept_count ; ++i)
		for (j = 0; j < tbox->atomic_concept_list[i]->told_subsumer_count; ++j)
			push(&scheduled_axioms, create_concept_saturation_axiom(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i]->told_subsumers[j], 0, 0, INIT));

	/*
	for (i = 0; i < tbox->atomic_concept_count ; ++i)
		push(&scheduled_axioms, create_concept_saturation_axiom(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i], 0, 0, INIT));
		*/


	ax = pop(&scheduled_axioms);
	// ax = dequeue(&scheduled_axioms);
	while (ax != NULL) {
		++total_derivation_count;
		if (mark_concept_saturation_axiom_processed(ax)) {
			++unique_derivation_count;

			// conjunction introduction
			// the first conjunct
			int lhs_is_subsumed_by_other_conjunct;
			for (i = 0; i < ax->rhs->first_conjunct_of_count; i++) {
				// check if lhs is subsumed by the second conjunct as well
				J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->first_conjunct_of_list[i]->description.conj->conjunct2);
				if (lhs_is_subsumed_by_other_conjunct)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->first_conjunct_of_list[i], 1, 0, CONJ_INTRO_1));
			}

			// now the same for the second conjunct
			for (i = 0; i < ax->rhs->second_conjunct_of_count; i++) {
				// check if lhs is also subsumed by the first conjunct
				// TODO: (replace with a call to the function is_subsumer_of, check perf. change. first move the function from package hierarchy to model)
				J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->second_conjunct_of_list[i]->description.conj->conjunct1);
				if (lhs_is_subsumed_by_other_conjunct)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->second_conjunct_of_list[i], 1, 0, CONJ_INTRO_2));
			}

			switch (ax->rhs->type) {
			// conjunction decomposition
			case CONJUNCTION:
				if (!ax->derived_via_conj_introduction) {
					// conjunct 1 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj->conjunct1, 0, 0, CONJ_DECOMP_1));

					// conjunct 2 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj->conjunct2, 0, 0, CONJ_DECOMP_2));
				}
				break;
			case EXISTENTIAL_RESTRICTION:
				for (i = 0; i < ax->rhs->description.exists->role->subsumer_count; ++i) {
					Concept* ex = get_negative_exists(ax->rhs->description.exists->filler, ax->rhs->description.exists->role->subsumer_list[i]);
					if (ex != NULL)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, 0, 0, EXISTS_DECOMP));
				}
				if (!ax->derived_via_exists_introduction) {
					if (ax->rhs->description.exists->filler->type != ATOMIC_CONCEPT)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs->description.exists->filler, ax->rhs->description.exists->filler, 0, 0, INIT));

					add_predecessor(ax->lhs, ax->rhs);
					add_successor(ax->lhs, ax->rhs);

					for (i = 0; i < ax->rhs->description.exists->filler->subsumer_count; ++i) {
						for (j = 0; j < ax->rhs->description.exists->role->subsumer_count; ++j) {
							Concept* ex = get_negative_exists(ax->rhs->description.exists->filler->subsumer_list[i], ax->rhs->description.exists->role->subsumer_list[j]);
							if (ex != NULL && ax->rhs->description.exists->filler != ax->rhs->description.exists->filler->subsumer_list[i])
								push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, 0, 0, EXISTS_DECOMP));
						}
					}

					// subsumers of role of existential on the rhs
					for (i = 0; i < ax->rhs->description.exists->role->subsumer_count; ++i) {
						printf("subsumer of filler of rhs: ");
						print_role(ax->rhs->description.exists->role->subsumer_list[i]);
						printf("\n");

						// the role composition where this role appears as the second component
						for (j = 0; j < ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_count; ++j) {
							printf("role composition where this role appears as the second component: ");
							print_role(ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]);
							printf("\n");

							PWord_t predecessor_bitmap_p;
							Pvoid_t predecessor_bitmap;
							Word_t role_p;
							int predecessor_bitmap_nonempty, k, l;
							Word_t predecessor_p;
							role_p = 0;
							JLF(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
							while (predecessor_bitmap_p != NULL) {
								for (k = 0; k < ((Role*) role_p)->subsumer_count; ++k) {
									// now check:
									// role_p->subsumer_list[k] == ax->rhs->descriptin.exists->role->subsumer_list[i]->second_component_of_list[j]->role1
									printf("role_p->subsumer_list[k]: ");
									print_role(((Role*) role_p)->subsumer_list[k]);
									printf("\n");
									printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->second_component_of_list[j]->role1: ");
									print_role(((RoleComposition*) ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j])->role1);
									printf("\n");

									if (((Role*) role_p)->subsumer_list[k] ==
											((RoleComposition*) ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j])->role1) {
										// such a composition has been found, now for each predecessor
										predecessor_p = 0;
										predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;
										Concept *ex = NULL;
										J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
										while (predecessor_bitmap_nonempty) {
											for (l = 0; l < ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->subsumer_count; ++l) {
												// create exists ... role=subsumers of the composition, filler=filler of the rhs
												ex = get_create_exists_restriction(
														ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->subsumer_list[l],
														ax->rhs->description.exists->filler,
														tbox);
												// create ax: lhs = predecessor, rhs = exists created
												push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ex, 0, 0, EXISTS_DECOMP));
												J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
											}
										}
									}
								}
								JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
							}
						}

						// now the same for the successors of the filler of the existential on the rhs
						// the role composition where this role appears as the first component
						/*
						for (j = 0; j < ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_count; ++j) {
							printf("role composition where this role appears as the first component: ");
							print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]);
							printf("\n");

							PWord_t successor_bitmap_p;
							Pvoid_t successor_bitmap;
							Word_t role_p;
							int successor_bitmap_nonempty, k, l;
							Word_t successor_p;
							role_p = 0;
							JLF(successor_bitmap_p, ax->lhs->successors, role_p);
							while (successor_bitmap_p != NULL) {
								printf("role_p: ");
								print_role((Role*) role_p);
								printf("\n");

								for (k = 0; k < ((Role*) role_p)->subsumer_count; ++k) {
									// now check:
									// role_p->subsumer_list[k] == ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->role2
									printf("role_p->subsumer_list[k]: ");
									print_role(((Role*) role_p)->subsumer_list[k]);
									printf("\n");
									printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->role2: ");
									print_role(((RoleComposition*) ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j])->role2);
									printf("\n");

									if (((Role*) role_p)->subsumer_list[k] ==
											((RoleComposition*) ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j])->role2) {
										// such a composition has been found, now for each successor
										successor_p = 0;
										successor_bitmap = (Pvoid_t) *successor_bitmap_p;
										Concept *ex = NULL;
										J1F(successor_bitmap_nonempty, successor_bitmap, successor_p);
										while (successor_bitmap_nonempty) {
											for (l = 0; l < ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_count; ++l) {
												// create exists ... role=subsumers of the composition, filler=successor of the filler of the rhs
												ex = get_create_exists_restriction(
														ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l],
														(Concept*) successor_p,
														tbox);
												// create ax: lhs = ax->lhs, rhs = exists created
												push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) ax->lhs, ex, 0, 0, EXISTS_DECOMP));
												J1N(successor_bitmap_nonempty, successor_bitmap, successor_p);
											}
										}
									}
								}
								JLN(successor_bitmap_p, ax->rhs->successors, role_p);
							}
						}
						 */
						for (j = 0; j < ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_count; ++j) {
							printf("role composition where this role appears as the first component: ");
							print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]);
							printf("\n");

							PWord_t successor_bitmap_p;
							Pvoid_t successor_bitmap;
							Word_t role_p;
							int successor_bitmap_nonempty, k, l;
							Word_t successor_p;
							role_p = 0;
							JLF(successor_bitmap_p, ax->rhs->description.exists->filler->successors, role_p);
							while (successor_bitmap_p != NULL) {
								printf("role_p: ");
								print_role((Role*) role_p);
								printf("\n");

								for (k = 0; k < ((Role*) role_p)->subsumer_count; ++k) {
									// now check:
									// role_p->subsumer_list[k] == ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->role2
									printf("role_p->subsumer_list[k]: ");
									print_role(((Role*) role_p)->subsumer_list[k]);
									printf("\n");
									printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2: ");
									print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2);
									printf("\n");

									if (((Role*) role_p)->subsumer_list[k] ==
											ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2) {
										// such a composition has been found, now for each successor
										successor_p = 0;
										successor_bitmap = (Pvoid_t) *successor_bitmap_p;
										Concept *ex = NULL;
										J1F(successor_bitmap_nonempty, successor_bitmap, successor_p);
										while (successor_bitmap_nonempty) {
											printf("successor: ");
											print_concept((Concept*) successor_p);
											printf("\n");

											for (l = 0; l < ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_count; ++l) {
												// create exists ... role=subsumers of the composition, filler=successor of the filler of the rhs
												printf("ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l]: ");
												print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l]);
												printf("\n");

												ex = get_create_exists_restriction(
														ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l],
														(Concept*) successor_p,
														tbox);
												// create ax: lhs = ax->lhs, rhs = exists created
												push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) ax->lhs, ex, 0, 0, EXISTS_DECOMP));
											}
											J1N(successor_bitmap_nonempty, successor_bitmap, successor_p);
										}
									}
								}
								JLN(successor_bitmap_p, ax->rhs->successors, role_p);
							}
						}



					}

				}
				break;
			}


			// existential introduction
			PWord_t predecessor_bitmap_p;
			Pvoid_t predecessor_bitmap;
			Word_t role_p;
			Concept* ex;
			int predecessor_bitmap_nonempty;
			Word_t predecessor_p;

			role_p = 0;
			JLF(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
			while (predecessor_bitmap_p != NULL) {
				for (i = 0; i < ((Role*) role_p)->subsumer_count; ++i) {
					// ex = get_negative_exists(ax->rhs, (Role*) role_p);
					ex = get_negative_exists(ax->rhs, ((Role*) role_p)->subsumer_list[i]);
					if (ex != NULL) {
						predecessor_p = 0;
						predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;
						J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
						while (predecessor_bitmap_nonempty) {
							push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ex, 0, 1, EXISTS_INTRO));
							J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
						}
					}
				}
				JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
			}

			// told subsumers
			for (i = 0; i < ax->rhs->told_subsumer_count; i++)
				push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i], 0, 0, TOLD_SUBSUMER));
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}
	// printf("Total derivations:%d\nUnique derivations:%d\n", total_derivation_count, unique_derivation_count);
}

void saturate_tbox(TBox* tbox) {

	saturate_roles(tbox);
	saturate_concepts(tbox);

}

