/*
 * The ELepHant Reasoner
 *
 * Copyright (C) Baris Sertkaya (sertkaya@fb2.fra-uas.de)
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
#include <stdlib.h>
#include <pthread.h>
#include "utils.h"
#include "../model/print_utils.h"
#include "../index/utils.h"

// marks the axiom with the premise lhs and conclusion rhs as processed
#define MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(lhs, rhs)		SET_ADD(rhs, &(lhs->subsumers))

/*
static inline ConceptSaturationAxiom* create_concept_saturation_axiom( ClassExpression* rhs, ObjectPropertyExpression* role, enum saturation_axiom_type type) {
	ConceptSaturationAxiom* ax = (ConceptSaturationAxiom*) malloc(sizeof(ConceptSaturationAxiom));
	assert(ax != NULL);
	ax->rhs = rhs;
	ax->role = role;
	ax->type = type;
	return ax;
}
*/

// for statistics
int saturation_unique_subsumption_count = 0, saturation_total_subsumption_count = 0;
int saturation_unique_link_count = 0, saturation_total_link_count = 0;

lstack_t *lhs_stack;

void process_own_axioms(ClassExpression *lhs, KB* kb) {
	ConceptSaturationAxiom *ax;
	TBox *tbox = kb->tbox;
	while ((ax = pop(&(lhs->own_axioms))) != NULL) {
		switch (ax->type) {
		case SUBSUMPTION_CONJUNCTION_INTRODUCTION:
		case SUBSUMPTION_EXISTENTIAL_INTRODUCTION:
			++saturation_total_subsumption_count;
			// no conjunction decomposition, no existential decomposition and no bottom rule here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(lhs, ax->rhs)) {
				++saturation_unique_subsumption_count;

				// conjunction introduction
				// the first conjunct
				int i;
				for (i = 0; i < ax->rhs->first_conjunct_of_list.size; ++i) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(lhs, ((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i])->description.conj.conjunct2))
						push(&(lhs->own_axioms), create_concept_saturation_axiom((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (int i = 0; i < ax->rhs->second_conjunct_of_list.size; ++i) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(lhs, ((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i])->description.conj.conjunct1))
						push(&(lhs->own_axioms), create_concept_saturation_axiom((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// existential introduction
				ClassExpression* ex;
				for (int i = 0; i < lhs->predecessor_r_count; ++i)
					for (int j = 0; j < lhs->predecessors[i].role->subsumer_list.size; ++j) {
						ex = GET_NEGATIVE_EXISTS(ax->rhs, ((ObjectPropertyExpression*) lhs->predecessors[i].role->subsumer_list.elements[j]));
						if (ex != NULL) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(lhs->predecessors[i].fillers));
							ClassExpression* predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != NULL) {
								pthread_mutex_lock(&(predecessor->foreign_axioms.mutex));
								ts_push(&(predecessor->foreign_axioms), create_concept_saturation_axiom(ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
								pthread_mutex_unlock(&(predecessor->foreign_axioms.mutex));
								if (!atomic_flag_test_and_set(&(predecessor->is_active)))
									lstack_push(lhs_stack, predecessor);
								predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							}
						}
					}


				// told subsumers
				for (int i = 0; i < ax->rhs->told_subsumers.size; ++i)
					push(&(lhs->own_axioms), create_concept_saturation_axiom(ax->rhs->told_subsumers.elements[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case SUBSUMPTION_INITIALIZATION:
		case SUBSUMPTION_CONJUNCTION_DECOMPOSITION:
		case SUBSUMPTION_TOLD_SUBSUMER:
		case SUBSUMPTION_BOTTOM:
			++saturation_total_subsumption_count;
			// all here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(lhs, ax->rhs)) {
				++saturation_unique_subsumption_count;

				// bottom rule
				if (ax->rhs == tbox->bottom_concept) {
					// If the top concept or a nominal is subsumed by bottom, the kb is inconsistent
					if (lhs->type == OBJECT_ONE_OF_TYPE ||lhs == tbox->top_concept)
						// TODO: terminate all threads??
						// return inconsistent immediately
						// return -1;
						return;
					// We push the saturation axiom bottom <= ax->lhs, if we already know ax->lhs <= bottom. This way ax->lhs = bottom
					// gets computed. The information bottom <= c is not taken into account for any other concept c.
					pthread_mutex_lock(&(tbox->bottom_concept->foreign_axioms.mutex));
					ts_push(&(tbox->bottom_concept->foreign_axioms), create_concept_saturation_axiom(lhs, NULL, SUBSUMPTION_BOTTOM));
					pthread_mutex_unlock(&(tbox->bottom_concept->foreign_axioms.mutex));
					if (!atomic_flag_test_and_set(&(tbox->bottom_concept->is_active)))
						lstack_push(lhs_stack, tbox->bottom_concept);
					for (int i = 0; i < lhs->predecessor_r_count; ++i) {
						SetIterator predecessors_iterator;
						SET_ITERATOR_INIT(&predecessors_iterator, &(lhs->predecessors[i].fillers));
						ClassExpression* predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
						while (predecessor != NULL) {
							pthread_mutex_lock(&(predecessor->foreign_axioms.mutex));
							ts_push(&(predecessor->foreign_axioms), create_concept_saturation_axiom(tbox->bottom_concept, NULL, SUBSUMPTION_BOTTOM));
							pthread_mutex_unlock(&(predecessor->foreign_axioms.mutex));
							if (!atomic_flag_test_and_set(&(predecessor->is_active)))
								lstack_push(lhs_stack, predecessor);
							predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
						}
					}
				}


				// conjunction introduction
				// the first conjunct
				for (int i = 0; i < ax->rhs->first_conjunct_of_list.size; ++i) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(lhs, ((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i])->description.conj.conjunct2))
						push(&(lhs->own_axioms), create_concept_saturation_axiom((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (int i = 0; i < ax->rhs->second_conjunct_of_list.size; ++i) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(lhs, ((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i])->description.conj.conjunct1))
						push(&(lhs->own_axioms), create_concept_saturation_axiom((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				switch (ax->rhs->type) {
				case OBJECT_INTERSECTION_OF_TYPE:
					// conjunction decomposition
					// conjunct 1 as rhs
					push(&(lhs->own_axioms), create_concept_saturation_axiom( ax->rhs->description.conj.conjunct1, NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));

					// conjunct 2 as rhs
					push(&(lhs->own_axioms), create_concept_saturation_axiom(ax->rhs->description.conj.conjunct2, NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));
					break;
				case OBJECT_SOME_VALUES_FROM_TYPE:
					// existential decomposition
					push(&(lhs->own_axioms), create_concept_saturation_axiom(ax->rhs->description.exists.filler, ax->rhs->description.exists.role, LINK));
					pthread_mutex_lock(&ax->rhs->description.exists.filler->foreign_axioms.mutex);
					ts_push(&(ax->rhs->description.exists.filler->foreign_axioms), create_concept_saturation_axiom( ax->rhs->description.exists.filler, NULL, SUBSUMPTION_INITIALIZATION));
					pthread_mutex_unlock(&ax->rhs->description.exists.filler->foreign_axioms.mutex);
					if (!atomic_flag_test_and_set(&(ax->rhs->description.exists.filler->is_active)))
						lstack_push(lhs_stack, ax->rhs->description.exists.filler);
					break;
				}

				// existential introduction
				int j,k;
				ClassExpression* ex;
				// TODO: Experimental ! Predecessors should be locked !
				// Busy-wait if already locked
				while (atomic_flag_test_and_set(&lhs->is_predecessors_locked));

				for (int i = 0; i < lhs->predecessor_r_count; ++i)
					for (int j = 0; j < lhs->predecessors[i].role->subsumer_list.size; ++j) {
						ex = GET_NEGATIVE_EXISTS(ax->rhs, ((ObjectPropertyExpression*) lhs->predecessors[i].role->subsumer_list.elements[j]));
						if (ex != NULL) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(lhs->predecessors[i].fillers));
							ClassExpression* predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != NULL) {
								pthread_mutex_lock(&(predecessor->foreign_axioms.mutex));
								ts_push(&(predecessor->foreign_axioms), create_concept_saturation_axiom(ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
								pthread_mutex_unlock(&(predecessor->foreign_axioms.mutex));
								if (!atomic_flag_test_and_set(&(predecessor->is_active)))
									lstack_push(lhs_stack, predecessor);
								predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							}
						}
					}
				atomic_flag_clear(&lhs->is_predecessors_locked);


				// told subsumers
				for (int i = 0; i < ax->rhs->told_subsumers.size; ++i)
					push(&(lhs->own_axioms), create_concept_saturation_axiom( ax->rhs->told_subsumers.elements[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case LINK:
			++saturation_total_link_count;
			if (add_successor(lhs, ax->role, ax->rhs, tbox)) {
				 add_predecessor(ax->rhs, ax->role, lhs, tbox);
				++saturation_unique_link_count;

				// int i, j, k;

				// bottom rule
				if (IS_SUBSUMED_BY(ax->rhs, tbox->bottom_concept))
					push(&(lhs->own_axioms), create_concept_saturation_axiom(tbox->bottom_concept, NULL, SUBSUMPTION_BOTTOM));


				// existential introduction
				SetIterator subsumers_iterator;
				SET_ITERATOR_INIT(&subsumers_iterator, &(ax->rhs->subsumers));
				void* subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				// TODO: change the order of the loops for better performance
				while (subsumer != NULL) {
					for (int j = 0; j < ax->role->subsumer_list.size; ++j) {
						ClassExpression* ex = GET_NEGATIVE_EXISTS(((ClassExpression*) subsumer), ((ObjectPropertyExpression*) ax->role->subsumer_list.elements[j]));
						if (ex != NULL) {
							push(&(lhs->own_axioms), create_concept_saturation_axiom(ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
						}
					}
					subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				}

				// the role chain rule
				// the role composition where this role appears as the second component
				for (int i = 0; i < ax->role->second_component_of_count; ++i) {
					for (int j = 0; j < lhs->predecessor_r_count; ++j)
						if (lhs->predecessors[j].role == ax->role->second_component_of_list[i]->description.object_property_chain.role1) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(lhs->predecessors[j].fillers));
							ClassExpression* predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != NULL) {
								pthread_mutex_lock(&(predecessor->foreign_axioms.mutex));
								for (int l = 0; l < ax->role->second_component_of_list[i]->subsumer_list.size; ++l) {
									ts_push(&(predecessor->foreign_axioms), create_concept_saturation_axiom(ax->rhs, (ObjectPropertyExpression*) ax->role->second_component_of_list[i]->subsumer_list.elements[l], LINK));
									// TODO: move the following out of the loop! (done)
									// push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));
								}
								pthread_mutex_unlock(&(predecessor->foreign_axioms.mutex));
								if (!atomic_flag_test_and_set(&(predecessor->is_active)))
									lstack_push(lhs_stack, predecessor);
								predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							}
							// TODO: Check this! Previously it was above, in the loop and has been moved to here. But it can even be moved further out of the two
							// for loops. What about the "if" in this case? Code-restructure?
							// push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));
							pthread_mutex_lock(&(ax->rhs->foreign_axioms.mutex));
							ts_push(&(ax->rhs->foreign_axioms), create_concept_saturation_axiom(ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));
							pthread_mutex_unlock(&(ax->rhs->foreign_axioms.mutex));
							if (!atomic_flag_test_and_set(&(ax->rhs->is_active)))
								lstack_push(lhs_stack, ax->rhs);
						}
				}


				// now the same for the successors of the filler of the existential on the rhs
				// the role composition where this role appears as the first component
				for (int i = 0; i < ax->role->first_component_of_count; ++i) {
					for (int j = 0; j < ax->rhs->successor_r_count; ++j)
						if (ax->rhs->successors[j].role == ax->role->first_component_of_list[i]->description.object_property_chain.role2) {
							SetIterator successors_iterator;
							SET_ITERATOR_INIT(&successors_iterator, &(ax->rhs->successors[j].fillers));
							ClassExpression* successor= (ClassExpression*) SET_ITERATOR_NEXT(&successors_iterator);
							while (successor!= NULL) {
								int l;
								for (l = 0; l < ax->role->first_component_of_list[i]->subsumer_list.size; ++l) {
									push(&(lhs->own_axioms),
											create_concept_saturation_axiom(successor, (ObjectPropertyExpression*) ax->role->first_component_of_list[i]->subsumer_list.elements[l], LINK));
									// TODO: This does not have to be in the for loop! (Moved outside)
									// push(&scheduled_axioms, create_concept_saturation_axiom(successor, successor, NULL, SUBSUMPTION_INITIALIZATION));
								}
								pthread_mutex_lock(&(successor->foreign_axioms.mutex));
								ts_push(&(successor->foreign_axioms), create_concept_saturation_axiom(successor, NULL, SUBSUMPTION_INITIALIZATION));
								pthread_mutex_unlock(&(successor->foreign_axioms.mutex));
								if (!atomic_flag_test_and_set(&(successor->is_active)))
									lstack_push(lhs_stack, successor);
								successor= (ClassExpression*) SET_ITERATOR_NEXT(&successors_iterator);
							}
						}
				}

				if (kb->top_occurs_on_lhs) {
					pthread_mutex_lock(&(ax->rhs->foreign_axioms.mutex));
					ts_push(&(ax->rhs->foreign_axioms), create_concept_saturation_axiom(tbox->top_concept, NULL, SUBSUMPTION_INITIALIZATION));
					pthread_mutex_unlock(&(ax->rhs->foreign_axioms.mutex));
					if (!atomic_flag_test_and_set(&(ax->rhs->is_active)))
						lstack_push(lhs_stack, ax->rhs);
				}

			}
			break;
		default:
			fprintf(stderr, "Unknown saturation axiom type, aborting.");
			exit(EXIT_FAILURE);
		}
		free(ax);
	}
	// printf("Total subsumptions:%d\nUnique subsumptions:%d\n",  saturation_total_subsumption_count, saturation_unique_subsumption_count);
	// printf("Total links:%d\nUnique links:%d\n", saturation_total_link_count, saturation_unique_link_count);
}

void process_foreign_axioms(ClassExpression *lhs, KB* kb) {
	ConceptSaturationAxiom *ax;
	TBox *tbox = kb->tbox;
	pthread_mutex_lock(&(lhs->foreign_axioms.mutex));
	ax = ts_pop(&(lhs->foreign_axioms));
	pthread_mutex_unlock(&(lhs->foreign_axioms.mutex));
	while (ax != NULL) {
		// printf("pop:%s\n", class_expression_to_string(kb,ax->rhs));
		switch (ax->type) {
		case SUBSUMPTION_CONJUNCTION_INTRODUCTION:
		case SUBSUMPTION_EXISTENTIAL_INTRODUCTION:
			++saturation_total_subsumption_count;
			// no conjunction decomposition, no existential decomposition and no bottom rule here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(lhs, ax->rhs)) {
				++saturation_unique_subsumption_count;

				// conjunction introduction
				// the first conjunct
				for (int i = 0; i < ax->rhs->first_conjunct_of_list.size; ++i) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(lhs, ((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i])->description.conj.conjunct2))
						push(&(lhs->own_axioms), create_concept_saturation_axiom((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (int i = 0; i < ax->rhs->second_conjunct_of_list.size; ++i) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(lhs, ((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i])->description.conj.conjunct1))
						push(&(lhs->own_axioms), create_concept_saturation_axiom((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// existential introduction
				ClassExpression* ex;
				for (int i = 0; i < lhs->predecessor_r_count; ++i)
					for (int j = 0; j < lhs->predecessors[i].role->subsumer_list.size; ++j) {
						ex = GET_NEGATIVE_EXISTS(ax->rhs, ((ObjectPropertyExpression*) lhs->predecessors[i].role->subsumer_list.elements[j]));
						if (ex != NULL) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(lhs->predecessors[i].fillers));
							ClassExpression* predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != NULL) {
								pthread_mutex_lock(&(predecessor->foreign_axioms.mutex));
								ts_push(&(predecessor->foreign_axioms), create_concept_saturation_axiom(ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
								pthread_mutex_unlock(&(predecessor->foreign_axioms.mutex));
								if (!atomic_flag_test_and_set(&(predecessor->is_active)))
									lstack_push(lhs_stack, predecessor);
								predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							}
						}
					}


				// told subsumers
				for (int i = 0; i < ax->rhs->told_subsumers.size; ++i)
					push(&(lhs->own_axioms), create_concept_saturation_axiom(ax->rhs->told_subsumers.elements[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case SUBSUMPTION_INITIALIZATION:
		case SUBSUMPTION_CONJUNCTION_DECOMPOSITION:
		case SUBSUMPTION_TOLD_SUBSUMER:
		case SUBSUMPTION_BOTTOM:
			++saturation_total_subsumption_count;
			// all here
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(lhs, ax->rhs)) {
				++saturation_unique_subsumption_count;

				// bottom rule
				if (ax->rhs == tbox->bottom_concept) {
					// If the top concept or a nominal is subsumed by bottom, the kb is inconsistent
					if (lhs->type == OBJECT_ONE_OF_TYPE ||lhs == tbox->top_concept)
						// TODO: terminate all threads??
						// return inconsistent immediately
						// return -1;
						return;
					// We push the saturation axiom bottom <= ax->lhs, if we already know ax->lhs <= bottom. This way ax->lhs = bottom
					// gets computed. The information bottom <= c is not taken into account for any other concept c.
					pthread_mutex_lock(&(tbox->bottom_concept->foreign_axioms.mutex));
					ts_push(&(tbox->bottom_concept->foreign_axioms), create_concept_saturation_axiom(lhs, NULL, SUBSUMPTION_BOTTOM));
					pthread_mutex_unlock(&(tbox->bottom_concept->foreign_axioms.mutex));
					if (!atomic_flag_test_and_set(&(tbox->bottom_concept->is_active)))
						lstack_push(lhs_stack, tbox->bottom_concept);
					for (int i = 0; i < lhs->predecessor_r_count; ++i) {
						SetIterator predecessors_iterator;
						SET_ITERATOR_INIT(&predecessors_iterator, &(lhs->predecessors[i].fillers));
						ClassExpression* predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
						while (predecessor != NULL) {
							pthread_mutex_lock(&(predecessor->foreign_axioms.mutex));
							ts_push(&(predecessor->foreign_axioms), create_concept_saturation_axiom(tbox->bottom_concept, NULL, SUBSUMPTION_BOTTOM));
							pthread_mutex_unlock(&(predecessor->foreign_axioms.mutex));
							if (!atomic_flag_test_and_set(&(predecessor->is_active)))
								lstack_push(lhs_stack, predecessor);
							predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
						}
					}
				}


				// conjunction introduction
				// the first conjunct
				for (int i = 0; i < ax->rhs->first_conjunct_of_list.size; ++i) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(lhs, ((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i])->description.conj.conjunct2))
						push(&(lhs->own_axioms), create_concept_saturation_axiom((ClassExpression*) ax->rhs->first_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (int i = 0; i < ax->rhs->second_conjunct_of_list.size; ++i) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(lhs, ((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i])->description.conj.conjunct1))
						push(&(lhs->own_axioms), create_concept_saturation_axiom((ClassExpression*) ax->rhs->second_conjunct_of_list.elements[i], NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				switch (ax->rhs->type) {
				case OBJECT_INTERSECTION_OF_TYPE:
					// conjunction decomposition
					// conjunct 1 as rhs
					push(&(lhs->own_axioms), create_concept_saturation_axiom( ax->rhs->description.conj.conjunct1, NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));

					// conjunct 2 as rhs
					push(&(lhs->own_axioms), create_concept_saturation_axiom(ax->rhs->description.conj.conjunct2, NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));
					break;
				case OBJECT_SOME_VALUES_FROM_TYPE:
					// existential decomposition
					push(&(lhs->own_axioms), create_concept_saturation_axiom(ax->rhs->description.exists.filler, ax->rhs->description.exists.role, LINK));
					pthread_mutex_lock(&ax->rhs->description.exists.filler->foreign_axioms.mutex);
					ts_push(&(ax->rhs->description.exists.filler->foreign_axioms), create_concept_saturation_axiom( ax->rhs->description.exists.filler, NULL, SUBSUMPTION_INITIALIZATION));
					pthread_mutex_unlock(&ax->rhs->description.exists.filler->foreign_axioms.mutex);
					if (!atomic_flag_test_and_set(&(ax->rhs->description.exists.filler->is_active)))
						lstack_push(lhs_stack, ax->rhs->description.exists.filler);
					break;
				}

				// existential introduction
				int j,k;
				ClassExpression* ex;
				for (int i = 0; i < lhs->predecessor_r_count; ++i)
					for (int j = 0; j < lhs->predecessors[i].role->subsumer_list.size; ++j) {
						ex = GET_NEGATIVE_EXISTS(ax->rhs, ((ObjectPropertyExpression*) lhs->predecessors[i].role->subsumer_list.elements[j]));
						if (ex != NULL) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(lhs->predecessors[i].fillers));
							ClassExpression* predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != NULL) {
								pthread_mutex_lock(&(predecessor->foreign_axioms.mutex));
								ts_push(&(predecessor->foreign_axioms), create_concept_saturation_axiom(ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
								pthread_mutex_unlock(&(predecessor->foreign_axioms.mutex));
								if (!atomic_flag_test_and_set(&(predecessor->is_active)))
									lstack_push(lhs_stack, predecessor);
								predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							}
						}
					}


				// told subsumers
				for (int i = 0; i < ax->rhs->told_subsumers.size; ++i)
					push(&(lhs->own_axioms), create_concept_saturation_axiom( ax->rhs->told_subsumers.elements[i], NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			break;
		case LINK:
			++saturation_total_link_count;
			if (add_successor(lhs, ax->role, ax->rhs, tbox)) {
				 add_predecessor(ax->rhs, ax->role, lhs, tbox);
				++saturation_unique_link_count;

				// int i, j, k;

				// bottom rule
				if (IS_SUBSUMED_BY(ax->rhs, tbox->bottom_concept))
					push(&(lhs->own_axioms), create_concept_saturation_axiom(tbox->bottom_concept, NULL, SUBSUMPTION_BOTTOM));


				// existential introduction
				SetIterator subsumers_iterator;
				SET_ITERATOR_INIT(&subsumers_iterator, &(ax->rhs->subsumers));
				void* subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				// TODO: change the order of the loops for better performance
				while (subsumer != NULL) {
					for (int j = 0; j < ax->role->subsumer_list.size; ++j) {
						ClassExpression* ex = GET_NEGATIVE_EXISTS(((ClassExpression*) subsumer), ((ObjectPropertyExpression*) ax->role->subsumer_list.elements[j]));
						if (ex != NULL) {
							push(&(lhs->own_axioms), create_concept_saturation_axiom(ex, NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION));
						}
					}
					subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				}

				// the role chain rule
				// the role composition where this role appears as the second component
				for (int i = 0; i < ax->role->second_component_of_count; ++i) {
					for (int j = 0; j < lhs->predecessor_r_count; ++j)
						if (lhs->predecessors[j].role == ax->role->second_component_of_list[i]->description.object_property_chain.role1) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(lhs->predecessors[j].fillers));
							ClassExpression* predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != NULL) {
								pthread_mutex_lock(&(predecessor->foreign_axioms.mutex));
								for (int l = 0; l < ax->role->second_component_of_list[i]->subsumer_list.size; ++l) {
									ts_push(&(predecessor->foreign_axioms), create_concept_saturation_axiom(ax->rhs, (ObjectPropertyExpression*) ax->role->second_component_of_list[i]->subsumer_list.elements[l], LINK));
									// TODO: move the following out of the loop! (done)
									// push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));
								}
								pthread_mutex_unlock(&(predecessor->foreign_axioms.mutex));
								if (!atomic_flag_test_and_set(&(predecessor->is_active)))
									lstack_push(lhs_stack, predecessor);
								predecessor = (ClassExpression*) SET_ITERATOR_NEXT(&predecessors_iterator);
							}
							// TODO: Check this! Previously it was above, in the loop.
							// push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));
							pthread_mutex_lock(&(ax->rhs->foreign_axioms.mutex));
							ts_push(&(ax->rhs->foreign_axioms), create_concept_saturation_axiom(ax->rhs, NULL, SUBSUMPTION_INITIALIZATION));
							pthread_mutex_unlock(&(ax->rhs->foreign_axioms.mutex));
							if (!atomic_flag_test_and_set(&(ax->rhs->is_active)))
								lstack_push(lhs_stack, ax->rhs);
						}
				}


				// now the same for the successors of the filler of the existential on the rhs
				// the role composition where this role appears as the first component
				for (int i = 0; i < ax->role->first_component_of_count; ++i) {
					for (int j = 0; j < ax->rhs->successor_r_count; ++j)
						if (ax->rhs->successors[j].role == ax->role->first_component_of_list[i]->description.object_property_chain.role2) {
							SetIterator successors_iterator;
							SET_ITERATOR_INIT(&successors_iterator, &(ax->rhs->successors[j].fillers));
							ClassExpression* successor= (ClassExpression*) SET_ITERATOR_NEXT(&successors_iterator);
							while (successor!= NULL) {
								int l;
								for (l = 0; l < ax->role->first_component_of_list[i]->subsumer_list.size; ++l) {
									push(&(lhs->own_axioms),
											create_concept_saturation_axiom(successor, (ObjectPropertyExpression*) ax->role->first_component_of_list[i]->subsumer_list.elements[l], LINK));
									// TODO: This does not have to be in the for loop! (Moved outside)
									// push(&scheduled_axioms, create_concept_saturation_axiom(successor, successor, NULL, SUBSUMPTION_INITIALIZATION));
								}
								pthread_mutex_lock(&(successor->foreign_axioms.mutex));
								ts_push(&(successor->foreign_axioms), create_concept_saturation_axiom(successor, NULL, SUBSUMPTION_INITIALIZATION));
								pthread_mutex_unlock(&(successor->foreign_axioms.mutex));
								if (!atomic_flag_test_and_set(&(successor->is_active)))
									lstack_push(lhs_stack, successor);
								successor= (ClassExpression*) SET_ITERATOR_NEXT(&successors_iterator);
							}
						}
				}

				if (kb->top_occurs_on_lhs) {
					pthread_mutex_lock(&(ax->rhs->foreign_axioms.mutex));
					ts_push(&(ax->rhs->foreign_axioms), create_concept_saturation_axiom(tbox->top_concept, NULL, SUBSUMPTION_INITIALIZATION));
					pthread_mutex_unlock(&(ax->rhs->foreign_axioms.mutex));
					if (!atomic_flag_test_and_set(&(ax->rhs->is_active)))
						lstack_push(lhs_stack, ax->rhs);
				}

			}
			break;
		default:
			fprintf(stderr, "Unknown saturation axiom type, aborting.");
			exit(EXIT_FAILURE);
		}
		free(ax);
		pthread_mutex_lock(&(lhs->foreign_axioms.mutex));
		ax = ts_pop(&(lhs->foreign_axioms));
		pthread_mutex_unlock(&(lhs->foreign_axioms.mutex));
	}
	// printf("Total subsumptions:%d\nUnique subsumptions:%d\n",  saturation_total_subsumption_count, saturation_unique_subsumption_count);
	// printf("Total links:%d\nUnique links:%d\n", saturation_total_link_count, saturation_unique_link_count);
}

void *saturation_worker(void *job) {

	lhs_stack = ((SaturationJob*) job)->lhs_stack;
	KB *kb = ((SaturationJob*) job)->kb;
	ClassExpression *lhs;
	printf("Thread id: %d\n", ((SaturationJob*) job)->thread_id);

	while ((lhs = lstack_pop(lhs_stack)) != NULL) {
		// first process the own axioms
		process_own_axioms(lhs, kb);
		process_foreign_axioms(lhs, kb);
		atomic_flag_clear(&(lhs->is_active));
		// printf("%s\n", class_expression_to_string(kb, lhs));
		// if ((lhs->own_axioms.size > 0 || lhs->foreign_axioms.size > 0) && !atomic_flag_test_and_set(&(lhs->is_active)))
		// 	lstack_push(lhs_stack, lhs);
	}
}

