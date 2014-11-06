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


#ifndef _HIERARCHY_UTILS_H_
#define _HIERARCHY_UTILS_H_

// Add concept c1 to the list of equivalent concepts of c2
int add_equivalent_concept(ClassExpression* c1, ClassExpression* c2);

// returns 1 if c1 is a subsumer of c2, otherwise 0
int is_subsumer_of(ClassExpression* c1, ClassExpression* c2);

/**
 * Add class expression c1 to the equivalent classes of class expression c2.
 */
#define ADD_EQUIVALENT_CLASS(c1, c2)	SET_ADD(c1, &(c2->description.atomic->equivalent_classes))

/**
 * Add class expression c1 to the direct subsumers of c2
 */
#define ADD_DIRECT_SUBSUMER(c1, c2)		SET_ADD(c1, &(c2->description.atomic->direct_subsumers))

/**
 * Remove class expression c1 from the direct subsumers of class expression c2
 */
#define REMOVE_DIRECT_SUBSUMER(c1, c2)	SET_REMOVE(c1, &(c2->description.atomic->direct_subsumers))


#endif /* _HIERARCHY_UTILS_H_ */
