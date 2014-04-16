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


#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "tbox/datatypes.h"
#include "abox/datatypes.h"


typedef struct knowledge_base KB;
typedef enum reasoning_task ReasoningTask;

// reasoning tasks
enum reasoning_task {
	CLASSIFICATION, CONSISTENCY, REALISATION
};

// knowledge base
struct knowledge_base {
	TBox* tbox;
	ABox* abox;
	// flag for inconsistency
	char inconsistent;
};

#endif /* DATATYPES_H_ */
