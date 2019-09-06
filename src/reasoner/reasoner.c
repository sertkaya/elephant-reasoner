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
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

#include "../model/datatypes.h"
#include "../model/limits.h"
#include "../model/model.h"
#include "../model/print_utils.h"
#include "../preprocessing/preprocessing.h"
#include "../index/index.h"
#include "../saturation/saturation.h"
#include "../hierarchy/hierarchy.h"
#include "../utils/timer.h"
#include "../utils/map.h"
#include "reasoner.h"

// the parser
extern int yyparse(KB* kb);
// file pointer for the parser
extern FILE* yyin;


TBox* init_tbox() {

	TBox* tbox = (TBox*) malloc(sizeof(TBox));
	assert(tbox != NULL);

	tbox->class_expressions = calloc(INITIAL_DEFAULT_CLASS_EXPRESSION_COUNT, sizeof(ClassExpression));
	assert(tbox->class_expressions != NULL);
	tbox->class_expressions_size = INITIAL_DEFAULT_CLASS_EXPRESSION_COUNT;

	tbox->object_property_expressions= calloc(INITIAL_DEFAULT_OBJECT_PROPERTY_EXPRESSION_COUNT, sizeof(ObjectPropertyExpression));
	assert(tbox->object_property_expressions != NULL);
	tbox->object_property_expressions_size = INITIAL_DEFAULT_OBJECT_PROPERTY_EXPRESSION_COUNT;

	// Last assigned class expression id. Increment before using
	// The first id that will be given is 1. 0 is not a valid id.
	tbox->last_class_expression_id = 0;
	// Last assigned object property expression id. Increment before using
	tbox->last_object_property_expression_id = 0;

	MAP_INIT(&(tbox->classes), DEFAULT_ATOMIC_CONCEPTS_HASH_SIZE);

	MAP_INIT(&(tbox->objectproperties), DEFAULT_ATOMIC_ROLES_HASH_SIZE);

	tbox->object_some_values_from_exps_count = 0;
	MAP_INIT(&(tbox->object_some_values_from_exps), DEFAULT_EXISTS_RESTRICTIONS_HASH_SIZE);

	MAP_INIT(&(tbox->object_one_of_exps), DEFAULT_NOMINALS_HASH_SIZE);

	tbox->object_intersection_of_exps_count = 0;
	// tbox->unique_conjunction_count = 0;
	tbox->binary_object_intersection_of_exps_count = 0;
	MAP_INIT(&(tbox->object_intersection_of_exps), DEFAULT_CONJUNCTIONS_HASH_SIZE);

	tbox->role_composition_count = 0;
	tbox->binary_role_composition_count = 0;
	MAP_INIT(&(tbox->objectproperty_chains), DEFAULT_ROLE_COMPOSITIONS_HASH_SIZE);

	SET_INIT_64(&(tbox->subclass_of_axioms), DEFAULT_SUBCLASS_OF_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->equivalent_classes_axioms), DEFAULT_EQUIVALENT_CLASSES_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->disjoint_classes_axioms), DEFAULT_DISJOINT_CLASSES_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->subobjectproperty_of_axioms), DEFAULT_SUBOBJECTPROPERTY_OF_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->equivalent_objectproperties_axioms), DEFAULT_EQUIVALENT_OBJECTPROPERTIES_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->transitive_objectproperty_axioms), DEFAULT_TRANSITIVE_OBJECTPROPERTY_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->objectproperty_domain_axioms), DEFAULT_OBJECTPROPERTY_DOMAIN_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->same_individual_axioms), DEFAULT_SAME_INDIVIDUAL_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->different_individuals_axioms), DEFAULT_DIFFERENT_INDIVIDUALS_AXIOMS_SET_SIZE);

	// add the top and bottom concepts
	tbox->top_concept = get_create_atomic_concept(OWL_THING, tbox);
	tbox->bottom_concept = get_create_atomic_concept(OWL_NOTHING, tbox);

	// init the generated axioms, nominals and exists restrictions
	tbox->generated_object_some_values_from_count = 0;
	MAP_INIT(&(tbox->generated_object_some_values_from_exps), DEFAULT_EXISTS_RESTRICTIONS_HASH_SIZE);
	MAP_INIT(&(tbox->generated_object_one_of_exps), DEFAULT_NOMINALS_HASH_SIZE);

	SET_INIT_64(&(tbox->generated_subclass_of_axioms), DEFAULT_SUBCLASS_OF_AXIOMS_SET_SIZE);
	SET_INIT_64(&(tbox->generated_subobjectproperty_of_axioms), DEFAULT_SUBOBJECTPROPERTY_OF_AXIOMS_SET_SIZE);

	return tbox;
}

ABox* init_abox() {

	ABox* abox = (ABox*) malloc(sizeof(ABox));
	assert(abox != NULL);

	abox->individuals = calloc(DEFAULT_INDIVIDUAL_COUNT, sizeof(Individual));
	assert(abox->individuals != NULL);

	// Last assigned individual id. Increment before using
	// The first id that will be given is 1. 0 is not a valid id.
	abox->last_individual_id = 0;
	abox->individual_count = 0;
	abox->individuals_map = hash_map_create(DEFAULT_INDIVIDUALS_HASH_SIZE);
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

	LIST_INIT_64(&(kb->prefix_names));
	MAP_INIT_64(&(kb->prefixes), DEFAULT_PREFIXES_HASH_SIZE);

	kb->top_occurs_on_lhs = 0;
	kb->bottom_occurs_on_rhs = 0;

	return kb;
}

void read_kb(FILE* input_kb, KB* kb) {
	struct timeval start_time, stop_time;

	// parser return code
	int parser;

	yyin = input_kb;

	printf("Loading KB.........................: ");
	fflush(stdout);
	START_TIMER(start_time);
	// parser = yyparse(tbox, abox);
	parser = yyparse(kb);
	STOP_TIMER(stop_time);
	// total_time += TIME_DIFF(start_time, stop_time);
	if (parser != 0) {
		print_short_stats(kb);
		fprintf(stderr,"aborting\n");
		exit(-1);
	}
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
}

void classify(KB* kb) {
	struct timeval start_time, stop_time;

	// total runtime
	double total_time = 0.0;

	printf("Preprocessing......................: ");
	fflush(stdout);
	START_TIMER(start_time);
	preprocess_kb(kb);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);

	printf("Indexing...........................: ");
	fflush(stdout);
	START_TIMER(start_time);
	char indexing_result = index_kb(kb, CLASSIFICATION);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);

	// return if we during indexing already find out that the ontology is inconsistent
	if (indexing_result == -1) {
		kb->inconsistent = 1;
		printf("Total time.........................: %.3f milisecs\n", total_time / 1000);
		return;
	}

	printf("Saturating.........................: ");
	fflush(stdout);
	START_TIMER(start_time);
	char saturation_result = saturate_tbox(kb, CLASSIFICATION);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);

	// return if we during saturation find out that the ontology is inconsistent
	if (saturation_result == -1){
		kb->inconsistent = 1;
		printf("Total time.........................: %.3f milisecs\n", total_time / 1000);
		return;
	}

	// compute the class hierarchy if the ontology is consistent
	printf("Computing concept hierarchy........: ");
	fflush(stdout);
	START_TIMER(start_time);
	compute_concept_hierarchy(kb);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);

	printf("Total time.........................: %.3f milisecs\n", total_time / 1000);

}

// Returns
//	0: if the kb is consistent
//	1: it it is inconsistent
char check_consistency(KB* kb) {
	struct timeval start_time, stop_time;
	// total runtime
	double total_time = 0.0;

	printf("Preprocessing......................: ");
	fflush(stdout);
	START_TIMER(start_time);
	preprocess_kb(kb);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);

	printf("Indexing...........................: ");
	fflush(stdout);
	START_TIMER(start_time);
	char indexing_result = index_kb(kb, CONSISTENCY);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);
	// Return inconsistent if indexing returned inconsistent
	if (indexing_result == -1) {
		kb->inconsistent = 1;
		printf("Total time.........................: %.3f milisecs\n", total_time / 1000);
		return 1;
	}
	// Return consistent if bottom does not appear on the rhs of an axiom
	// (indexing returns 1 in this case)
	if (indexing_result == 1) {
		printf("Total time.........................: %.3f milisecs\n", total_time / 1000);
		return 0;
	}

	// Indexing did not provide enough information for checking consistency.
	// Saturate the KB.
	printf("Saturating.........................: ");
	fflush(stdout);
	START_TIMER(start_time);
	char saturation_result = saturate_tbox(kb, CONSISTENCY);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);
	printf("Total time.........................: %.3f milisecs\n", total_time / 1000);
	// return inconsistent if saturation returned inconsistent
	if (saturation_result == -1) {
		kb->inconsistent = 1;
		return 1;
	}

	return 0;
}

// Returns
//	0: if the kb is consistent
//	1: it it is inconsistent
char realize_kb(KB* kb) {
	struct timeval start_time, stop_time;
	// total runtime
	double total_time = 0.0;

	printf("Preprocessing......................: ");
	fflush(stdout);
	START_TIMER(start_time);
	preprocess_kb(kb);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);

	printf("Indexing...........................: ");
	fflush(stdout);
	START_TIMER(start_time);
	char indexing_result = index_kb(kb, REALISATION);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);
	// Return inconsistent if indexing returned inconsistent
	if (indexing_result == -1) {
		kb->inconsistent = 1;
		printf("Total time.........................: %.3f milisecs\n", total_time / 1000);
		return 1;
	}

	printf("Saturating.........................: ");
	fflush(stdout);
	START_TIMER(start_time);
	char saturation_result = saturate_tbox(kb, REALISATION);
	STOP_TIMER(stop_time);
	printf("%.3f milisecs\n", TIME_DIFF(start_time, stop_time) / 1000);
	total_time += TIME_DIFF(start_time, stop_time);
	// return inconsistent if saturation returned inconsistent
	if (saturation_result == -1) {
		printf("Total time.........................: %.3f milisecs\n", total_time / 1000);
		kb->inconsistent = 1;
		return 1;
	}

	printf("Total time.........................: %.3f milisecs\n", total_time / 1000);

	return 0;
}
