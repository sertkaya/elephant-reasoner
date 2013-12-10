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
// #include "../model/utils.h"
// #include "../model/memory_utils.h"
#include "../model/print_utils.h"
#include "../model/limits.h"
#include "../utils/stack.h"
#include "../index/utils.h"
#include "utils.h"

// marks the axiom with the premise lhs and conclusion rhs as processed
#define MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)		add_to_concept_subsumer_list(ax->lhs, ax->rhs)

ConceptSaturationAxiom* create_concept_saturation_axiom(Concept* lhs, Concept* rhs, Role* role, enum saturation_axiom_type type) {
	ConceptSaturationAxiom* ax = (ConceptSaturationAxiom*) malloc(sizeof(ConceptSaturationAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	ax->role = role;
	ax->type = type;
	return ax;
}

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

				/*
				printf("SUBS:");
				print_concept(ax->lhs);
				printf("->");
				print_concept(ax->rhs);
				printf("\n");
				*/

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

				/*
				printf("SUBS:");
				print_concept(ax->lhs);
				printf("->");
				print_concept(ax->rhs);
				printf("\n");
				*/

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

				/*
				printf("LINK:");
				print_concept(ax->lhs);
				printf("->");
				print_role(ax->role);
				printf("->");
				print_concept(ax->rhs);
				printf("\n");
				*/

				// existential introduction
				for (i = 0; i < ax->rhs->subsumer_count; ++i)
					for (j = 0; j < ax->role->subsumer_count; ++j) {
						Concept* ex = get_negative_exists(ax->rhs->subsumer_list[i], ax->role->subsumer_list[j]);
						if (ex != NULL)
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
					}


				// the role chain rule
				// the role composition where this role appears as the second component
				for (j = 0; j < ax->role->second_component_of_count; ++j) {

					/*
					 printf("role composition where this role appears as the second component: ");
					 print_role(ax->role->subsumer_list[i]->second_component_of_list[j]);
					 printf("\n");
					 */

					PWord_t predecessor_bitmap_p;
					JLG(predecessor_bitmap_p, ax->lhs->predecessors, (Word_t) ax->role->second_component_of_list[j]->description.role_composition->role1);

					// if ax->lhs has ax->role->...->role1 predecessor
					if (predecessor_bitmap_p != NULL) {
						int predecessor_bitmap_nonempty, k, l;
						Word_t predecessor_p = 0;
						Pvoid_t predecessor_bitmap = (Pvoid_t) *predecessor_bitmap_p;

						J1F(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
						while (predecessor_bitmap_nonempty) {
							// printf("predecessor: ");
							// print_concept((Concept*) predecessor_p);
							// printf("\n");
							// for (l = 0; l < ax->role->second_component_of_list[j]->subsumer_count; ++l)
								// push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ax->rhs, ax->role->second_component_of_list[j]->subsumer_list[l], LINK));
							push(&scheduled_axioms, create_concept_saturation_axiom((Concept*) predecessor_p, ax->rhs, ax->role->second_component_of_list[j], LINK));
							J1N(predecessor_bitmap_nonempty, predecessor_bitmap, predecessor_p);
						}
					}
				}


				// now the same for the successors of the filler of the existential on the rhs
				// the role composition where this role appears as the first component
				for (j = 0; j < ax->role->first_component_of_count; ++j) {

					// printf("role composition where this role appears as the first component: ");
					// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]);
					// printf("\n");

					PWord_t successor_bitmap_p;
					JLG(successor_bitmap_p, ax->rhs->successors, (Word_t) ax->role->first_component_of_list[j]->description.role_composition->role2);

					// if ax->rhs has an ax->role->...->role2 successor
					if (successor_bitmap_p != NULL) {
						int successor_bitmap_nonempty, k, l;
						Word_t successor_p = 0;
						Pvoid_t successor_bitmap = (Pvoid_t) *successor_bitmap_p;

						J1F(successor_bitmap_nonempty, successor_bitmap, successor_p);
						while (successor_bitmap_nonempty) {
							// printf("successor: ");
							// print_concept((Concept*) successor_p);
							// printf("\n");

							// for (l = 0; l < ax->role->first_component_of_list[j]->subsumer_count; ++l)
								// push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, (Concept*) successor_p, ax->role->first_component_of_list[j]->subsumer_list[l], LINK));
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, (Concept*) successor_p, ax->role->first_component_of_list[j], LINK));
							J1N(successor_bitmap_nonempty, successor_bitmap, successor_p);
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
