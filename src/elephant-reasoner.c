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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>

#include "reasoner/reasoner.h"
#include "model/datatypes.h"
#include "model/memory_utils.h"
#include "model/print_utils.h"

void usage(char* program) {
	fprintf(stderr, "Usage: %s -i ontology -o output -c\n", program);
}

int main(int argc, char *argv[]) {
	FILE* input_ontology;
	FILE* output;

	int c, reasoning_task_flag = 0, input_flag = 0, output_flag = 0, wrong_argument_flag = 0, verbose_flag = 0;
	char *reasoning_task = "", *ontology_file_name = "", *output_file = "";
	static char usage[] = "Usage: %s -i ontology -o output -r[classification|realisation|consistency]\n";
	while ((c = getopt(argc, argv, "r:i:o:v")) != -1)
		switch (c) {
		case 'r':
			reasoning_task_flag = 1;
			reasoning_task = optarg;
			break;
		case 'i':
			input_flag = 1;
			ontology_file_name = optarg;
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
	// open the ontology file
	input_ontology = fopen(ontology_file_name, "r");
	assert(input_ontology != NULL);

	// initialize global variables, allocate space for the ontology
	KB* kb = init_kb();

	// read and parse the ontology
	read_kb(input_ontology, kb);
	fclose(input_ontology);

	// display kb information
	if (verbose_flag) {
		print_short_stats(kb);
	}

	// open the output file
	output = fopen(output_file, "w");
	assert(output != NULL);

	if (!strcmp(reasoning_task, "classification")) {
		// classify the ontology
		classify(kb);
		// print the class hierarchy to the output file
		print_concept_hierarchy(kb, output);
	}
	else if (!strcmp(reasoning_task, "consistency")) {
		if (check_consistency(kb))
			fprintf(output, "false\n");
		else
			fprintf(output, "true\n");
	}
	else if (!strcmp(reasoning_task, "realisation")) {
		// realize the kb
		realize_kb(kb);
		// print the individual types to the output file
		print_individual_types(kb, output);
	}

	// close the output file
	fclose(output);

	// display saturation information
	if (verbose_flag) {
		printf("\n------ Saturation statistics ------\n");
		printf( "Total subsumptions.................: %d\n"
				"Unique subsumptions................: %d\n",
				saturation_total_subsumption_count,
				saturation_unique_subsumption_count);
		printf( "Total links........................: %d\n"
				"Unique links.......................: %d\n",
				saturation_total_link_count,
				saturation_unique_link_count);
	}

	// free the kb
	int freed_bytes = free_kb(kb);
	printf("Freed bytes:%d\n", freed_bytes);

	return 0;
}

