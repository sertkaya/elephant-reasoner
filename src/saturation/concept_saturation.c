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
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../model/datatypes.h"
#include "../model/model.h"
#include "../model/utils.h"
#include "../model/limits.h"
#include "../model/print_utils.h"
#include "../utils/stack.h"
#include "../index/utils.h"
#include "../hashing/hash_table.h"
#include "../hashing/hash_map.h"
#include "utils.h"

// for statistics
int saturation_unique_subsumption_count = 0, saturation_total_subsumption_count = 0;
int saturation_unique_link_count = 0, saturation_total_link_count = 0;

// marks the axiom with the premise lhs and conclusion rhs as processed
#define MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)		SET_ADD(ax->rhs, &(ax->lhs->subsumers))

static inline void print_saturation_axiom(KB* kb, ConceptSaturationAxiom* ax) {
	printf("%d: ", ax->type);
	char* lhs_str = class_expression_to_string(kb, ax->lhs);
	char* rhs_str = class_expression_to_string(kb, ax->rhs);
	if (ax->type == LINK) {
		char* role_str = object_property_expression_to_string(kb, ax->role);
		printf("%s -> %s -> %s\n", lhs_str, rhs_str, role_str);
		free(role_str);
	}
	else {
		printf("%s <= %s\n", lhs_str, rhs_str);
	}
	free(lhs_str);
	free(rhs_str);
}

static inline ConceptSaturationAxiom* create_concept_saturation_axiom(ClassExpression* lhs, ClassExpression* rhs, ObjectPropertyExpression* role, enum saturation_axiom_type type) {
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
 * 	-1: If the KB is inconsistent. In this case it immediately returns, i.e., saturation process
 * 	is cancelled.
 * 	0: Otherwise
 */
char saturate_concepts(KB* kb, ReasoningTask reasoning_task) {
	TBox* tbox = kb->tbox;

	ConceptSaturationAxiom* ax;
	Stack scheduled_axioms;

	// initialize the stack
	init_stack(&scheduled_axioms);

	// initialization axioms from classes
	MapIterator iterator;
	MAP_ITERATOR_INIT(&iterator, &(tbox->classes));
	void* class = MAP_ITERATOR_NEXT(&iterator);
	while (class) {
		push(&scheduled_axioms, create_concept_saturation_axiom((ClassExpression*) class, ((ClassExpression*) class), NULL, SUBSUMPTION_INITIALIZATION));
		if (kb->top_occurs_on_lhs) {
			push(&scheduled_axioms, create_concept_saturation_axiom((ClassExpression*) class, tbox->top_concept, NULL, SUBSUMPTION_INITIALIZATION));
		}
		class = MAP_ITERATOR_NEXT(&iterator);
	}

	// Nominals (created from ABox individuals) take place in the saturation only if either:
	// - the reasoning task is realisation, or
	// - the reasoning task is classification and owl:Nothing occurs on the rhs of an axiom
	// - the reasoning task is consistency and owl:Nothing occurs on the rhs of an axiom
	// (in the second and third cases, there is the possibility that the ontology is inconsistent since an indivial has the type owl:Nothing)
	if (reasoning_task == REALISATION || (reasoning_task == CLASSIFICATION && kb->bottom_occurs_on_rhs) || (reasoning_task == CONSISTENCY && kb->bottom_occurs_on_rhs)) {
		// Traverse the map of nominals that are generated during preprocessing.
		MAP_ITERATOR_INIT(&iterator, &(kb->generated_nominals));
		ClassExpression* nominal = (ClassExpression*) MAP_ITERATOR_NEXT(&iterator);
		// The input axioms generated from concept and role assertions
		while (nominal) {
			// add owl:Thing manually to the subsumers of the generated nominals
			SET_ADD(kb->tbox->top_concept, &(nominal->subsumers));

			push(&scheduled_axioms, create_concept_saturation_axiom((ClassExpression*) nominal, ((ClassExpression*) nominal), NULL, SUBSUMPTION_INITIALIZATION));
			if (kb->top_occurs_on_lhs) {
				push(&scheduled_axioms, create_concept_saturation_axiom((ClassExpression*) nominal, tbox->top_concept, NULL, SUBSUMPTION_INITIALIZATION));
			}
			nominal = (ClassExpression*) MAP_ITERATOR_NEXT(&iterator);
		}
	}

	int i, j, k;
	ax = pop(&scheduled_axioms);
	while (ax != NULL) {
		switch (ax->type) {
		case SUBSUMPTION_CONJUNCTION_INTRODUCTION:
		case SUBSUMPTION_EXISTENTIAL_INTRODUCTION:
			++saturation_total_subsumption_count;
			// no conjunction decomposition, no existential decomposition and no bottom rule here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
				++saturation_unique_subsumption_count;

				// print_saturation_axiom(kb, ax);

				// conjunction introduction
				// the first conjunct
				for (i = 0; i < ax->rhs->first_conjunct_of_list.size; ++i) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(ax->lhs, ((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i])->description.conj.conjunct2))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, (ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax->rhs->second_conjunct_of_list.size; ++i) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(ax->lhs, ((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i])->description.conj.conjunct1))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, (ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// existential introduction
				ClassExpression* ex;
				for (i = 0; i < ax->lhs->predecessor_r_count; ++i)
					for (j = 0; j < ax->lhs->predecessors[i].role->subsumer_list.size; ++j) {
						ex = GET_NEGATIVE_EXISTS(ax->rhs, ((ObjectPropertyExpression*) ax->lhs->predecessors[i].role->subsumer_list.elements[j]));
						if (ex != NULL)
							for (k = 0; k < ax->lhs->predecessors[i].filler_count; ++k)
								push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs->predecessors[i].fillers[k], ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
					}


				// told subsumers
				for (i = 0; i < ax->rhs->told_subsumers.size; ++i)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers.elements[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case SUBSUMPTION_INITIALIZATION:
		case SUBSUMPTION_CONJUNCTION_DECOMPOSITION:
		case SUBSUMPTION_TOLD_SUBSUMER:
		case SUBSUMPTION_BOTTOM:
			++saturation_total_subsumption_count;
			// all here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
				++saturation_unique_subsumption_count;

				// print_saturation_axiom(kb, ax);

				// bottom rule
				if (ax->rhs == tbox->bottom_concept) {
					// If the top concept or a nominal is subsumed by bottom, the kb is inconsistent
					if (ax->lhs->type == OBJECT_ONE_OF_TYPE || ax->lhs == tbox->top_concept)
						// return inconsistent immediately
						return -1;
					// We push the saturation axiom bottom <= ax->lhs, if we already know ax->lhs <= bottom. This way ax->lhs = bottom
					// gets computed. The information bottom <= c is not taken into account for any other concept c.
					push(&scheduled_axioms, create_concept_saturation_axiom(tbox->bottom_concept, ax->lhs, NULL, SUBSUMPTION_BOTTOM));
					for (i = 0; i < ax->lhs->predecessor_r_count; ++i)
						for (j = 0; j < ax->lhs->predecessors[i].filler_count; ++j)
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs->predecessors[i].fillers[j], tbox->bottom_concept, NULL, SUBSUMPTION_BOTTOM));
				}


				// conjunction introduction
				// the first conjunct
				for (i = 0; i < ax->rhs->first_conjunct_of_list.size; ++i) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(ax->lhs, ((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i])->description.conj.conjunct2))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, (ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax->rhs->second_conjunct_of_list.size; ++i) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(ax->lhs, ((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i])->description.conj.conjunct1))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, (ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				switch (ax->rhs->type) {
				case OBJECT_INTERSECTION_OF_TYPE:
					// conjunction decomposition
					// conjunct 1 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj.conjunct1, NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));

					// conjunct 2 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.conj.conjunct2, NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));
					break;
				case OBJECT_SOME_VALUES_FROM_TYPE:
					// existential decomposition
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->description.exists.filler, ax->rhs->description.exists.role, LINK));
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs->description.exists.filler, ax->rhs->description.exists.filler, NULL, SUBSUMPTION_INITIALIZATION));
					break;
				}

				// existential introduction
				int j,k;
				ClassExpression* ex;
				for (i = 0; i < ax->lhs->predecessor_r_count; ++i)
					for (j = 0; j < ax->lhs->predecessors[i].role->subsumer_list.size; ++j) {
						ex = GET_NEGATIVE_EXISTS(ax->rhs, ((ObjectPropertyExpression*) ax->lhs->predecessors[i].role->subsumer_list.elements[j]));
						if (ex != NULL)
							for (k = 0; k < ax->lhs->predecessors[i].filler_count; ++k)
								push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs->predecessors[i].fillers[k], ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
					}


				// told subsumers
				for (i = 0; i < ax->rhs->told_subsumers.size; ++i)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax->rhs->told_subsumers.elements[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case LINK:
			++saturation_total_link_count;
			if (add_successor(ax->lhs, ax->role, ax->rhs, tbox)) {
				add_predecessor(ax->rhs, ax->role, ax->lhs, tbox);
				++saturation_unique_link_count;

				// print_saturation_axiom(kb, ax);

				// init
				// push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));

				int i, j, k;

				// bottom rule
				if (IS_SUBSUMED_BY(ax->rhs, tbox->bottom_concept))
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, tbox->bottom_concept, NULL, SUBSUMPTION_BOTTOM));


				// existential introduction
				SetIterator subsumers_iterator;
				SET_ITERATOR_INIT(&subsumers_iterator, &(ax->rhs->subsumers));
				void* subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				// TODO: change the order of the loops for better performance
				while (subsumer != NULL) {
					// printf("!!! subsumer %s of %s\n", class_expression_to_string(kb, subsumer), class_expression_to_string(kb, ax->rhs));
					for (j = 0; j < ax->role->subsumer_list.size; ++j) {
						// printf("### role subsumer:%s\n", object_property_expression_to_string(kb, (ObjectPropertyExpression*) ax->role->subsumer_list.elements[j]));
						ClassExpression* ex = GET_NEGATIVE_EXISTS(((ClassExpression*) subsumer), ((ObjectPropertyExpression*) ax->role->subsumer_list.elements[j]));
						if (ex != NULL) {
							// printf("$$$ ex:%s\n", class_expression_to_string(kb, ex));
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
						}
					}
					subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
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
						if (ax->lhs->predecessors[j].role == ax->role->second_component_of_list[i]->description.object_property_chain.role1) {
							for (k = 0; k < ax->lhs->predecessors[j].filler_count; ++k) {
								int l;
								for (l = 0; l < ax->role->second_component_of_list[i]->subsumer_list.size; ++l) {
									// printf("**%s %s\n", object_property_expression_to_string(kb, (ObjectPropertyExpression*) ax->role->second_component_of_list[i]), object_property_expression_to_string(kb, (ObjectPropertyExpression*) ax->role->second_component_of_list[i]->subsumer_list.elements[l]));
									push(&scheduled_axioms,
											create_concept_saturation_axiom(
													ax->lhs->predecessors[j].fillers[k],
													ax->rhs,
													// ax->role->second_component_of_list[i], LINK));
													(ObjectPropertyExpression*) ax->role->second_component_of_list[i]->subsumer_list.elements[l], LINK));
									push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));

								}
								/*
								SET_ITERATOR_INIT(&subsumers_iterator, &(ax->role->second_component_of_list[i]->subsumers));
								subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
								while (subsumer) {
									push(&scheduled_axioms,
											create_concept_saturation_axiom(
													ax->lhs->predecessors[j].fillers[k],
													ax->rhs,
													(ObjectPropertyExpression*) subsumer, LINK));
									subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
								}
								*/
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
						if (ax->rhs->successors[j].role == ax->role->first_component_of_list[i]->description.object_property_chain.role2) {
							for (k = 0; k < ax->rhs->successors[j].filler_count; ++k) {
								int l;
								for (l = 0; l < ax->role->first_component_of_list[i]->subsumer_list.size; ++l) {
									push(&scheduled_axioms,
											create_concept_saturation_axiom(
													ax->lhs,
													ax->rhs->successors[j].fillers[k],
													// ax->role->first_component_of_list[i], LINK));
													(ObjectPropertyExpression*) ax->role->first_component_of_list[i]->subsumer_list.elements[l], LINK));
									push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs->successors[j].fillers[k], ax->rhs->successors[j].fillers[k], NULL, SUBSUMPTION_INITIALIZATION));
								}
								/*
								SET_ITERATOR_INIT(&subsumers_iterator, &(ax->role->first_component_of_list[i]->subsumers));
								subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
								while (subsumer) {
									push(&scheduled_axioms,
											create_concept_saturation_axiom(
													ax->lhs,
													ax->rhs->successors[j].fillers[k],
													(ObjectPropertyExpression*) subsumer, LINK));
									subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
								}
								*/
							}
						}
				}


				if (kb->top_occurs_on_lhs) {
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, tbox->top_concept, NULL, SUBSUMPTION_INITIALIZATION));
				}

			}
			break;
		default:
			fprintf(stderr, "Unknown saturation axiom type, aborting.");
			exit(EXIT_FAILURE);
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}
	// printf("Total subsumptions:%d\nUnique subsumptions:%d\n", saturation_total_subsumption_count, saturation_unique_subsumption_count);
	// printf("Total links:%d\nUnique links:%d\n", saturation_total_link_count, saturation_unique_link_count);

	return 0;
}
