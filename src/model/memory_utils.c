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
#include <stdlib.h>

#include "datatypes.h"
#include "limits.h"
#include "../utils/set.h"
#include "../utils/list_64.h"
#include "../utils/map.h"



int free_concept(ClassExpressionId id, TBox* tbox) {
	int total_freed_bytes = 0;

	ClassExpression* c = &(tbox->class_expressions[id]);

	switch (c->type) {
	// free the class
	case CLASS_TYPE:
		// free the equivalent concepts list
		total_freed_bytes += SET_RESET(&(c->description.atomic.equivalent_classes));

		// free the direct subsumers set
		total_freed_bytes += SET_RESET(&(c->description.atomic.direct_subsumers));

		total_freed_bytes += sizeof(char) * strlen(c->description.atomic.IRI);
		free(c->description.atomic.IRI);
		break;
	case OBJECT_INTERSECTION_OF_TYPE:
	case OBJECT_SOME_VALUES_FROM_TYPE:
	case OBJECT_ONE_OF_TYPE:
		// nothing to seperately handle for these types
		break;
	default:
		fprintf(stderr,"unknown class expression type, aborting\n");
		exit(-1);
	}

	// free the told subsumers list
	total_freed_bytes += list_reset(&(c->told_subsumers));

	// free the subsumers hash
	total_freed_bytes += SET_RESET(&(c->subsumers));

	// free the predecessors matrix.
	int i;
	for (i = 0; i < c->predecessor_r_count; ++i) {
		total_freed_bytes += SET_RESET(&(c->predecessors[i].fillers));
	}
	free(c->predecessors);
	total_freed_bytes += c->predecessor_r_count  * sizeof(Link);

	// similarly free the successors hash.
	for (i = 0; i < c->successor_r_count; ++i) {
		total_freed_bytes += SET_RESET(&(c->successors[i].fillers));
	}
	free(c->successors);
	total_freed_bytes += c->successor_r_count  * sizeof(Link);

	// free the filler of negative existentials set
	if (c->filler_of_negative_exists != NULL)
		total_freed_bytes += MAP_FREE(c->filler_of_negative_exists);

	// free the list of conjunctions where this concept occurs
	total_freed_bytes += list_reset(&(c->first_conjunct_of_list));
	// free the first_conjunct_of hash, if it exists
	if (c->first_conjunct_of != NULL)
		total_freed_bytes += SET_FREE(c->first_conjunct_of);

	// now for the second conjunct
	total_freed_bytes += list_reset(&(c->second_conjunct_of_list));
	// free the second_conjunct_of hash, if it exists
	if (c->second_conjunct_of != NULL)
		total_freed_bytes += SET_FREE(c->second_conjunct_of);

	return total_freed_bytes;
}

int free_role(ObjectPropertyExpressionId id, TBox* tbox) {
	int total_freed_bytes = 0;

	ObjectPropertyExpression* r = &(tbox->object_property_expressions[id]);

	switch (r->type) {
	case OBJECT_PROPERTY_TYPE:
		total_freed_bytes += sizeof(char) * strlen(r->description.atomic.IRI);
		free(r->description.atomic.IRI);
		break;
	case OBJECT_PROPERTY_CHAIN_TYPE:
		break;
	}

	// free the told subsumers list
	// total_freed_bytes += sizeof(Role*) * r->told_subsumer_count;
	// free(r->told_subsumers);
	total_freed_bytes += SET_RESET(&(r->told_subsumers));

	// free the  subsumers list
	total_freed_bytes += list_reset(&(r->subsumer_list));

	// free the subsumers hash
	total_freed_bytes += SET_RESET(&(r->subsumers));

	// free the subsumees hash
	total_freed_bytes += SET_RESET(&(r->subsumees));

	// free the list of role compositions where this role occurs
	total_freed_bytes += sizeof(ObjectPropertyExpressionId) * r->first_component_of_count;
	free(r->first_component_of_list);

	total_freed_bytes += SET_RESET(&(r->first_component_of));

	// now for the second component
	total_freed_bytes += sizeof(ObjectPropertyExpressionId) * r->second_component_of_count;
	free(r->second_component_of_list);

	total_freed_bytes += SET_RESET(&(r->second_component_of));

	return total_freed_bytes;
}

int free_tbox(TBox* tbox) {
	int i, total_freed_bytes = 0;

	SetIterator_64 set_iterator;
	// free subclass axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->subclass_of_axioms));
	void* ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(SubClassOfAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->subclass_of_axioms));

	// free equivalent class axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->equivalent_classes_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(EquivalentClassesAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->equivalent_classes_axioms));

	// free the disjoint classes axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->disjoint_classes_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		total_freed_bytes += list_reset(&(((DisjointClassesAxiom*) ax)->classes));
		free(ax);
		total_freed_bytes += sizeof(DisjointClassesAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->disjoint_classes_axioms));

	// free role subobjectpropertyof axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->subobjectproperty_of_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(SubObjectPropertyOfAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->subobjectproperty_of_axioms));

	// free equivalent role axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->equivalent_objectproperties_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(EquivalentObjectPropertiesAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->equivalent_objectproperties_axioms));

	// free transitive role axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->transitive_objectproperty_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(TransitiveObjectPropertyAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->transitive_objectproperty_axioms));

	// free the objectproperty domain axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->objectproperty_domain_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(ObjectPropertyDomainAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->objectproperty_domain_axioms));

	// free the same individual axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->same_individual_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		total_freed_bytes += list_reset(&(((SameIndividualAxiom*) ax)->individuals));
		free(ax);
		total_freed_bytes += sizeof(SameIndividualAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->same_individual_axioms));

	// free the different individual axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->different_individuals_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		total_freed_bytes += list_reset(&(((DifferentIndividualsAxiom*) ax)->individuals));
		free(ax);
		total_freed_bytes += sizeof(DifferentIndividualsAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->different_individuals_axioms));

	// free the generated subclass axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->generated_subclass_of_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(SubClassOfAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->generated_subclass_of_axioms));

	// free the generated subrole axioms
	SET_ITERATOR_INIT_64(&set_iterator, &(tbox->generated_subobjectproperty_of_axioms));
	ax = SET_ITERATOR_NEXT_64(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(SubObjectPropertyOfAxiom);
		ax = SET_ITERATOR_NEXT_64(&set_iterator);
	}
	total_freed_bytes += SET_RESET_64(&(tbox->generated_subobjectproperty_of_axioms));


	/************************************************************/

	// iterate over the tbox->class_expressions array, free the class expressions
	ClassExpressionId id;
	for (id = 0; id <= tbox->last_class_expression_id; ++id) {
		total_freed_bytes += free_concept(id, tbox);
	}

	// free the class expressions array itself
	free(tbox->class_expressions);
	total_freed_bytes += (tbox->last_class_expression_id + 1) * sizeof(ClassExpression);

	// free the existentials map
	total_freed_bytes += MAP_RESET(&(tbox->object_some_values_from_exps));

	// free the generated nominals hash
	total_freed_bytes += MAP_RESET(&(tbox->generated_object_some_values_from_exps));

	// free the conjunctions map
	total_freed_bytes += MAP_RESET(&(tbox->object_intersection_of_exps));

	// free the nominals hash
	total_freed_bytes += MAP_RESET(&(tbox->object_one_of_exps));

	// free the generated nominals map
	total_freed_bytes += MAP_RESET(&(tbox->generated_object_one_of_exps));


	// free the atomic concepts hash
	total_freed_bytes += MAP_RESET(&(tbox->classes));

	// iterate over the object property expressions array,
	// free the object property expressions
	for (id = 0; id <= tbox->last_object_property_expression_id; ++id)
		total_freed_bytes += free_role(id, tbox);

	// free the object property expressions array itself
	free(tbox->object_property_expressions);
	total_freed_bytes += (tbox->last_object_property_expression_id + 1) * sizeof(ObjectPropertyExpression);

	// free the role compositions hash
	total_freed_bytes += MAP_RESET(&(tbox->objectproperty_chains));

	// free the atomic roles hash
	total_freed_bytes += MAP_RESET(&(tbox->objectproperties));

	// finally free the tbox itself
	free(tbox);

	return total_freed_bytes;
}

int free_individual(IndividualId ind, ABox* abox) {
	int total_freed_bytes = 0;

	total_freed_bytes += sizeof(char) * strlen(abox->individuals[ind].IRI);
	free(abox->individuals[ind].IRI);

	return total_freed_bytes;
}

int free_abox(ABox* abox) {
	int i, total_freed_bytes = 0;

	// free concept assertions
	for (i = 0; i < abox->concept_assertion_count; ++i)
		free(abox->concept_assertions[i]);
	total_freed_bytes += sizeof(ClassAssertion) * abox->concept_assertion_count;
	free(abox->concept_assertions);
	total_freed_bytes += sizeof(ClassAssertion*) * abox->concept_assertion_count;

	// free role assertions
	for (i = 0; i < abox->role_assertion_count; ++i)
		free(abox->role_assertions[i]);
	total_freed_bytes += sizeof(ObjectPropertyAssertion) * abox->role_assertion_count;
	free(abox->role_assertions);
	total_freed_bytes += sizeof(ObjectPropertyAssertion*) * abox->role_assertion_count;

	// iterate over the individuals array, free the individuals
	ClassExpressionId id;
	for (id = 0; id <= abox->last_individual_id; ++id)
		total_freed_bytes += free_individual(id, abox);

	// free the individuals array
	free(abox->individuals);
	total_freed_bytes += (abox->last_individual_id + 1) * sizeof(Individual);

	// free the individuals hash
	total_freed_bytes += hash_map_free(abox->individuals_map);

	// finally free the abox itself
	free(abox);

	return total_freed_bytes;
}

int free_kb(KB* kb) {
	int total_freed_bytes = 0;

	total_freed_bytes += free_tbox(kb->tbox);
	total_freed_bytes += free_abox(kb->abox);

	// free the prefix names
	int i;
	for (i = 0; i < kb->prefix_names.size; ++i) {
		total_freed_bytes += sizeof(char) * strlen((char*) (kb->prefix_names.elements[i]));
		free((char*) (kb->prefix_names.elements[i]));
	}
	total_freed_bytes += list_reset_64(&kb->prefix_names);

	// free the prefixes
	MapIterator_64 map_iterator_64;
	void* map_element_64;

	MAP_ITERATOR_INIT_64(&map_iterator_64, &(kb->prefixes));
	map_element_64 = MAP_ITERATOR_NEXT_64(&map_iterator_64);
	while (map_element_64) {
		// the value is prefix (char*), free it
		total_freed_bytes += sizeof(char) * strlen((char*) map_element_64);
		free(map_element_64);
		map_element_64 = MAP_ITERATOR_NEXT_64(&map_iterator_64);
	}
	// free the prefixes map
	total_freed_bytes += MAP_RESET_64(&(kb->prefixes));

	return total_freed_bytes;
}
