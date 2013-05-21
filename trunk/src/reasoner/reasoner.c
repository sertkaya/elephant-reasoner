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
#include "../index/index.h"
#include "../saturation/saturation.h"
#include "../hierarchy/hierarchy.h"
#include "reasoner.h"

extern int yyparse(TBox* tbox);

FILE* yyin;

// total runtime
double total_time = 0.0;

TBox* init_reasoner() {

	TBox* tbox = (TBox*) malloc(sizeof(TBox));
	assert(tbox != NULL);

	tbox->last_concept_id = 0;
	tbox->last_role_id = 0;

	tbox->atomic_concept_count = 0;
	tbox->atomic_concept_list = NULL;
	tbox->atomic_concepts = (Pvoid_t) NULL;

	tbox->atomic_role_count = 0;
	tbox->atomic_roles = (Pvoid_t) NULL;

	tbox->exists_restriction_count = 0;
	tbox->unique_exists_restriction_count = 0;
	tbox->exists_restrictions = (Pvoid_t) NULL;

	tbox->conjunction_count = 0;
	// tbox->unique_conjunction_count = 0;
	tbox->binary_conjunction_count = 0;
	tbox->unique_binary_conjunction_count = 0;
	tbox->conjunctions = (Pvoid_t) NULL;

	tbox->role_composition_count = 0;
	tbox->binary_role_composition_count = 0;
	tbox->unique_binary_role_composition_count = 0;
	tbox->role_compositions = (Pvoid_t) NULL;

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

	return tbox;
}


void read_kb(char* kb_file_name, TBox* tbox) {
	// parser return code
	int parser;

	yyin = fopen(kb_file_name, "r");
	assert(yyin != NULL);

	printf("Loading KB.........................");
	fflush(stdout);
	START_TIMER;
	parser = yyparse(tbox);
	STOP_TIMER;
	total_time += TIME_DIFF;
	if (parser != 0) {
		print_short_stats();
		fprintf(stderr,"aborting\n");
		exit(-1);
	}
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	fclose(yyin);
}

void classify(TBox* tbox) {
	printf("Indexing...........................");
	fflush(stdout);
	START_TIMER;
	index_tbox(tbox);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;

	printf("Saturating..........................");
	fflush(stdout);
	START_TIMER;
	// init_saturation(tbox);
	saturate_tbox(tbox);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;

	printf("Computing concept hierarchy........");
	fflush(stdout);
	START_TIMER;
	compute_concept_hierarchy(tbox);
	STOP_TIMER;
	printf("%.3f milisecs\n", TIME_DIFF / 1000);
	total_time += TIME_DIFF;

	printf("Total time.........................%.3f milisecs\n", total_time / 1000);
}
