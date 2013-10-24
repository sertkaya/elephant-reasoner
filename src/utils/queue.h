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


#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct queue_element QueueElement;
typedef struct queue Queue;


// #define DEQUEUE(qe,q) 		qe = (q)->data[(q)->front++]
// #define IS_EMPTY_QUEUE(q) 	((q)->front == (q)->rear)
// #define ENQUEUE(qe,q) 		(q)->data[(q)->rear++] = qe


struct queue_element {
	void* data;
	QueueElement* next;
};

struct queue {
	QueueElement* front;
	QueueElement* rear;
};

/*
// circular queue
struct queue {
	int size;		// max number of elements
	int count;		// number of elements
	int front;
	int rear;
	void** elements;
};
*/
void init_queue(Queue* q);
void enqueue(Queue* queue, void* data);
void* dequeue(Queue* queue);






#endif /* QUEUE_H_ */
