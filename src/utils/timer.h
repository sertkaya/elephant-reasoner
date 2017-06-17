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

#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>

#define START_TIMER(start_time)				gettimeofday(&start_time, NULL)
#define STOP_TIMER(stop_time)				gettimeofday(&stop_time, NULL)
#define TIME_DIFF(start_time, stop_time)	(((double)stop_time.tv_sec * 1000000.0 + \
		(double)stop_time.tv_usec) - \
		((double)start_time.tv_sec * 1000000.0 + (double)start_time.tv_usec) )



#endif /* TIMER_H_ */
