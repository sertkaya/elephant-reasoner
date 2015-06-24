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
#include <inttypes.h>

#include "../utils/map.h"
#include "../hashing/utils.h"

int main(int argc, char *argv[]) {
	Map map;
	MAP_INIT(&map, 16);
	MAP_PUT(HASH_STRING("<http://sadi-ontology.semanticscience.org#D054872>"), "D054872", &map);

	printf("%s\n", MAP_GET(HASH_STRING("<http://sadi-ontology.semanticscience.org#D054872>"), &map));
	printf("%s\n", MAP_GET(HASH_STRING("<http://sadi-ontology.semanticscience.org#D054809>"), &map));
	printf("%" PRIu32 "\n%" PRIu32 "\n", HASH_STRING("<http://sadi-ontology.semanticscience.org#D054872>"), HASH_STRING("<http://sadi-ontology.semanticscience.org#D054809>"));

	return 0;
}

