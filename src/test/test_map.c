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

#include "../hashing/utils.h"
#include "../utils/map.h"

int main(int argc, char *argv[]) {
	Map map;
	MAP_INIT(&map, 16);

	char* prefix_name = "";
	char* prefix = "<http://www.w3.org/2002/07/owl#>";
	MAP_PUT(HASH_STRING(prefix_name), prefix, &map);

	char str[32] = ":Thing";
	char delim[2] = ":";
	char* token = strtok(str, delim);
	printf("token: %s\n", token);
	printf("token value: %s\n", (char*) MAP_GET(HASH_STRING(token), &map));

	return 0;
}

