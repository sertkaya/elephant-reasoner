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
#include "../hashing/hash_map.h"



int free_concept(ClassExpression* c, TBox* tbox) {
	int total_freed_bytes = 0;

	// free the 3 different description types
	switch (c->type) {
	case CLASS_TYPE:
		// free the equivalent concepts list
		total_freed_bytes += list_free(c->description.atomic->equivalent_classes);

		// free the direct subsumers set
		total_freed_bytes += set_free(c->description.atomic->direct_subsumers);

		total_freed_bytes += sizeof(char) * strlen(c->description.atomic->IRI);
		free(c->description.atomic->IRI);

		total_freed_bytes += sizeof(Class);
		free(c->description.atomic);
		break;
	case OBJECT_INTERSECTION_OF_TYPE:
		total_freed_bytes += sizeof(ObjectIntersectionOf);
		free(c->description.conj);
		break;
	case OBJECT_SOME_VALUES_FROM_TYPE:
		total_freed_bytes += sizeof(ObjectSomeValuesFrom);
		free(c->description.exists);
		break;
	case OBJECT_ONE_OF_TYPE:
		total_freed_bytes += sizeof(ObjectOneOf);
		free(c->description.nominal);
		break;
	default:
		fprintf(stderr,"unknown concept type, aborting\n");
		exit(-1);
	}

	// free the told subsumers list
	total_freed_bytes += list_free(c->told_subsumers);

	// free the subsumers hash
	total_freed_bytes += SET_FREE(c->subsumers);

	// free the predecessors matrix.
	int i;
	for (i = 0; i < c->predecessor_r_count; ++i) {
		free(c->predecessors[i]->fillers);
		total_freed_bytes += c->predecessors[i]->filler_count * sizeof(ClassExpression*);
		free(c->predecessors[i]);
		total_freed_bytes += sizeof(Link);
	}
	free(c->predecessors);
	total_freed_bytes += c->predecessor_r_count  * sizeof(Link*);

	// similarly free the successors hash.
	for (i = 0; i < c->successor_r_count; ++i) {
		free(c->successors[i]->fillers);
		total_freed_bytes += c->successors[i]->filler_count * sizeof(ClassExpression*);
		free(c->successors[i]);
		total_freed_bytes += sizeof(Link);
	}
	free(c->successors);
	total_freed_bytes += c->successor_r_count  * sizeof(Link*);

	// free the filler of negative existentials hash
	free(c->filler_of_negative_exists);
	total_freed_bytes += (tbox->atomic_role_count + tbox->unique_binary_role_composition_count) * sizeof(ClassExpression*);

	// free the list of conjunctions where this concept occurs
	free(c->first_conjunct_of_list);
	total_freed_bytes += sizeof(ClassExpression*) * c->first_conjunct_of_count;
	// free it if the first_conjunct_of hash exists
	if (c->first_conjunct_of != NULL)
		total_freed_bytes += free_key_hash_table(c->first_conjunct_of);

	// now for the second conjunct
	free(c->second_conjunct_of_list);
	total_freed_bytes += sizeof(ClassExpression*) * c->second_conjunct_of_count;
	// free it if the second_conjunct_of hash exists
	if (c->second_conjunct_of != NULL)
		total_freed_bytes += free_key_hash_table(c->second_conjunct_of);

	// finally free this concept
	total_freed_bytes += sizeof(ClassExpression);
	free(c);

	return total_freed_bytes;
}

int free_role(Role* r) {
	int total_freed_bytes = 0;

	switch (r->type) {
	case ATOMIC_ROLE:
		total_freed_bytes += sizeof(char) * strlen(r->description.atomic->name);
		free(r->description.atomic->name);
		total_freed_bytes += sizeof(AtomicRole);
		free(r->description.atomic);
		break;
	case ROLE_COMPOSITION:
		total_freed_bytes += sizeof(RoleComposition);
		free(r->description.role_composition);
		break;
	}

	// free the told subsumers list
	// total_freed_bytes += sizeof(Role*) * r->told_subsumer_count;
	// free(r->told_subsumers);
	total_freed_bytes += hash_map_free(r->told_subsumers);

	total_freed_bytes += hash_map_free(r->told_subsumees);

	// free the  subsumers list
	total_freed_bytes += sizeof(Role*) * r->subsumer_count;
	free(r->subsumer_list);

	// free the subsumers hash
	total_freed_bytes += free_key_hash_table(r->subsumers);

	// free the  subsumees list
	total_freed_bytes += sizeof(Role*) * r->subsumee_count;
	free(r->subsumee_list);

	// free the subsumees hash
	total_freed_bytes += free_key_hash_table(r->subsumees);

	// free the list of role compositions where this role occurs
	total_freed_bytes += sizeof(Role*) * r->first_component_of_count;
	free(r->first_component_of_list);

	total_freed_bytes += free_key_hash_table(r->first_component_of);

	// now for the second component
	total_freed_bytes += sizeof(Role*) * r->second_component_of_count;
	free(r->second_component_of_list);

	total_freed_bytes += free_key_hash_table(r->second_component_of);

	// finally free this role
	total_freed_bytes += sizeof(Role);
	free(r);

	return total_freed_bytes;
}

int free_tbox(TBox* tbox) {
	int i, total_freed_bytes = 0;

	// free subclass axioms
	for (i = 0; i < tbox->subclass_axiom_count; i++)
		free(tbox->subclass_axioms[i]);
	total_freed_bytes += sizeof(SubClassAxiom) * tbox->subclass_axiom_count;
	free(tbox->subclass_axioms);
	total_freed_bytes += sizeof(SubClassAxiom*) * tbox->subclass_axiom_count;

	// free equivalent class axioms
	for  (i = 0; i < tbox->eqclass_axiom_count; i++)
		free(tbox->eqclass_axioms[i]);
	total_freed_bytes += sizeof(EqClassAxiom) * tbox->eqclass_axiom_count;
	free(tbox->eqclass_axioms);
	total_freed_bytes += sizeof(EqClassAxiom*) * tbox->eqclass_axiom_count;

	// free the disjoint classes axioms
	for (i = 0; i < tbox->disjointclasses_axiom_count; ++i) {
		free(tbox->disjointclasses_axioms[i]->concepts);
		total_freed_bytes += tbox->disjointclasses_axioms[i]->concept_count * sizeof(ClassExpression*);
		free(tbox->disjointclasses_axioms[i]);
	}
	total_freed_bytes += tbox->disjointclasses_axiom_count * sizeof(DisjointClassesAxiom);
	free(tbox->disjointclasses_axioms);
	total_freed_bytes += tbox->disjointclasses_axiom_count * sizeof(DisjointClassesAxiom*);

	// free role subrole axioms
	for (i = 0; i < tbox->subrole_axiom_count; i++)
		free(tbox->subrole_axioms[i]);
	total_freed_bytes += sizeof(SubRoleAxiom) * tbox->subrole_axiom_count;
	free(tbox->subrole_axioms);
	total_freed_bytes += sizeof(SubRoleAxiom*) * tbox->subrole_axiom_count;

	// free equivalent role axioms
	for  (i = 0; i < tbox->eqrole_axiom_count; i++)
		free(tbox->eqrole_axioms[i]);
	total_freed_bytes += sizeof(EqRoleAxiom) * tbox->eqrole_axiom_count;
	free(tbox->eqrole_axioms);
	total_freed_bytes += sizeof(EqRoleAxiom*) * tbox->eqrole_axiom_count;

	// free transitive role axioms
	for  (i = 0; i < tbox->transitive_role_axiom_count; i++)
		free(tbox->transitive_role_axioms[i]);
	total_freed_bytes += sizeof(TransitiveRoleAxiom) * tbox->transitive_role_axiom_count;
	free(tbox->transitive_role_axioms);
	total_freed_bytes += sizeof(TransitiveRoleAxiom*) * tbox->transitive_role_axiom_count;

	// iterate over the existentials hash, free the existentials
	HashMapElement* node = HASH_MAP_LAST_ELEMENT(tbox->exists_restrictions);
	while (node) {
		total_freed_bytes += free_concept((ClassExpression*) node->value, tbox);
		node = HASH_MAP_PREVIOUS_ELEMENT(tbox->exists_restrictions, node);
	}
	// free the existentials hash
	total_freed_bytes += hash_map_free(tbox->exists_restrictions);

	// iterate over the conjunctions hash, free the conjunctions
	node = HASH_MAP_LAST_ELEMENT(tbox->conjunctions);
	while (node) {
		total_freed_bytes += free_concept((ClassExpression*) node->value, tbox);
		node = HASH_MAP_PREVIOUS_ELEMENT(tbox->conjunctions, node);
	}
	// free the conjunctions hash
	total_freed_bytes += hash_map_free(tbox->conjunctions);

	// iterate over the nominals hash, free the nominals
	node = HASH_MAP_LAST_ELEMENT(tbox->nominals);
	while (node) {
		total_freed_bytes += free_concept((ClassExpression*) node->value, tbox);
		node = HASH_MAP_PREVIOUS_ELEMENT(tbox->nominals, node);
	}
	// free the nominals hash
	total_freed_bytes += hash_map_free(tbox->nominals);

	// iterate over the atomic concepts hash, free the atomic concepts
	node = HASH_MAP_LAST_ELEMENT(tbox->atomic_concepts);
	while (node) {
		total_freed_bytes += free_concept((ClassExpression*) node->value, tbox);
		node = HASH_MAP_PREVIOUS_ELEMENT(tbox->atomic_concepts, node);
	}
	// free the atomic concepts hash
	total_freed_bytes += hash_map_free(tbox->atomic_concepts);

	// free the atomic concepts list
	// this is the list kept for efficiently traversing over atomic concepts
	// in hierarchy computation. the actual place for keeping atomic concepts
	// is the atomic_concepts hash.
	free(tbox->atomic_concept_list);
	total_freed_bytes += sizeof(ClassExpression*) * tbox->atomic_concept_count;

	// iterate over the role_compositions hash, free the role compositions
	node = HASH_MAP_LAST_ELEMENT(tbox->role_compositions);
	while (node) {
		total_freed_bytes += free_role((Role*) node->value);
		node = HASH_MAP_PREVIOUS_ELEMENT(tbox->role_compositions, node);
	}
	// free the role compositions hash
	total_freed_bytes += hash_map_free(tbox->role_compositions);

	// iterate over atomic roles, free them
	node = HASH_MAP_LAST_ELEMENT(tbox->atomic_roles);
	while (node) {
		total_freed_bytes += free_role((Role*) node->value);
		node = HASH_MAP_PREVIOUS_ELEMENT(tbox->atomic_roles, node);
	}
	// free the atomic roles hash
	total_freed_bytes += hash_map_free(tbox->atomic_roles);

	// finally free the tbox itself
	free(tbox);

	return total_freed_bytes;
}

int free_individual(Individual* ind) {
	int total_freed_bytes = 0;

	total_freed_bytes += sizeof(char) * strlen(ind->name);
	free(ind->name);

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
	total_freed_bytes += sizeof(ConceptAssertion) * abox->concept_assertion_count;
	free(abox->concept_assertions);
	total_freed_bytes += sizeof(ConceptAssertion*) * abox->concept_assertion_count;

	// free role assertions
	for (i = 0; i < abox->role_assertion_count; ++i)
		free(abox->role_assertions[i]);
	total_freed_bytes += sizeof(RoleAssertion) * abox->role_assertion_count;
	free(abox->role_assertions);
	total_freed_bytes += sizeof(RoleAssertion*) * abox->role_assertion_count;

	// iterate over the individuals hash, free the individuals
	HashMapElement* node = HASH_MAP_LAST_ELEMENT(abox->individuals);
	while (node) {
		total_freed_bytes += free_individual((Individual*) node->value);
		node = HASH_MAP_PREVIOUS_ELEMENT(abox->individuals, node);
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
	total_freed_bytes += sizeof(SubClassAxiom) * kb->generated_subclass_axiom_count;
	free(kb->generated_subclass_axioms);
	total_freed_bytes += sizeof(SubClassAxiom*) * kb->generated_subclass_axiom_count;

	// free the generated subrole axioms
	for (i = 0; i < kb->generated_subrole_axiom_count; ++i)
		free(kb->generated_subrole_axioms[i]);
	total_freed_bytes += sizeof(SubRoleAxiom) * kb->generated_subrole_axiom_count;
	free(kb->generated_subrole_axioms);
	total_freed_bytes += sizeof(SubRoleAxiom*) * kb->generated_subrole_axiom_count;

	// iterate over the generated nominals hash, free the nominals
	HashMapElement* node = HASH_MAP_LAST_ELEMENT(kb->generated_nominals);
	while (node) {
		total_freed_bytes += free_concept((ClassExpression*) node->value, kb->tbox);
		node = HASH_MAP_PREVIOUS_ELEMENT(kb->generated_nominals, node);
	}
	// free the generated nominals hash
	total_freed_bytes += hash_map_free(kb->generated_nominals);

	// iterate over the generated existential restrictions hash, free them
	node = HASH_MAP_LAST_ELEMENT(kb->generated_exists_restrictions);
	while (node) {
		total_freed_bytes += free_concept((ClassExpression*) node->value, kb->tbox);
		node = HASH_MAP_PREVIOUS_ELEMENT(kb->generated_exists_restrictions, node);
	}
	// free the generated nominals hash
	total_freed_bytes += hash_map_free(kb->generated_exists_restrictions);

	return total_freed_bytes;
}
