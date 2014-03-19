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


#ifndef REASONER_H_
#define REASONER_H_

#include <sys/time.h>

#include "../model/datatypes.h"

struct timeval tStart, tEnd;
#define START_TIMER	gettimeofday(&tStart, NULL)
#define STOP_TIMER	gettimeofday(&tEnd, NULL)
#define TIME_DIFF	(((double)tEnd.tv_sec * 1000000.0 + \
		(double)tEnd.tv_usec) - \
		((double)tStart.tv_sec * 1000000.0 + (double)tStart.tv_usec) )

// the TBox
// TBox* tbox;

// initializes the reasoner and the necessary fields of the TBox
TBox* init_reasoner();

void read_kb(FILE* input_kb, TBox* tbox);

void classify(TBox* tbox);


#endif /* REASONER_H_ */