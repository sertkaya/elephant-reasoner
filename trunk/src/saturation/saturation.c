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
#include "../index/utils.h"
#include "utils.h"


void saturate_roles(TBox* tbox) {
	RoleSaturationAxiom* ax;
	Stack scheduled_axioms;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
    PWord_t key = NULL;
    uint8_t role_index[MAX_ROLE_NAME_LENGTH];

    // first the atomic roles
	// start with the smallest role name
	role_index[0] = '\0';
    JSLF(key, tbox->atomic_roles, role_index);
    while (key != NULL) {
            push(&scheduled_axioms, create_role_saturation_axiom((Role*) *key, (Role*) *key));
            JSLN(key, tbox->atomic_roles, role_index);
    }
    // now the role compositions
    int i;
	role_index[0] = '\0';
    JSLF(key, tbox->role_compositions, role_index);
    while (key != NULL) {
    		// for role compositions we do not need axioms of type role_comp(r,s) -> role_comp(r,s)
    		// in the initialization. the reason is, they can only appear on the lhs of a role inclusion axiom
    		// therefore, we push only axioms with told subsumer on the rhs
    		for (i = 0; i < ((Role*) *key)->told_subsumer_count; ++i)
    			push(&scheduled_axioms, create_role_saturation_axiom((Role*) *key, ((Role*) *key)->told_subsumers[i]));
    		// push(&scheduled_axioms, create_role_saturation_axiom((Role*) *key, (Role*) *key));
            JSLN(key, tbox->role_compositions, role_index);
    }

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

/*
void saturate_concepts(TBox* tbox) {
	ConceptSaturationAxiom* ax;
	Stack scheduled_axioms;
	int unique_subsumption_count = 0, total_subsumption_count = 0;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	int i,j;
	for (i = 0; i < tbox->atomic_concept_count ; ++i)
		push(&scheduled_axioms, create_concept_saturation_axiom(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i], INITIALIZATION));

	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		++total_subsumption_count;

		if (ax->type == LINK || MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
			// printf("%d:", ax->type);
			// print_concept(ax->lhs);
			// printf("=>");
			// print_concept(ax->rhs);
			// printf("\n");
			if (ax->type == LINK) {
				add_predecessor(ax->lhs, ax->rhs);
				// add_successor(ax->lhs, ax->rhs);

				// existential introduction
				for (i = 0; i < ax->rhs->description.exists->filler->subsumer_count; ++i)
					for (j = 0; j < ax->rhs->description.exists->role->subsumer_count; ++j) {
						Concept* ex = get_negative_exists(ax->rhs->description.exists->filler->subsumer_list[i], ax->rhs->description.exists->role->subsumer_list[j]);
						if (ex != NULL)
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, EXISTENTIAL_INTRODUCTION));
					}

				// the role chain rule
				// subsumers of role of existential on the rhs
				for (i = 0; i < ax->rhs->description.exists->role->subsumer_count; ++i) {

					// printf("subsumer of filler of rhs: ");
					// print_role(ax->rhs->description.exists->role->subsumer_list[i]);
					// printf("\n");

					// the role composition where this role appears as the second component
					for (j = 0; j < ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_count; ++j) {

						// printf("role composition where this role appears as the second component: ");
						// print_role(ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]);
						// printf("\n");

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

								// printf("role_p->subsumer_list[k]: ");
								// print_role(((Role*) role_p)->subsumer_list[k]);
								// printf("\n");
								// printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->second_component_of_list[j]->role1: ");
								// print_role(ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->description.role_composition->role1);
								// printf("\n");

								if (((Role*) role_p)->subsumer_list[k] ==
										ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->description.role_composition->role1) {
									// such a composition has been found, now for each predecessor
									predecessor_p = 0;
									predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;
									Concept *ex = NULL;
									J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
									while (predecessor_bitmap_nonempty) {
										for (l = 0; l < ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->subsumer_count; ++l) {
											// create exists ... role=subsumers of the composition, filler=filler of the rhs
											ex = get_exists_restriction(
													ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->subsumer_list[l]->id,
													ax->rhs->description.exists->filler->id,
													tbox);
											// create ax: lhs = predecessor, rhs = exists created
											if (ex != NULL)
												push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ex, LINK));
											// add_predecessor((Concept*) predecessor_p, ex);
											// add_successor((Concept*) predecessor_p, ex);
										}
										J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
									}
								}
							}
							JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
						}
					}

					// now the same for the successors of the filler of the existential on the rhs
					// the role composition where this role appears as the first component
					for (j = 0; j < ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_count; ++j) {

						// printf("role composition where this role appears as the first component: ");
						// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]);
						// printf("\n");

						PWord_t successor_bitmap_p;
						Pvoid_t successor_bitmap;
						Word_t role_p;
						int successor_bitmap_nonempty, k, l;
						Word_t successor_p;
						role_p = 0;
						JLF(successor_bitmap_p, ax->rhs->description.exists->filler->successors, role_p);
						while (successor_bitmap_p != NULL) {

							// printf("role_p: ");
							// print_role((Role*) role_p);
							// printf("\n");

							for (k = 0; k < ((Role*) role_p)->subsumer_count; ++k) {
								// now check:
								// role_p->subsumer_list[k] == ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->role2

								// printf("role_p->subsumer_list[k]: ");
								// print_role(((Role*) role_p)->subsumer_list[k]);
								// printf("\n");
								// printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2: ");
								// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2);
								// printf("\n");

								if (((Role*) role_p)->subsumer_list[k] ==
										ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2) {
									// such a composition has been found, now for each successor
									successor_p = 0;
									successor_bitmap = (Pvoid_t) *successor_bitmap_p;
									Concept *ex = NULL;
									J1F(successor_bitmap_nonempty, successor_bitmap, successor_p);
									while (successor_bitmap_nonempty) {

										// printf("successor: ");
										// print_concept((Concept*) successor_p);
										// printf("\n");

										for (l = 0; l < ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_count; ++l) {
											// create exists ... role=subsumers of the composition, filler=successor of the filler of the rhs

											// printf("ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l]: ");
											// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l]);
											// printf("\n");

											ex = get_exists_restriction(
													ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l]->id,
													((Concept*) successor_p)->id,
													tbox);
											// create ax: lhs = ax->lhs, rhs = exists created
											if (ex != NULL)
												push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) ax->lhs, ex, LINK));
											// add_predecessor((Concept*) ax->lhs, ex);
											// add_successor((Concept*) ax->lhs, ex);
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
			else {
				++unique_subsumption_count;
				// printf("%d:", ax->type);
				// print_concept(ax->lhs);
				// printf("=>");
				// print_concept(ax->rhs);
				// printf("\n");


				// conjunction introduction
				// the first conjunct
				int lhs_is_subsumed_by_other_conjunct;
				for (i = 0; i < ax->rhs->first_conjunct_of_count; i++) {
					// check if lhs is subsumed by the second conjunct as well
					J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->first_conjunct_of_list[i]->description.conj->conjunct2);
					if (lhs_is_subsumed_by_other_conjunct)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->first_conjunct_of_list[i], CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax->rhs->second_conjunct_of_count; i++) {
					// check if lhs is also subsumed by the first conjunct
					// TODO: (replace with a call to the function is_subsumer_of, check perf. change. first move the function from package hierarchy to model)
					J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->second_conjunct_of_list[i]->description.conj->conjunct1);
					if (lhs_is_subsumed_by_other_conjunct)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->second_conjunct_of_list[i], CONJUNCTION_INTRODUCTION));
				}

				switch (ax->rhs->type) {
				// conjunction decomposition
				case CONJUNCTION:
					if (ax->type != CONJUNCTION_INTRODUCTION) {
						// conjunct 1 as rhs
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj->conjunct1, CONJUNCTION_DECOMPOSITION));

						// conjunct 2 as rhs
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj->conjunct2, CONJUNCTION_DECOMPOSITION));
					}
					break;
				case EXISTENTIAL_RESTRICTION:
					if (ax->type != EXISTENTIAL_INTRODUCTION) {
						if (ax->rhs->description.exists->filler->type != ATOMIC_CONCEPT)
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs->description.exists->filler, ax->rhs->description.exists->filler, INITIALIZATION));

						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs, LINK));
						// printf("----------%d:", ax->type);
						// print_concept(ax->lhs);
						// printf("=>");
						// print_concept(ax->rhs);
						// printf("---------\n");
						// add_predecessor(ax->lhs, ax->rhs);
						// add_successor(ax->lhs, ax->rhs);
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
						ex = get_negative_exists(ax->rhs, ((Role*) role_p)->subsumer_list[i]);
						if (ex != NULL) {
							predecessor_p = 0;
							predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;
							J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
							while (predecessor_bitmap_nonempty) {
								push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ex, EXISTENTIAL_INTRODUCTION));
								J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
							}
						}
					}
					JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
				}

				// told subsumers
				for (i = 0; i < ax->rhs->told_subsumer_count; i++)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i], TOLD_SUBSUMER));
			}
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}
	// printf("Total derivations:%d\nUnique derivations:%d\n", total_subsumption_count, unique_subsumption_count);
}
*/

void saturate_concepts(TBox* tbox) {
	ConceptSaturationAxiom* ax;
	Stack scheduled_axioms;
	int unique_subsumption_count = 0, total_subsumption_count = 0;
	int unique_link_count = 0, total_link_count = 0;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	int i, j;
	/*
	for (i = 0; i < tbox->atomic_concept_count ; ++i)
		push(&scheduled_axioms, create_concept_saturation_axiom(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i], NULL, SUBSUMPTION_INITIALIZATION));
		*/

	for (i = 0; i < tbox->atomic_concept_count ; ++i)
		for (j = 0; j < tbox->atomic_concept_list[i]->told_subsumer_count; ++j)
			push(&scheduled_axioms, create_concept_saturation_axiom(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i]->told_subsumers[j], NULL, SUBSUMPTION_INITIALIZATION));

	/*
	for (i = 0; i < tbox->subclass_axiom_count; ++i)
		push(&scheduled_axioms, create_concept_saturation_axiom(tbox->subclass_axioms[i]->lhs, tbox->subclass_axioms[i]->rhs, NULL, SUBSUMPTION_INITIALIZATION));

	for (i = 0; i < tbox->eqclass_axiom_count; ++i) {
		push(&scheduled_axioms, create_concept_saturation_axiom(tbox->eqclass_axioms[i]->lhs, tbox->eqclass_axioms[i]->rhs, NULL, SUBSUMPTION_INITIALIZATION));
		push(&scheduled_axioms, create_concept_saturation_axiom(tbox->eqclass_axioms[i]->rhs, tbox->eqclass_axioms[i]->lhs, NULL, SUBSUMPTION_INITIALIZATION));
	}
	*/


	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		switch (ax->type) {
		case SUBSUMPTION_CONJUNCTION_INTRODUCTION:
		case SUBSUMPTION_EXISTENTIAL_INTRODUCTION:
			++total_subsumption_count;
			// no conjunction decomposition and no existential decomposition here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
				++unique_subsumption_count;

				// conjunction introduction
				// the first conjunct
				int lhs_is_subsumed_by_other_conjunct;
				for (i = 0; i < ax->rhs->first_conjunct_of_count; i++) {
					// check if lhs is subsumed by the second conjunct as well
					J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->first_conjunct_of_list[i]->description.conj->conjunct2);
					if (lhs_is_subsumed_by_other_conjunct)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->first_conjunct_of_list[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax->rhs->second_conjunct_of_count; i++) {
					// check if lhs is also subsumed by the first conjunct
					J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->second_conjunct_of_list[i]->description.conj->conjunct1);
					if (lhs_is_subsumed_by_other_conjunct)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->second_conjunct_of_list[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
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
						ex = get_negative_exists(ax->rhs, ((Role*) role_p)->subsumer_list[i]);
						if (ex != NULL) {
							predecessor_p = 0;
							predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;
							J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
							while (predecessor_bitmap_nonempty) {
								push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
								J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
							}
						}
					}
					JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
				}

				// told subsumers
				for (i = 0; i < ax->rhs->told_subsumer_count; i++)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case SUBSUMPTION_INITIALIZATION:
		case SUBSUMPTION_CONJUNCTION_DECOMPOSITION:
		case SUBSUMPTION_TOLD_SUBSUMER:
			++total_subsumption_count;
			// all here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
				++unique_subsumption_count;

				// conjunction introduction
				// the first conjunct
				int lhs_is_subsumed_by_other_conjunct;
				for (i = 0; i < ax->rhs->first_conjunct_of_count; i++) {
					// check if lhs is subsumed by the second conjunct as well
					J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->first_conjunct_of_list[i]->description.conj->conjunct2);
					if (lhs_is_subsumed_by_other_conjunct)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->first_conjunct_of_list[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax->rhs->second_conjunct_of_count; i++) {
					// check if lhs is also subsumed by the first conjunct
					J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->second_conjunct_of_list[i]->description.conj->conjunct1);
					if (lhs_is_subsumed_by_other_conjunct)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->second_conjunct_of_list[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				switch (ax->rhs->type) {
				case CONJUNCTION:
					// conjunction decomposition
					// conjunct 1 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj->conjunct1, NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));

					// conjunct 2 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj->conjunct2, NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));
					break;
				case EXISTENTIAL_RESTRICTION:
					// existential decomposition
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.exists->filler, ax->rhs->description.exists->role, LINK));
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
						ex = get_negative_exists(ax->rhs, ((Role*) role_p)->subsumer_list[i]);
						if (ex != NULL) {
							predecessor_p = 0;
							predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;
							J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
							while (predecessor_bitmap_nonempty) {
								push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
								J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
							}
						}
					}
					JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
				}

				// told subsumers
				for (i = 0; i < ax->rhs->told_subsumer_count; i++)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case LINK:
			++total_link_count;
			if (add_successor(ax->lhs, ax->role, ax->rhs)) {
				add_predecessor(ax->rhs, ax->role, ax->lhs);
				++unique_link_count;

				// existential introduction
				for (i = 0; i < ax->rhs->subsumer_count; ++i)
					for (j = 0; j < ax->role->subsumer_count; ++j) {
						Concept* ex = get_negative_exists(ax->rhs->subsumer_list[i], ax->role->subsumer_list[j]);
						if (ex != NULL)
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
					}

				// the role chain rule
				// subsumers of the role (of existential on the rhs)
				for (i = 0; i < ax->role->subsumer_count; ++i) {

					// printf("subsumer of filler of rhs: ");
					// print_role(ax->rhs->description.exists->role->subsumer_list[i]);
					// printf("\n");

					// the role composition where this role appears as the second component
					for (j = 0; j < ax->role->subsumer_list[i]->second_component_of_count; ++j) {

						// printf("role composition where this role appears as the second component: ");
						// print_role(ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]);
						// printf("\n");

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

								// printf("role_p->subsumer_list[k]: ");
								// print_role(((Role*) role_p)->subsumer_list[k]);
								// printf("\n");
								// printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->second_component_of_list[j]->role1: ");
								// print_role(ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->description.role_composition->role1);
								// printf("\n");

								if (((Role*) role_p)->subsumer_list[k] ==
										ax->role->subsumer_list[i]->second_component_of_list[j]->description.role_composition->role1) {
									// such a composition has been found, now for each predecessor
									predecessor_p = 0;
									predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;
									Concept *ex = NULL;
									J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
									while (predecessor_bitmap_nonempty) {
										for (l = 0; l < ax->role->subsumer_list[i]->second_component_of_list[j]->told_subsumer_count; ++l)
											push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ax->rhs, ax->role->subsumer_list[i]->second_component_of_list[j]->told_subsumers[l], LINK));
										J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
									}
								}
							}
							JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
						}
					}


					// now the same for the successors of the filler of the existential on the rhs
					// the role composition where this role appears as the first component
					for (j = 0; j < ax->role->subsumer_list[i]->first_component_of_count; ++j) {

						// printf("role composition where this role appears as the first component: ");
						// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]);
						// printf("\n");

						PWord_t successor_bitmap_p;
						Pvoid_t successor_bitmap;
						Word_t role_p;
						int successor_bitmap_nonempty, k, l;
						Word_t successor_p;
						role_p = 0;
						JLF(successor_bitmap_p, ax->rhs->successors, role_p);
						while (successor_bitmap_p != NULL) {

							// printf("role_p: ");
							// print_role((Role*) role_p);
							// printf("\n");

							for (k = 0; k < ((Role*) role_p)->subsumer_count; ++k) {
								// now check:
								// role_p->subsumer_list[k] == ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->role2

								// printf("role_p->subsumer_list[k]: ");
								// print_role(((Role*) role_p)->subsumer_list[k]);
								// printf("\n");
								// printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2: ");
								// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2);
								// printf("\n");

								if (((Role*) role_p)->subsumer_list[k] ==
										ax->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2) {
									// such a composition has been found, now for each successor
									successor_p = 0;
									successor_bitmap = (Pvoid_t) *successor_bitmap_p;
									Concept *ex = NULL;
									J1F(successor_bitmap_nonempty, successor_bitmap, successor_p);
									while (successor_bitmap_nonempty) {

										// printf("successor: ");
										// print_concept((Concept*) successor_p);
										// printf("\n");

										for (l = 0; l < ax->role->subsumer_list[i]->first_component_of_list[j]->told_subsumer_count; ++l)
											push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, (Concept*) successor_p, ax->role->subsumer_list[i]->first_component_of_list[j]->told_subsumers[l], LINK));
										J1N(successor_bitmap_nonempty, successor_bitmap, successor_p);
									}
								}
							}
							JLN(successor_bitmap_p, ax->rhs->successors, role_p);
						}
					}
				}

				// init
				push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));

			}
			break;
		default:
			fprintf(stderr, "Unknown saturation axiom type, aborting.");
			exit(EXIT_FAILURE);
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}
	printf("Total subsumptions:%d\nUnique subsumptions:%d\n", total_subsumption_count, unique_subsumption_count);
	printf("Total links:%d\nUnique links:%d\n", total_link_count, unique_link_count);
}

/*
void saturate_concepts(TBox* tbox) {
	ConceptSaturationAxiom* ax;
	Stack scheduled_axioms;
	int unique_subsumption_count = 0, total_subsumption_count = 0;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	int i,j;
	// for (i = 0; i < tbox->atomic_concept_count ; ++i)
	// 	push(&scheduled_axioms, create_concept_saturation_axiom(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i], INITIALIZATION));

	for (i = 0; i < tbox->atomic_concept_count ; ++i)
		for (j = 0; j < tbox->atomic_concept_list[i]->told_subsumer_count; ++j)
			push(&scheduled_axioms, create_concept_saturation_axiom(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i]->told_subsumers[j], INITIALIZATION));

	// for (i = 0; i < tbox->subclass_axiom_count; ++i)
	// 	push(&scheduled_axioms, create_concept_saturation_axiom(tbox->subclass_axioms[i]->lhs, tbox->subclass_axioms[i]->rhs, INITIALIZATION));

	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		++total_subsumption_count;

		if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
			++unique_subsumption_count;

			printf("%d:", ax->type);
			print_concept(ax->lhs);
			printf("=>");
			print_concept(ax->rhs);
			printf("\n");

			// conjunction introduction
			// the first conjunct
			int lhs_is_subsumed_by_other_conjunct;
			for (i = 0; i < ax->rhs->first_conjunct_of_count; i++) {
				// check if lhs is subsumed by the second conjunct as well
				J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->first_conjunct_of_list[i]->description.conj->conjunct2);
				if (lhs_is_subsumed_by_other_conjunct)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->first_conjunct_of_list[i], CONJUNCTION_INTRODUCTION));
			}

			// now the same for the second conjunct
			for (i = 0; i < ax->rhs->second_conjunct_of_count; i++) {
				// check if lhs is also subsumed by the first conjunct
				// TODO: (replace with a call to the function is_subsumer_of, check perf. change. first move the function from package hierarchy to model)
				J1T(lhs_is_subsumed_by_other_conjunct, ax->lhs->subsumers, (Word_t) ax->rhs->second_conjunct_of_list[i]->description.conj->conjunct1);
				if (lhs_is_subsumed_by_other_conjunct)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->second_conjunct_of_list[i], CONJUNCTION_INTRODUCTION));
			}

			switch (ax->rhs->type) {
			// conjunction decomposition
			case CONJUNCTION:
				if (ax->type != CONJUNCTION_INTRODUCTION) {
					// conjunct 1 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj->conjunct1, CONJUNCTION_DECOMPOSITION));

					// conjunct 2 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj->conjunct2, CONJUNCTION_DECOMPOSITION));
				}
				break;
			case EXISTENTIAL_RESTRICTION:
				if (ax->type != EXISTENTIAL_INTRODUCTION) {
					if (ax->rhs->description.exists->filler->type != ATOMIC_CONCEPT)
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs->description.exists->filler, ax->rhs->description.exists->filler, INITIALIZATION));

					add_predecessor(ax->rhs->description.exists->filler, ax->rhs->description.exists->role, ax->lhs);
					add_successor(ax->lhs, ax->rhs->description.exists->role, ax->rhs->description.exists->filler);

					for (i = 0; i < ax->rhs->description.exists->filler->subsumer_count; ++i)
						for (j = 0; j < ax->rhs->description.exists->role->subsumer_count; ++j) {
							Concept* ex = get_negative_exists(ax->rhs->description.exists->filler->subsumer_list[i], ax->rhs->description.exists->role->subsumer_list[j]);
							if (ex != NULL)
								push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, EXISTENTIAL_INTRODUCTION));
						}

				}

				// the role chain rule
				// subsumers of role of existential on the rhs
				for (i = 0; i < ax->rhs->description.exists->role->subsumer_count; ++i) {

					// printf("subsumer of filler of rhs: ");
					// print_role(ax->rhs->description.exists->role->subsumer_list[i]);
					// printf("\n");

					// the role composition where this role appears as the second component
					for (j = 0; j < ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_count; ++j) {

						// printf("role composition where this role appears as the second component: ");
						// print_role(ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]);
						// printf("\n");

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

								// printf("role_p->subsumer_list[k]: ");
								// print_role(((Role*) role_p)->subsumer_list[k]);
								// printf("\n");
								// printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->second_component_of_list[j]->role1: ");
								// print_role(ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->description.role_composition->role1);
								// printf("\n");

								if (((Role*) role_p)->subsumer_list[k] ==
										ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->description.role_composition->role1) {
									// such a composition has been found, now for each predecessor
									predecessor_p = 0;
									predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;
									Concept *ex = NULL;
									J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
									while (predecessor_bitmap_nonempty) {
										for (l = 0; l < ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->told_subsumer_count; ++l) {
											// create exists ... role=subsumers of the composition, filler=filler of the rhs
											// ex = get_create_exists_restriction(
											// 		ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->subsumer_list[l],
											// 		// ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->told_subsumers[l],
											// 		ax->rhs->description.exists->filler,
											// 		tbox);
											add_successor((Concept*) predecessor_p, ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->told_subsumers[l], ax->rhs->description.exists->filler);
											add_predecessor(ax->rhs->description.exists->filler, ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->told_subsumers[l], (Concept*) predecessor_p);
											// ex = get_exists_restriction(
											// 		ax->rhs->description.exists->role->subsumer_list[i]->second_component_of_list[j]->subsumer_list[l]->id,
											// 		ax->rhs->description.exists->filler->id, tbox);
											// create ax: lhs = predecessor, rhs = exists created
											// if (ex != NULL)
											// push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ex, EXISTENTIAL_INTRODUCTION));
											push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs->description.exists->filler, ax->rhs->description.exists->filler, INITIALIZATION));
										}
										J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
									}
								}
							}
							JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
						}
					}


					// now the same for the successors of the filler of the existential on the rhs
					// the role composition where this role appears as the first component
					for (j = 0; j < ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_count; ++j) {

						// printf("role composition where this role appears as the first component: ");
						// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]);
						// printf("\n");

						PWord_t successor_bitmap_p;
						Pvoid_t successor_bitmap;
						Word_t role_p;
						int successor_bitmap_nonempty, k, l;
						Word_t successor_p;
						role_p = 0;
						JLF(successor_bitmap_p, ax->rhs->description.exists->filler->successors, role_p);
						while (successor_bitmap_p != NULL) {

							// printf("role_p: ");
							// print_role((Role*) role_p);
							// printf("\n");

							for (k = 0; k < ((Role*) role_p)->subsumer_count; ++k) {
								// now check:
								// role_p->subsumer_list[k] == ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->role2

								// printf("role_p->subsumer_list[k]: ");
								// print_role(((Role*) role_p)->subsumer_list[k]);
								// printf("\n");
								// printf("ax->rhs->descriptin.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2: ");
								// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2);
								// printf("\n");

								if (((Role*) role_p)->subsumer_list[k] ==
										ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->description.role_composition->role2) {
									// such a composition has been found, now for each successor
									successor_p = 0;
									successor_bitmap = (Pvoid_t) *successor_bitmap_p;
									Concept *ex = NULL;
									J1F(successor_bitmap_nonempty, successor_bitmap, successor_p);
									while (successor_bitmap_nonempty) {

										// printf("successor: ");
										// print_concept((Concept*) successor_p);
										// printf("\n");

										for (l = 0; l < ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->told_subsumer_count; ++l) {
											// create exists ... role=subsumers of the composition, filler=successor of the filler of the rhs

											// printf("ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l]: ");
											// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l]);
											// printf("\n");

											// ex = get_create_exists_restriction(
											// 		ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->subsumer_list[l],
											// 		// ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->told_subsumers[l],
											// 		(Concept*) successor_p,
											// 		tbox);
											add_successor(ax->lhs, ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->told_subsumers[l], (Concept*) successor_p);
											add_predecessor((Concept*) successor_p, ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->told_subsumers[l], ax->lhs);
											// ex = get_exists_restriction(
											// 		ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]->told_subsumers[l]->id,
											// 		((Concept*) successor_p)->id, tbox);
											// create ax: lhs = ax->lhs, rhs = exists created
											// create ax: lhs = ax->lhs, rhs = exists created
											// if (ex != NULL)
											// push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) ax->lhs, ex, EXISTENTIAL_INTRODUCTION));
											push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) successor_p, (Concept*) successor_p, INITIALIZATION));
										}
										J1N(successor_bitmap_nonempty, successor_bitmap, successor_p);
									}
								}
							}
							JLN(successor_bitmap_p, ax->rhs->successors, role_p);
						}
					}
				}
//

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
							push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ex, EXISTENTIAL_INTRODUCTION));
							J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
						}
					}
				}
				JLN(predecessor_bitmap_p, ax->lhs->predecessors, role_p);
			}

			// told subsumers
			for (i = 0; i < ax->rhs->told_subsumer_count; i++)
				push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i], TOLD_SUBSUMER));
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}
	// printf("Total derivations:%d\nUnique derivations:%d\n", total_subsumption_count, unique_subsumption_count);
}
*/

void saturate_tbox(TBox* tbox) {

	saturate_roles(tbox);
	saturate_concepts(tbox);

}

