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
#include <unistd.h>
#include <assert.h>

#include "reasoner/reasoner.h"
#include "model/datatypes.h"
#include "model/memory_utils.h"
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

	// the input kb file
	input_kb = fopen(argv[1], "r");
	assert(input_kb != NULL);

	// the output taxonomy file
	output_taxonomy = fopen(argv[2], "w");
	assert(output_taxonomy != NULL);

	TBox* tbox = NULL;
	// initialize global variables, allocate space
	tbox = init_reasoner();

	// read and parse the kb
	read_kb(input_kb, tbox);
	fclose(input_kb);
	// display kb information
	// print_short_stats(tbox);

	// classify the kb
	classify(tbox);

	// print the concept hierarchy
	print_concept_hierarchy(tbox, output_taxonomy);
	fclose(output_taxonomy);

	// free the kb
	printf("Freed bytes:%d\n", free_tbox(tbox));

	return 0;
}

