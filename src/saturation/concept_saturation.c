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
#define MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)		SET_ADD(ax->rhs, &(ax_lhs->subsumers))

static inline void print_saturation_axiom(KB* kb, ConceptSaturationAxiom* ax) {
	printf("\n%d: ", ax->type);
	char* lhs_str = class_expression_to_string(kb, ax->lhs);
	char* rhs_str = class_expression_to_string(kb, ax->rhs);
	if (ax->type == LINK) {
		char* role_str = object_property_expression_to_string(kb, ax->role);
		printf("%s -> %s -> %s\n", lhs_str, rhs_str, role_str);
		free(role_str);
	}
	else {
		printf("%s => %s\n", lhs_str, rhs_str);
	}
	free(lhs_str);
	free(rhs_str);
}

static inline ConceptSaturationAxiom* create_concept_saturation_axiom(ClassExpressionId lhs, ClassExpressionId rhs, ObjectPropertyExpressionId role, enum saturation_axiom_type type) {
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
	ClassExpressionId class = (ClassExpressionId) MAP_ITERATOR_NEXT(&iterator);
	while (class != KEY_NOT_FOUND_IN_HASH_MAP) {
		push(&scheduled_axioms, create_concept_saturation_axiom(class, class, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
		if (kb->top_occurs_on_lhs) {
			push(&scheduled_axioms, create_concept_saturation_axiom(class, tbox->top_concept, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
		}
		class = (ClassExpressionId) MAP_ITERATOR_NEXT(&iterator);
	}

	// Nominals (created from ABox individuals) take place in the saturation only if either:
	// - the reasoning task is realisation, or
	// - the reasoning task is classification and owl:Nothing occurs on the rhs of an axiom
	// - the reasoning task is consistency and owl:Nothing occurs on the rhs of an axiom
	// (in the second and third cases, there is the possibility that the ontology is inconsistent since an indivial has the type owl:Nothing)
	// if (reasoning_task == REALISATION || (reasoning_task == CLASSIFICATION && kb->bottom_occurs_on_rhs) || (reasoning_task == CONSISTENCY && kb->bottom_occurs_on_rhs)) {
	// 	// Traverse the map of nominals that are generated during preprocessing.
	// 	MAP_ITERATOR_INIT(&iterator, &(kb->tbox->generated_object_one_of_exps));
	// 	ClassExpressionId nominal =  MAP_ITERATOR_NEXT(&iterator);
	// 	// The input axioms generated from concept and role assertions
	// 	while (nominal != KEY_NOT_FOUND_IN_HASH_MAP) {
	// 		// add owl:Thing manually to the subsumers of the generated nominals
	// 		SET_ADD(kb->tbox->top_concept, &(tbox->class_expressions[nominal].subsumers));

	// 		push(&scheduled_axioms, create_concept_saturation_axiom(nominal, nominal, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
	// 		if (kb->top_occurs_on_lhs) {
	// 			push(&scheduled_axioms, create_concept_saturation_axiom(nominal, tbox->top_concept, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
	// 		}
	// 		nominal = MAP_ITERATOR_NEXT(&iterator);
	// 	}
	// }

	int i, j;
	ax = pop(&scheduled_axioms);
	ClassExpression *ax_lhs, *ax_rhs;
	while (ax != NULL) {
		ax_lhs = &(tbox->class_expressions[ax->lhs]);
		ax_rhs = &(tbox->class_expressions[ax->rhs]);
		switch (ax->type) {
		case SUBSUMPTION_EXISTENTIAL_INTRODUCTION_1:
		case SUBSUMPTION_EXISTENTIAL_INTRODUCTION_2:
			// do not apply the existential decomposition rule
			// do not apply the existential introduction-1 rule
			// existential introduction-2 is required
			// conjunction decomposition cannot be applied anyway since ax->rhs is an existential
			// bottom rule neither due to the same reason
		case SUBSUMPTION_CONJUNCTION_INTRODUCTION:
			// do not apply the conjunction decomposition rule if the axiom is derived using the conjunction introduction rule
			// existential introduction-1,2,3 and existential decomposition cannot be applied anyway, since ax->rhs is a conjunction
			// bottom rule neither due to the same reason
			++saturation_total_subsumption_count;
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
				++saturation_unique_subsumption_count;

				// print_saturation_axiom(kb, ax);

				// conjunction introduction
				// the first conjunct
				for (i = 0; i < ax_rhs->first_conjunct_of_list.size; ++i) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(ax->lhs, CEXP(ax_rhs->first_conjunct_of_list.elements[i]).description.conj.conjunct2, kb->tbox))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs,  ax_rhs->first_conjunct_of_list.elements[i], EXPRESSION_ID_NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax_rhs->second_conjunct_of_list.size; ++i) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(ax->lhs, CEXP(ax_rhs->second_conjunct_of_list.elements[i]).description.conj.conjunct1, kb->tbox))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs,  ax_rhs->second_conjunct_of_list.elements[i], EXPRESSION_ID_NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// existential introduction
				ClassExpressionId ex;
				for (i = 0; i < ax_lhs->predecessor_r_count; ++i) {
					for (j = 0; j < OPEXP(ax_lhs->predecessors[i].role).subsumer_list.size; ++j) {
						// ex = GET_NEGATIVE_EXISTS(ax->rhs, ax_lhs->predecessors[i].role, tbox);
						ex = GET_NEGATIVE_EXISTS(ax->rhs, OPEXP(ax_lhs->predecessors[i].role).subsumer_list.elements[j], tbox);
						if (ex != KEY_NOT_FOUND_IN_HASH_MAP) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(ax_lhs->predecessors[i].fillers));
							// !!!
							// SET_ITERATOR_INIT(&predecessors_iterator, &(ax_lhs->predecessors[i].fillers_not_exist_introduction));
							ClassExpressionId predecessor =  SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != HASH_TABLE_KEY_NOT_FOUND) {
								// push if the ax_lhs->predecessors[i].role predecessor of ax->lhs is not obtained using SUBSUMPTION_EXISTENTIAL_INTRODUCTION_1 rule.
								// push if the function check_predecessor(ax->lhs, predecessor, ax_lhs->predecessors[i].role) returns false.
								push(&scheduled_axioms, create_concept_saturation_axiom(predecessor, ex, EXPRESSION_ID_NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION_2));
								predecessor =  SET_ITERATOR_NEXT(&predecessors_iterator);
							}
						}
					}
				}

				// told subsumers
				for (i = 0; i < ax_rhs->told_subsumers.size; ++i)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax_rhs->told_subsumers.elements[i], EXPRESSION_ID_NULL, SUBSUMPTION_TOLD_SUBSUMER));

			}
			// else {
			// 	print_saturation_axiom(kb, ax);
			// }
			break;
		case LINK:
			++saturation_total_link_count;
			if (add_successor(ax->lhs, ax->role, ax->rhs, tbox)) {
				add_predecessor(ax->rhs, ax->role, ax->lhs, tbox);
				++saturation_unique_link_count;

				// print_saturation_axiom(kb, ax);


				// bottom rule
				if (IS_SUBSUMED_BY(ax->rhs, tbox->bottom_concept, kb->tbox))
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, tbox->bottom_concept, EXPRESSION_ID_NULL, SUBSUMPTION_BOTTOM));

				// existential introduction
				SetIterator subsumers_iterator;
				SET_ITERATOR_INIT(&subsumers_iterator, &(ax_rhs->subsumers));
				ClassExpressionId subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				// TODO: change the order of the loops for better performance
				while (subsumer != HASH_TABLE_KEY_NOT_FOUND) {
					for (j = 0; j < OPEXP(ax->role).subsumer_list.size; ++j) {
						ClassExpressionId ex = GET_NEGATIVE_EXISTS(subsumer,  OPEXP(ax->role).subsumer_list.elements[j], tbox);
						if (ex != KEY_NOT_FOUND_IN_HASH_MAP) {
							push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ex, EXPRESSION_ID_NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION_1));
						}
					}
					subsumer = SET_ITERATOR_NEXT(&subsumers_iterator);
				}


				// the role chain rule
				// the role composition where this role appears as the second component
				// printf("second component: %s\n", object_property_expression_to_string(kb, role));
				int i, j;
				for (i = 0; i < OPEXP(ax->role).second_component_of_count; ++i) {
					// printf("role chain: %s\n", object_property_expression_to_string(kb, OPEXP(role).second_component_of_list[i]));
					for (j = 0; j < ax_lhs->predecessor_r_count; ++j)
						if (ax_lhs->predecessors[j].role == OPEXP(OPEXP(ax->role).second_component_of_list[i]).description.object_property_chain.role1) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(ax_lhs->predecessors[j].fillers));
							ClassExpressionId predecessor =  SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != HASH_TABLE_KEY_NOT_FOUND) {
								int l;
								for (l = 0; l < OPEXP(OPEXP(ax->role).second_component_of_list[i]).subsumer_list.size; ++l) {
									push(&scheduled_axioms,
											create_concept_saturation_axiom(predecessor, ax->rhs, OPEXP(OPEXP(ax->role).second_component_of_list[i]).subsumer_list.elements[l], LINK));
									// push(&scheduled_axioms,
									// 		create_concept_saturation_axiom(predecessor, ax->rhs, OPEXP(OPEXP(ax->role).second_component_of_list[i]).subsumer_list.elements[l], BACKWARD_LINK));
									// TODO: Is this needed?
									// push(&scheduled_axioms, create_concept_saturation_axiom(CEXP(ex).description.exists.filler, CEXP(ex).description.exists.filler, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
									// push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
								}
								/*
								push(&scheduled_axioms,
										create_concept_saturation_axiom(predecessor, ax->rhs, OPEXP(ax->role).second_component_of_list[i], LINK));
								// TODO: Is this needed?
								// push(&scheduled_axioms, create_concept_saturation_axiom(CEXP(ex).description.exists.filler, CEXP(ex).description.exists.filler, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
								push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
								*/

								predecessor = SET_ITERATOR_NEXT(&predecessors_iterator);
							}
						}
				}


				// now the same for the successors of the filler of the existential on the rhs
				// the role composition where this role appears as the first component
				// ClassExpressionId filler = ax_rhs->description.exists.filler;

				for (i = 0; i < OPEXP(ax->role).first_component_of_count; ++i) {
					// for (j = 0; j < CEXP(filler).successor_r_count; ++j)
					for (j = 0; j < ax_rhs->successor_r_count; ++j)
						// if (CEXP(filler).successors[j].role == OPEXP(OPEXP(ax->role).first_component_of_list[i]).description.object_property_chain.role2) {
						if (ax_rhs->successors[j].role == OPEXP(OPEXP(ax->role).first_component_of_list[i]).description.object_property_chain.role2) {
							SetIterator successors_iterator;
							// TODO: fillers not obtained from exists introduction instead?
							// SET_ITERATOR_INIT(&successors_iterator, &(CEXP(filler).successors[j].fillers));
							SET_ITERATOR_INIT(&successors_iterator, &(ax_rhs->successors[j].fillers));
							ClassExpressionId successor =  SET_ITERATOR_NEXT(&successors_iterator);
							while (successor!= HASH_TABLE_KEY_NOT_FOUND) {
								int l;
								for (l = 0; l < OPEXP(OPEXP(ax->role).first_component_of_list[i]).subsumer_list.size; ++l) {
									push(&scheduled_axioms,
											// create_concept_saturation_axiom(ax->lhs, successor,  OPEXP(OPEXP(ax->role).first_component_of_list[i]).subsumer_list.elements[l], BACKWARD_LINK));
											create_concept_saturation_axiom(ax->lhs, successor,  OPEXP(OPEXP(ax->role).first_component_of_list[i]).subsumer_list.elements[l], LINK));
									// TODO: Is this needed?
									// push(&scheduled_axioms, create_concept_saturation_axiom(successor, successor, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
								}
								//
								// push(&scheduled_axioms,
								// 		create_concept_saturation_axiom(ax->lhs, successor,  OPEXP(ax->role).first_component_of_list[i], LINK));
								// // TODO: Is this needed?
								// push(&scheduled_axioms, create_concept_saturation_axiom(successor, successor, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
								//

								successor = SET_ITERATOR_NEXT(&successors_iterator);
							}
						}
				}

				if (kb->top_occurs_on_lhs) {
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, tbox->top_concept, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
				}

				// TODO: Is this needed?
				// push(&scheduled_axioms, create_concept_saturation_axiom(ax->rhs, ax->rhs, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
			}
			// else {
			// 	print_saturation_axiom(kb, ax);
			// }
			break;
		default:
			// apply all rules
			++saturation_total_subsumption_count;
			if (MARK_CONCEPT_SATURATION_AXIOM_PROCESSED(ax)) {
				++saturation_unique_subsumption_count;

				// print_saturation_axiom(kb, ax);

				// bottom rule
				if (ax->rhs == tbox->bottom_concept) {
					// If the top concept or a nominal is subsumed by bottom, the kb is inconsistent
					if (ax_lhs->type == OBJECT_ONE_OF_TYPE || ax->lhs == tbox->top_concept)
						// return inconsistent immediately
						return(-1);
					// We push the saturation axiom bottom <= ax->lhs, if we already know ax->lhs <= bottom. This way ax->lhs = bottom
					// gets computed. The information bottom <= c is not taken into account for any other concept c.
					push(&scheduled_axioms, create_concept_saturation_axiom(tbox->bottom_concept, ax->lhs, EXPRESSION_ID_NULL, SUBSUMPTION_BOTTOM));
					for (i = 0; i < ax_lhs->predecessor_r_count; ++i) {
						SetIterator predecessors_iterator;
						// TODO: !!! which fillers? all or only those that were not obtained by an ex_intro rule?
						SET_ITERATOR_INIT(&predecessors_iterator, &(ax_lhs->predecessors[i].fillers));
						ClassExpressionId predecessor =  SET_ITERATOR_NEXT(&predecessors_iterator);
						while (predecessor != HASH_TABLE_KEY_NOT_FOUND) {
							push(&scheduled_axioms, create_concept_saturation_axiom(predecessor, tbox->bottom_concept, EXPRESSION_ID_NULL, SUBSUMPTION_BOTTOM));
							predecessor = SET_ITERATOR_NEXT(&predecessors_iterator);
						}
					}
				}

				// conjunction introduction
				// the first conjunct
				for (i = 0; i < ax_rhs->first_conjunct_of_list.size; ++i) {
					// check if lhs is subsumed by the second conjunct as well
					if (IS_SUBSUMED_BY(ax->lhs, CEXP(ax_rhs->first_conjunct_of_list.elements[i]).description.conj.conjunct2, kb->tbox))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs,  ax_rhs->first_conjunct_of_list.elements[i], EXPRESSION_ID_NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				// now the same for the second conjunct
				for (i = 0; i < ax_rhs->second_conjunct_of_list.size; ++i) {
					// check if lhs is also subsumed by the first conjunct
					if (IS_SUBSUMED_BY(ax->lhs, CEXP(ax_rhs->second_conjunct_of_list.elements[i]).description.conj.conjunct1, kb->tbox))
						push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs,  ax_rhs->second_conjunct_of_list.elements[i], EXPRESSION_ID_NULL, SUBSUMPTION_CONJUNCTION_INTRODUCTION));
				}

				SetIterator subsumers_iterator;

				switch (ax_rhs->type) {
				case OBJECT_INTERSECTION_OF_TYPE:
					// conjunction decomposition
					// conjunct 1 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax_rhs->description.conj.conjunct1, EXPRESSION_ID_NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));

					// conjunct 2 as rhs
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax_rhs->description.conj.conjunct2, EXPRESSION_ID_NULL, SUBSUMPTION_CONJUNCTION_DECOMPOSITION));
					break;
				case OBJECT_SOME_VALUES_FROM_TYPE:
					// existential decomposition
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax_rhs->description.exists.filler, ax_rhs->description.exists.role, LINK));
					// TODO:
					push(&scheduled_axioms, create_concept_saturation_axiom(ax_rhs->description.exists.filler, ax_rhs->description.exists.filler, EXPRESSION_ID_NULL, SUBSUMPTION_INITIALIZATION));
					break;
				}

				// existential introduction
				ClassExpressionId ex;
				for (i = 0; i < ax_lhs->predecessor_r_count; ++i) {
					for (j = 0; j < OPEXP(ax_lhs->predecessors[i].role).subsumer_list.size; ++j) {
						// ex = GET_NEGATIVE_EXISTS(ax->rhs, ax_lhs->predecessors[i].role, tbox);
						ex = GET_NEGATIVE_EXISTS(ax->rhs, OPEXP(ax_lhs->predecessors[i].role).subsumer_list.elements[j], tbox);
						if (ex != KEY_NOT_FOUND_IN_HASH_MAP) {
							SetIterator predecessors_iterator;
							SET_ITERATOR_INIT(&predecessors_iterator, &(ax_lhs->predecessors[i].fillers));
							// !!!
							// SET_ITERATOR_INIT(&predecessors_iterator, &(ax_lhs->predecessors[i].fillers_not_exist_introduction));
							ClassExpressionId predecessor =  SET_ITERATOR_NEXT(&predecessors_iterator);
							while (predecessor != HASH_TABLE_KEY_NOT_FOUND) {
								// push if the ax_lhs->predecessors[i].role predecessor of ax->lhs is not obtained using SUBSUMPTION_EXISTENTIAL_INTRODUCTION_1 rule.
								// push if the function check_predecessor(ax->lhs, predecessor, ax_lhs->predecessors[i].role) returns false.
								push(&scheduled_axioms, create_concept_saturation_axiom(predecessor, ex, EXPRESSION_ID_NULL, SUBSUMPTION_EXISTENTIAL_INTRODUCTION_2));
								predecessor =  SET_ITERATOR_NEXT(&predecessors_iterator);
							}
						}
					}
				}

				// told subsumers
				for (i = 0; i < ax_rhs->told_subsumers.size; ++i)
					push(&scheduled_axioms, create_concept_saturation_axiom(ax->lhs, ax_rhs->told_subsumers.elements[i], EXPRESSION_ID_NULL, SUBSUMPTION_TOLD_SUBSUMER));
			}
			// else {
			// 	print_saturation_axiom(kb, ax);
			// }
			break;
		}
		free(ax);
		ax = pop(&scheduled_axioms);
	}

	return(0);
}
