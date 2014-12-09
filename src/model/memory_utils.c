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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "datatypes.h"
#include "limits.h"
#include "../utils/set.h"
#include "../utils/map.h"
#include "../hashing/hash_map.h"



int free_concept(ClassExpression* c, TBox* tbox) {
	int total_freed_bytes = 0;

	// free the class
	if (c->type == CLASS_TYPE) {
		// free the equivalent concepts list
		total_freed_bytes += SET_RESET(&(c->description.atomic.equivalent_classes));

		// free the direct subsumers set
		total_freed_bytes += SET_RESET(&(c->description.atomic.direct_subsumers));

		total_freed_bytes += sizeof(char) * strlen(c->description.atomic.IRI);
		free(c->description.atomic.IRI);
	}

	// free the told subsumers list
	total_freed_bytes += list_reset(&(c->told_subsumers));

	// free the subsumers hash
	total_freed_bytes += SET_RESET(&(c->subsumers));

	// free the predecessors matrix.
	int i;
	for (i = 0; i < c->predecessor_r_count; ++i) {
		free(c->predecessors[i].fillers);
		total_freed_bytes += c->predecessors[i].filler_count * sizeof(ClassExpression*);
	}
	free(c->predecessors);
	total_freed_bytes += c->predecessor_r_count  * sizeof(Link);

	// similarly free the successors hash.
	for (i = 0; i < c->successor_r_count; ++i) {
		free(c->successors[i].fillers);
		total_freed_bytes += c->successors[i].filler_count * sizeof(ClassExpression*);
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

	// finally free this concept
	total_freed_bytes += sizeof(ClassExpression);
	free(c);

	return total_freed_bytes;
}

int free_role(ObjectPropertyExpression* r) {
	int total_freed_bytes = 0;

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
	total_freed_bytes += sizeof(ObjectPropertyExpression*) * r->first_component_of_count;
	free(r->first_component_of_list);

	total_freed_bytes += SET_RESET(&(r->first_component_of));

	// now for the second component
	total_freed_bytes += sizeof(ObjectPropertyExpression*) * r->second_component_of_count;
	free(r->second_component_of_list);

	total_freed_bytes += SET_RESET(&(r->second_component_of));

	// finally free this role
	total_freed_bytes += sizeof(ObjectPropertyExpression);
	free(r);

	return total_freed_bytes;
}

int free_tbox(TBox* tbox) {
	int i, total_freed_bytes = 0;

	SetIterator set_iterator;
	// free subclass axioms
	SET_ITERATOR_INIT(&set_iterator, &(tbox->subclassof_axioms));
	void* ax = SET_ITERATOR_NEXT(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(SubClassOfAxiom);
		ax = SET_ITERATOR_NEXT(&set_iterator);
	}
	total_freed_bytes += SET_RESET(&(tbox->subclassof_axioms));

	// free equivalent class axioms
	SET_ITERATOR_INIT(&set_iterator, &(tbox->equivalentclasses_axioms));
	ax = SET_ITERATOR_NEXT(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(EquivalentClassesAxiom);
		ax = SET_ITERATOR_NEXT(&set_iterator);
	}
	total_freed_bytes += SET_RESET(&(tbox->equivalentclasses_axioms));

	// free the disjoint classes axioms
	for (i = 0; i < tbox->disjointclasses_axiom_count; ++i) {
		total_freed_bytes += list_reset(&(tbox->disjointclasses_axioms[i]->classes));
		free(tbox->disjointclasses_axioms[i]);
	}
	total_freed_bytes += tbox->disjointclasses_axiom_count * sizeof(DisjointClassesAxiom);
	free(tbox->disjointclasses_axioms);
	total_freed_bytes += tbox->disjointclasses_axiom_count * sizeof(DisjointClassesAxiom*);

	// free role subobjectpropertyof axioms
	SET_ITERATOR_INIT(&set_iterator, &(tbox->subobjectpropertyof_axioms));
	ax = SET_ITERATOR_NEXT(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(SubObjectPropertyOfAxiom);
		ax = SET_ITERATOR_NEXT(&set_iterator);
	}
	total_freed_bytes += SET_RESET(&(tbox->subobjectpropertyof_axioms));

	// free equivalent role axioms
	for  (i = 0; i < tbox->eqrole_axiom_count; i++)
		free(tbox->eqrole_axioms[i]);
	total_freed_bytes += sizeof(EquivalentObjectPropertiesAxiom) * tbox->eqrole_axiom_count;
	free(tbox->eqrole_axioms);
	total_freed_bytes += sizeof(EquivalentObjectPropertiesAxiom*) * tbox->eqrole_axiom_count;

	// free transitive role axioms
	SET_ITERATOR_INIT(&set_iterator, &(tbox->transitiveobjectproperty_axioms));
	ax = SET_ITERATOR_NEXT(&set_iterator);
	while (ax) {
		free(ax);
		total_freed_bytes += sizeof(TransitiveObjectPropertyAxiom);
		ax = SET_ITERATOR_NEXT(&set_iterator);
	}
	total_freed_bytes += SET_RESET(&(tbox->transitiveobjectproperty_axioms));

	// iterate over the existentials hash, free the existentials
	MapIterator iterator;
	MAP_ITERATOR_INIT(&iterator, &(tbox->object_some_values_from_exps));
	void* map_element = MAP_ITERATOR_NEXT(&iterator);
	while (map_element) {
		total_freed_bytes += free_concept((ClassExpression*) map_element, tbox);
		map_element = MAP_ITERATOR_NEXT(&iterator);
	}
	// free the existentials map
	total_freed_bytes += MAP_RESET(&(tbox->object_some_values_from_exps));

	// iterate over the conjunctions map, free the conjunctions
	MAP_ITERATOR_INIT(&iterator, &(tbox->object_intersection_of_exps));
	map_element = MAP_ITERATOR_NEXT(&iterator);
	while (map_element) {
		total_freed_bytes += free_concept((ClassExpression*) map_element, tbox);
		map_element = MAP_ITERATOR_NEXT(&iterator);
	}
	// free the conjunctions map
	total_freed_bytes += MAP_RESET(&(tbox->object_intersection_of_exps));

	// iterate over the nominals map, free the nominals
	MAP_ITERATOR_INIT(&iterator, &(tbox->object_one_of_exps));
	map_element = MAP_ITERATOR_NEXT(&iterator);
	while (map_element) {
		total_freed_bytes += free_concept((ClassExpression*) map_element, tbox);
		map_element = MAP_ITERATOR_NEXT(&iterator);
	}
	// free the nominals hash
	total_freed_bytes += MAP_RESET(&(tbox->object_one_of_exps));

	// iterate over the atomic concepts map, free the atomic concepts
	MAP_ITERATOR_INIT(&iterator, &(tbox->classes));
	map_element = MAP_ITERATOR_NEXT(&iterator);
	while (map_element) {
		total_freed_bytes += free_concept((ClassExpression*) map_element, tbox);
		map_element = MAP_ITERATOR_NEXT(&iterator);
	}
	// free the atomic concepts hash
	total_freed_bytes += MAP_RESET(&(tbox->classes));

	// iterate over the role_compositions hash, free the role compositions
	MAP_ITERATOR_INIT(&iterator, &(tbox->object_property_chains));
	map_element = MAP_ITERATOR_NEXT(&iterator);
	while (map_element) {
		total_freed_bytes += free_role((ObjectPropertyExpression*) map_element);
		map_element = MAP_ITERATOR_NEXT(&iterator);
	}
	// free the role compositions hash
	total_freed_bytes += MAP_RESET(&(tbox->object_property_chains));

	// iterate over atomic roles, free them
	MAP_ITERATOR_INIT(&iterator, &(tbox->object_properties));
	map_element = MAP_ITERATOR_NEXT(&iterator);
	while (map_element) {
		total_freed_bytes += free_role((ObjectPropertyExpression*) map_element);
		map_element = MAP_ITERATOR_NEXT(&iterator);
	}
	// free the atomic roles hash
	total_freed_bytes += MAP_RESET(&(tbox->object_properties));

	// finally free the tbox itself
	free(tbox);

	return total_freed_bytes;
}

int free_individual(Individual* ind) {
	int total_freed_bytes = 0;

	total_freed_bytes += sizeof(char) * strlen(ind->IRI);
	free(ind->IRI);

	// finally free this individual
	total_freed_bytes += sizeof(Individual);
	free(ind);

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

	// iterate over the individuals hash, free the individuals
	HashMapElement* node = HASH_MAP_LAST_ELEMENT(abox->individuals);
	while (node) {
		total_freed_bytes += free_individual((Individual*) node->value);
		node = HASH_MAP_PREVIOUS_ELEMENT(node);
	}
	// free the individuals hash
	total_freed_bytes += hash_map_free(abox->individuals);

	// finally free the abox itself
	free(abox);

	return total_freed_bytes;
}

int free_kb(KB* kb) {
	int total_freed_bytes = 0;

	total_freed_bytes += free_tbox(kb->tbox);
	total_freed_bytes += free_abox(kb->abox);

	/*
	// iterate over the prefixes hash, free the prefixes
	Node* node = last_node(kb->prefixes);
	while (node) {
		total_freed_bytes += strlen((char*) node->value);
		free((char*) node->value);
		node = previous_node(node);
	}
	// free the prefixes hash
	total_freed_bytes += free_key_value_hash_table(kb->prefixes);
	*/

	// iterate through the prefixes and prefix names list,
	// free each of them
	int i;
	for (i = 0; i < kb->prefix_count; ++i) {
		total_freed_bytes += strlen(kb->prefix_names_list[i]);
		free(kb->prefix_names_list[i]);
		total_freed_bytes += strlen(kb->prefix_list[i]);
		free(kb->prefix_list[i]);
	}
	// free the list of prefix names
	free(kb->prefix_names_list);
	total_freed_bytes += kb->prefix_count * sizeof(char*);

	// free the list of prefixes
	free(kb->prefix_list);
	total_freed_bytes += kb->prefix_count * sizeof(char*);

	// free the generated subclass axioms
	for (i = 0; i < kb->generated_subclass_axiom_count; ++i)
		free(kb->generated_subclass_axioms[i]);
	total_freed_bytes += sizeof(SubClassOfAxiom) * kb->generated_subclass_axiom_count;
	free(kb->generated_subclass_axioms);
	total_freed_bytes += sizeof(SubClassOfAxiom*) * kb->generated_subclass_axiom_count;

	// free the generated subrole axioms
	for (i = 0; i < kb->generated_subrole_axiom_count; ++i)
		free(kb->generated_subrole_axioms[i]);
	total_freed_bytes += sizeof(SubObjectPropertyOfAxiom) * kb->generated_subrole_axiom_count;
	free(kb->generated_subrole_axioms);
	total_freed_bytes += sizeof(SubObjectPropertyOfAxiom*) * kb->generated_subrole_axiom_count;

	// iterate over the generated nominals hash, free the nominals
	HashMapElement* node = HASH_MAP_LAST_ELEMENT(kb->generated_nominals);
	while (node) {
		total_freed_bytes += free_concept((ClassExpression*) node->value, kb->tbox);
		node = HASH_MAP_PREVIOUS_ELEMENT(node);
	}
	// free the generated nominals hash
	total_freed_bytes += hash_map_free(kb->generated_nominals);

	// iterate over the generated existential restrictions hash, free them
	node = HASH_MAP_LAST_ELEMENT(kb->generated_exists_restrictions);
	while (node) {
		total_freed_bytes += free_concept((ClassExpression*) node->value, kb->tbox);
		node = HASH_MAP_PREVIOUS_ELEMENT(node);
	}
	// free the generated nominals hash
	total_freed_bytes += hash_map_free(kb->generated_exists_restrictions);

	return total_freed_bytes;
}
