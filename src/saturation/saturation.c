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

#include "../model/datatypes.h"
#include "../model/model.h"
#include "role_saturation.h"
#include "concept_saturation.h"


/*
 * Saturates the given TBox.
 * Returns:
 * 	-1: If the reasoning task is consistency check, and an atomic concept has the
 * 	subsumer bottom. In this case it immediately returns, i.e., saturation process
 * 	is cancelled.
 * 	0: Otherwise
 */
char saturate_tbox(KB* kb, ReasoningTask reasoning_task) {
	char saturation_result = 0;

	saturate_roles(kb);
	saturation_result = saturate_concepts(kb, reasoning_task);

	return saturation_result;
}

