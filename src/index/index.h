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


#ifndef INDEX_H_
#define INDEX_H_

#include "../model/datatypes.h"

/*
 * Indexes the given TBox.
 * Returns:
 * 	-1: If the reasoning task is consistency check, and an atomic concept has the
 * 	told subsumer bottom. In this case it immediately returns, i.e., the rest of
 * 	the KB is not indexed!
 * 	1: If the reasoning task is consistency check and bottom does not appear on the
 * 	rhs of any axiom. This means the KB is consistent.
 * 	0: Otherwise
 */
// char index_tbox(TBox* tbox, ReasoningTask reasoning_task );

void index_role(ObjectPropertyExpressionId r);

// void index_abox(ABox* abox);

char index_kb(KB* kb, ReasoningTask reasoning_task);

#endif /* INDEX_H_ */
