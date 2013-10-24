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
#include <stdlib.h>
#include <assert.h>

#include "queue.h"

#define IS_QUEUE_FULL(q) 	((q)->size == (q)->count)
#define IS_QUEUE_EMPTY(q) 	((q)->count == 0)

/*
 * Linked list implementation
*/

void init_queue(Queue* q) {
	q->front = NULL;
	q->rear = NULL;
}

void enqueue(Queue* queue, void* data) {
	QueueElement* new_qe;

	new_qe = (QueueElement*) malloc(sizeof(QueueElement));
	assert(new_qe != NULL);

	new_qe->data = data;
	new_qe->next = NULL;

	if (queue->rear == NULL) {
		queue->front = new_qe;
		queue->rear = new_qe;
	}
	else {
		queue->rear->next = new_qe;
		queue->rear = new_qe;
	}
}

void* dequeue(Queue* queue) {
	void* data;
	QueueElement* tmp;

	if (queue->front == NULL && queue->rear == NULL)
		// the queue is empty
		return NULL;

	data = queue->front->data;
	if (queue->rear == queue->front)
		// we are dequeuing the last element
		queue->rear = NULL;
	tmp = queue->front;
	queue->front = queue->front->next;
	free(tmp);

	return data;
}


/*
 * Circular queue implementation
*/
/*
void init_queue(Queue* queue, int size) {
	queue->front = 0;
	queue->rear = 0;
	queue->size = size;
	queue->count = 0;
	queue->elements = (void**) malloc(queue->size * sizeof(void*));
	assert(queue->elements != NULL);
}

void enqueue(Queue* queue, void* element) {
	// if (IS_QUEUE_FULL(queue)) {
	// 	fprintf(stderr, "Queue full, aborting\n");
	// 	exit(EXIT_FAILURE);
	// }

	queue->elements[queue->rear] = element;
	queue->rear = (queue->rear + 1) % queue->size;
	if (queue->rear == queue->front)
		queue->front = (queue->front + 1) % queue->size;
	++queue->count;
}

void* dequeue(Queue* queue) {
	void *tmp;
	
	if (IS_QUEUE_EMPTY(queue))
		return NULL;
	
	tmp = queue->elements[queue->front];
	queue->front = (queue->front + 1) % queue->size;
	--queue->count;
	
	return tmp;
}
*/
