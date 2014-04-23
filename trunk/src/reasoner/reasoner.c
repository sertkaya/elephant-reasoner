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
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "../model/datatypes.h"
#include "../model/limits.h"
#include "../model/model.h"
#include "../preprocessing/preprocessing.h"
#include "../index/index.h"
#include "../saturation/saturation.h"
#include "../hierarchy/hierarchy.h"
#include "reasoner.h"

// the parser
// extern int yyparse(TBox* tbox, ABox* abox);
extern int yyparse(KB* kb);
// file pointer for the parser
extern FILE* yyin;

// total runtime
double total_time = 0.0;

TBox* init_tbox() {

	TBox* tbox = (TBox*) malloc(sizeof(TBox));
	assert(tbox != NULL);

	tbox->last_concept_id = 0;
	tbox->last_role_id = 0;

	tbox->atomic_concept_count = 0;
	tbox->atomic_concept_list = NULL;
	tbox->atomic_concepts = create_key_value_hash_table(DEFAULT_ATOMIC_CONCEPTS_HASH_SIZE);

	tbox->atomic_role_count = 0;
	tbox->atomic_roles = create_key_value_hash_table(DEFAULT_ATOMIC_ROLES_HASH_SIZE);

	tbox->exists_restriction_count = 0;
	tbox->unique_exists_restriction_count = 0;
	tbox->exists_restrictions = create_key_value_hash_table(DEFAULT_EXISTS_RESTRICTIONS_HASH_SIZE);

	tbox->nominals = create_key_value_hash_table(DEFAULT_NOMINALS_HASH_SIZE);

	tbox->conjunction_count = 0;
	// tbox->unique_conjunction_count = 0;
	tbox->binary_conjunction_count = 0;
	tbox->unique_binary_conjunction_count = 0;
	tbox->conjunctions = create_key_value_hash_table(DEFAULT_CONJUNCTIONS_HASH_SIZE);

	tbox->role_composition_count = 0;
	tbox->binary_role_composition_count = 0;
	tbox->unique_binary_role_composition_count = 0;
	tbox->role_compositions = create_key_value_hash_table(DEFAULT_ROLE_COMPOSITIONS_HASH_SIZE);

	tbox->subclass_axioms = NULL;
	tbox->subclass_axiom_count = 0;
	tbox->eqclass_axioms = NULL;
	tbox->eqclass_axiom_count = 0;
	tbox->subrole_axioms = NULL;
	tbox->subrole_axiom_count = 0;
	tbox->eqrole_axioms = NULL;
	tbox->eqrole_axiom_count = 0;
	tbox->transitive_role_axioms = NULL;
	tbox->transitive_role_axiom_count = 0;
	tbox->disjointclasses_axioms = NULL;
	tbox->disjointclasses_axiom_count = 0;

	// add the top and bottom concepts
	tbox->top_concept = get_create_atomic_concept(OWL_THING, tbox);
	tbox->bottom_concept = get_create_atomic_concept(OWL_NOTHING, tbox);

	return tbox;
}

ABox* init_abox() {

	ABox* abox = (ABox*) malloc(sizeof(ABox));
	assert(abox != NULL);

	abox->last_individual_id = 0;
	abox->individual_count = 0;
	abox->individuals = create_key_value_hash_table(DEFAULT_INDIVIDUALS_HASH_SIZE);
	// abox->individual_list = NULL;

	abox->concept_assertion_count = 0;
	abox->concept_assertions = NULL;

	abox->role_assertion_count = 0;
	abox->role_assertions = NULL;

	return abox;
}

KB* init_kb() {
	KB* kb = (KB*) malloc(sizeof(KB));
	assert(kb != NULL);

	kb->tbox = init_tbox();
	kb->abox = init_abox();

	kb->inconsistent = 0;

	return kb;
}

void read_kb(FILE* input_kb, KB* kb) {

	// parser return code
	int parser;

	yyin = input_kb;

	printf("Loading KB.........................:");
	fflush(stdout);
	START_TIMER;
	// parser = yyparse(tbox, abox);
	parser = yyparse(kb);
	STOP_TIMER;
	total_time += TIME_DIFF;
	if (parser != 0) {
		print_short_stats();
		fprintf(stderr,"aborting\n");
		exit(-1);
	}
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
}

void classify(KB* kb) {
	printf("Preprocessing......................:");
	fflush(stdout);
	START_TIMER;
	preprocess_tbox(kb->tbox);
	preprocess_abox(kb->abox);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;

	printf("Indexing...........................:");
	fflush(stdout);
	START_TIMER;
	index_tbox(kb->tbox, CLASSIFICATION);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;

	printf("Saturating.........................:");
	fflush(stdout);
	START_TIMER;
	// init_saturation(tbox);
	saturate_tbox(kb->tbox, CLASSIFICATION);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;

	printf("Computing concept hierarchy........:");
	fflush(stdout);
	START_TIMER;
	compute_concept_hierarchy(kb->tbox);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;

	printf("Total time.........................:%.3f milisecs\n", total_time / 1000);
}

// Returns
//	0: if the kb is consistent
//	1: it it is inconsistent
char check_consistency(KB* kb) {

	printf("Preprocessing......................:");
	fflush(stdout);
	START_TIMER;
	preprocess_tbox(kb->tbox);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;

	printf("Indexing...........................:");
	fflush(stdout);
	START_TIMER;
	char indexing_result = index_tbox(kb->tbox, CONSISTENCY);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;
	// Return inconsistent if indexing returned inconsistent
	// if (indexing_result == -1) {
	// 	kb->inconsistent = 1;
	// 	return 1;
	// }
	// Return consistent if bottom does not appear on the rhs of an axiom
	// (indexing returns 1 in this case)
	// else
	if (indexing_result == 1)
		return 0;

	printf("Saturating.........................:");
	fflush(stdout);
	START_TIMER;
	// init_saturation(tbox);
	char saturation_result = saturate_tbox(kb->tbox, CONSISTENCY);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;
	// return inconsistent if saturation returned inconsistent
	if (saturation_result == -1) {
		kb->inconsistent = 1;
		return 1;
	}
	// TODO:
	// here close the concept and role assertions under the saturated TBox
	// check for inconsistency

	return 0;
}

void realize_kb(KB* kb) {

}
