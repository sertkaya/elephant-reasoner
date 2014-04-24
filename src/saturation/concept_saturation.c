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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../model/datatypes.h"
#include "../model/model.h"
#include "../model/utils.h"
#include "../model/limits.h"
#include "../utils/stack.h"
#include "../index/utils.h"
#include "../hashing/key_hash_table.h"
#include "../hashing/key_value_hash_table.h"
#include "utils.h"

// marks the axiom with the premise lhs and conclusion rhs as processed
#define MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)		insert_key(ax->lhs->subsumers, ax->rhs->id)

ConceptSaturationAxiom* create_concept_saturation_axiom(Concept* lhs, Concept* rhs, Role* role, enum saturation_axiom_type type) {
	ConceptSaturationAxiom* ax = (ConceptSaturationAxiom*) malloc(sizeof(ConceptSaturationAxiom));
	assert(ax != NULL);
	ax->lhs = lhs;
	ax->rhs = rhs;
	ax->role = role;
	ax->type = type;
	return ax;
}

/*
 * Saturates the concepts of a given TBox.
 * Returns:
 * 	-1: If the reasoning task is consistency check, and an atomic concept has the
 * 	subsumer bottom. In this case it immediately returns, i.e., saturation process
 * 	is cancelled.
 * 	0: Otherwise
 */
char saturate_concepts(TBox* tbox, ReasoningTask reasoning_task) {
	ConceptSaturationAxiom* ax;
	Stack scheduled_axioms;
	int unique_subsumption_count = 0, total_subsumption_count = 0;
	int unique_link_count = 0, total_link_count = 0;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// push the input axioms to the stack
	int i, j, k;

	for (i = 0; i < tbox->atomic_concept_count ; ++i)
		for (j = 0; j < tbox->atomic_concept_list[i]->told_subsumer_count; ++j)
			push(&scheduled_axioms, create_concept_saturation_axiom(tbox->atomic_concept_list[i], tbox->atomic_concept_list[i]->told_subsumers[j], NULL, SUBSUMPTION_INITIALIZATION));

	// The hash of nominals that are generated during preprocessing.
	extern KeyValueHashTable* generated_nominals;
	Node* node = last_node(generated_nominals);
	Concept* nominal = NULL;
	// The input axioms generated from concept and role assertions
	while (node) {
		nominal = (Concept*) node->value;
		for (j = 0; j < nominal->told_subsumer_count; ++j)
			push(&scheduled_axioms, create_concept_saturation_axiom(nominal, nominal->told_subsumers[j], NULL, SUBSUMPTION_INITIALIZATION));
		node = previous_node(node);
	}

	ax = pop(&scheduled_axioms);
	while (ax != NULL) {

		switch (ax->type) {
		case SUBSUMPTION_CONJUNCTION_INTRODUCTION:
		case SUBSUMPTION_EXISTENTIAL_INTRODUCTION:
			++total_subsumption_count;
			// no conjunction decomposition, no existential decomposition and no bottom rule here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
				++unique_subsumption_count;

				/*
				printf("SUBS:");
				print_concept(ax->lhs);
				printf("->");
				print_concept(ax->rhs);
				printf("\n");
				*/

				add_to_concept_subsumer_list(ax->lhs, ax->rhs);

				// conjunction introduction
				// the first conjunct
				for (i = 0; i < ax->rhs->first_conjunct_of_count; i++) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(ax->lhs, ax->rhs->first_conjunct_of_list[i]->description.conj->conjunct2))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->first_conjunct_of_list[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax->rhs->second_conjunct_of_count; i++) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(ax->lhs, ax->rhs->second_conjunct_of_list[i]->description.conj->conjunct1))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->second_conjunct_of_list[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// existential introduction
				Concept* ex;
				for (i = 0; i < ax->lhs->predecessor_r_count; ++i)
					for (j = 0; j < ax->lhs->predecessors[i]->role->subsumer_count; ++j) {
						ex = GET_NEGATIVE_EXISTS(ax->rhs, ax->lhs->predecessors[i]->role->subsumer_list[j]);
						if (ex != NULL)
							for (k = 0; k < ax->lhs->predecessors[i]->filler_count; ++k)
								push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs->predecessors[i]->fillers[k], ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
					}


				// told subsumers
				for (i = 0; i < ax->rhs->told_subsumer_count; i++)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case SUBSUMPTION_INITIALIZATION:
		case SUBSUMPTION_CONJUNCTION_DECOMPOSITION:
		case SUBSUMPTION_TOLD_SUBSUMER:
		case SUBSUMPTION_BOTTOM:
			++total_subsumption_count;
			// all here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
				++unique_subsumption_count;

				add_to_concept_subsumer_list(ax->lhs, ax->rhs);

				printf("SUBS:");
				print_concept(ax->lhs);
				printf("->");
				print_concept(ax->rhs);
				printf("\n");

				// bottom rule
				if (ax->rhs == tbox->bottom_concept) {
					// If we are checking consistency and an atomic concept is subsumed by bottom
					if (reasoning_task == CONSISTENCY && ax->lhs->type == ATOMIC_CONCEPT)
						// return inconsistent immediately
						return -1;
					// We push the saturation axiom bottom <= ax->lhs, if we already know ax->lhs <= bottom. This way ax->lhs = bottom
					// gets computed. The information bottom <= c is not taken into account for any other concept c.
					push(&scheduled_axioms, create_concept_saturation_axiom(tbox->bottom_concept, ax->lhs, NULL, SUBSUMPTION_BOTTOM));
					for (i = 0; i < ax->lhs->predecessor_r_count; ++i)
						for (j = 0; j < ax->lhs->predecessors[i]->filler_count; ++j)
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs->predecessors[i]->fillers[j], tbox->bottom_concept, NULL, SUBSUMPTION_BOTTOM));
				}


				// conjunction introduction
				// the first conjunct
				int lhs_is_subsumed_by_other_conjunct;
				for (i = 0; i < ax->rhs->first_conjunct_of_count; i++) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(ax->lhs, ax->rhs->first_conjunct_of_list[i]->description.conj->conjunct2))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->first_conjunct_of_list[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax->rhs->second_conjunct_of_count; i++) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(ax->lhs, ax->rhs->second_conjunct_of_list[i]->description.conj->conjunct1))
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
				int j,k;
				Concept* ex;
				for (i = 0; i < ax->lhs->predecessor_r_count; ++i)
					for (j = 0; j < ax->lhs->predecessors[i]->role->subsumer_count; ++j) {
						ex = GET_NEGATIVE_EXISTS(ax->rhs, ax->lhs->predecessors[i]->role->subsumer_list[j]);
						if (ex != NULL)
							for (k = 0; k < ax->lhs->predecessors[i]->filler_count; ++k)
								push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs->predecessors[i]->fillers[k], ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
					}


				// told subsumers
				for (i = 0; i < ax->rhs->told_subsumer_count; i++)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case LINK:
			++total_link_count;
			if (add_successor(ax->lhs, ax->role, ax->rhs, tbox)) {
				add_predecessor(ax->rhs, ax->role, ax->lhs, tbox);
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

				int i, j, k;

				// bottom rule
				if (IS_SUBSUMED_BY(ax->rhs, tbox->bottom_concept))
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, tbox->bottom_concept, NULL, SUBSUMPTION_BOTTOM));


				// existential introduction
				for (i = 0; i < ax->rhs->subsumer_count; ++i)
					for (j = 0; j < ax->role->subsumer_count; ++j) {
						Concept* ex = GET_NEGATIVE_EXISTS(ax->rhs->subsumer_list[i], ax->role->subsumer_list[j]);
						if (ex != NULL)
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
					}


				// the role chain rule
				// the role composition where this role appears as the second component
				for (i = 0; i < ax->role->second_component_of_count; ++i) {

					/*
					 printf("role composition where this role appears as the second component: ");
					 print_role(ax->role->subsumer_list[i]->second_component_of_list[j]);
					 printf("\n");
					 */

					for (j = 0; j < ax->lhs->predecessor_r_count; ++j)
						if (ax->lhs->predecessors[j]->role == ax->role->second_component_of_list[i]->description.role_composition->role1) {
							for (k = 0; k < ax->lhs->predecessors[j]->filler_count; ++k) {
								push(&scheduled_axioms,
										create_concept_saturation_axiom(
												ax->lhs->predecessors[j]->fillers[k],
												ax->rhs,
												ax->role->second_component_of_list[i], LINK));

							}
						}
				}


				// now the same for the successors of the filler of the existential on the rhs
				// the role composition where this role appears as the first component
				for (i = 0; i < ax->role->first_component_of_count; ++i) {

					// printf("role composition where this role appears as the first component: ");
					// print_role(ax->rhs->description.exists->role->subsumer_list[i]->first_component_of_list[j]);
					// printf("\n");

					for (j = 0; j < ax->rhs->successor_r_count; ++j)
						if (ax->rhs->successors[j]->role == ax->role->first_component_of_list[i]->description.role_composition->role2) {
							for (k = 0; k < ax->rhs->successors[j]->filler_count; ++k) {
								push(&scheduled_axioms,
										create_concept_saturation_axiom(
												ax->lhs,
												ax->rhs->successors[j]->fillers[k],
												ax->role->first_component_of_list[i], LINK));

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

	return 0;
}
