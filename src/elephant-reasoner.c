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
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>

#include "reasoner/reasoner.h"
#include "model/tbox/datatypes.h"
#include "model/abox/datatypes.h"
#include "model/tbox/memory_utils.h"
#include "model/abox/memory_utils.h"
#include "model/print_utils.h"

void usage(char* program) {
	fprintf(stderr, "Usage: %s -i ontology -o output -c\n", program);
}

int main(int argc, char *argv[]) {
	FILE* input_kb;
	FILE* output_taxonomy;

	if (argc <= 2) {
		fprintf(stderr,"Usage: %s input_kb output_taxonomy\n", argv[0]);
		return -1;
	}

	// extern char *optarg;
	// extern int optind;
	int c, reasoning_task_flag = 0, input_flag = 0, output_flag = 0, wrong_argument_flag = 0, verbose_flag = 0;
	char *reasoning_task = "", *ontology_file = "", *output_file = "";
	static char usage[] = "Usage: %s -i ontology -o output -r[classification|realisation|consistency]\n";
	while ((c = getopt(argc, argv, "r:i:o:v")) != -1)
		switch (c) {
		case 'r':
			reasoning_task_flag = 1;
			reasoning_task = optarg;
			break;
		case 'i':
			input_flag = 1;
			ontology_file = optarg;
			break;
		case 'o':
			output_flag = 1;
			output_file = optarg;
			break;
		case 'v':
			verbose_flag = 1;
			break;
		case '?':
			wrong_argument_flag = 1;
			break;
		}
	if (input_flag == 0) {
		fprintf(stderr, "%s: Provide an input ontology\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(EXIT_FAILURE);
	}
	if (output_flag == 0) {
		fprintf(stderr, "%s: Provide an output file\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(EXIT_FAILURE);
	}
	if (reasoning_task_flag == 0) {
		fprintf(stderr, "%s: Provide one of the reasoning tasks: classification | realisation | consistency\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(EXIT_FAILURE);
	}
	if (strcmp(reasoning_task, "classification") != 0 && strcmp(reasoning_task, "realisation") != 0 && strcmp(reasoning_task, "consistency") != 0) {
		fprintf(stderr, "%s: Provide one of the reasoning tasks: classification | realisation | consistency\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(EXIT_FAILURE);
	}
	if (wrong_argument_flag) {
		fprintf(stderr, usage, argv[0]);
		exit(EXIT_FAILURE);
	}
	// the input kb file
	input_kb = fopen(ontology_file, "r");
	assert(input_kb != NULL);

	// the output taxonomy file
	output_taxonomy = fopen(output_file, "w");
	assert(output_taxonomy != NULL);

	// TBox* tbox = NULL;
	// ABox* abox = NULL;
	// initialize global variables, allocate space
	// tbox = init_tbox();
	// abox = init_abox();
	KB* kb = init_kb();

	// read and parse the kb
	// read_kb(input_kb, tbox, abox);
	read_kb(input_kb, kb);
	fclose(input_kb);

	// classify the kb
	if (!strcmp(reasoning_task, "classification"))
		// classify(tbox);
		classify(kb->tbox);

	// display kb information
	if (verbose_flag)
		// print_short_stats(tbox, abox);
		print_short_stats(kb);

	// print the concept hierarchy
	print_concept_hierarchy(kb->tbox, output_taxonomy);
	fclose(output_taxonomy);

	// free the kb
	int freed_bytes = free_tbox(kb->tbox);
	freed_bytes += free_abox(kb->abox);
	printf("Freed bytes:%d\n", freed_bytes);

	return 0;
}

