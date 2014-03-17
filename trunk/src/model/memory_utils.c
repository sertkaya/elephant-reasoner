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


#include <string.h>
#include <stdlib.h>

#include "datatypes.h"
#include "limits.h"



int free_concept(Concept* c, TBox* tbox) {
	int total_freed_bytes = 0;
	Word_t freed_bytes;

	// free the 3 different description types
	switch (c->type) {
	case ATOMIC_CONCEPT:
		// free the equivalent concepts list
		total_freed_bytes += sizeof(Concept*) * c->description.atomic->equivalent_concepts_count;
		free(c->description.atomic->equivalent_concepts_list);

		// free the direct subsumers hash
		freed_bytes = free_key_hash_table(c->description.atomic->direct_subsumers);
		total_freed_bytes += freed_bytes;

		// now the direct subsumers list
		total_freed_bytes += sizeof(Concept*) * c->description.atomic->direct_subsumer_count;
		free(c->description.atomic->direct_subsumer_list);

		total_freed_bytes += sizeof(char) * strlen(c->description.atomic->name);
		free(c->description.atomic->name);

		total_freed_bytes += sizeof(AtomicConcept);
		free(c->description.atomic);
		break;
	case CONJUNCTION:
		total_freed_bytes += sizeof(Conjunction);
		free(c->description.conj);
		break;
	case EXISTENTIAL_RESTRICTION:
		total_freed_bytes += sizeof(Exists);
		free(c->description.exists);
		break;
	default:
		fprintf(stderr,"unknown concept type, aborting\n");
		exit(-1);
	}

	// free the told subsumers hash
	// J1FA(freed_bytes, c->told_subsumers);
	// total_freed_bytes += freed_bytes;
	total_freed_bytes += sizeof(Concept*) * c->told_subsumer_count;
	free(c->told_subsumers);

	// free the subsumers list
	total_freed_bytes += sizeof(Concept*) * c->subsumer_count;
	free(c->subsumer_list);

	// free the subsumers hash
	// J1FA(freed_bytes, c->subsumers);
	// total_freed_bytes += freed_bytes;

	// free the predecessors hash. note that this is
	// 2 dimensional
	PWord_t key;
	Pvoid_t bitmap;
	Word_t index = 0;
	/*
	JLF(key, c->predecessors, index);
	while (key != NULL) {
		bitmap = (Pvoid_t) *key;
		J1FA(freed_bytes, bitmap);
		total_freed_bytes += freed_bytes;
		JLN(key, c->predecessors, index);
	}
	JLFA(freed_bytes, c->predecessors);
	total_freed_bytes += freed_bytes;
	*/
	int i;
	for (i = 0; i < c->predecessor_r_count; ++i) {
		free(c->predecessors[i]->fillers);
		total_freed_bytes += c->predecessors[i]->filler_count * sizeof(Concept*);
		free(c->predecessors[i]);
		total_freed_bytes += sizeof(Link);
	}
	free(c->predecessors);
	total_freed_bytes += c->predecessor_r_count  * sizeof(Link*);

	// similarly free the successors hash.
	for (i = 0; i < c->successor_r_count; ++i) {
		free(c->successors[i]->fillers);
		total_freed_bytes += c->successors[i]->filler_count * sizeof(Concept*);
		free(c->successors[i]);
		total_freed_bytes += sizeof(Link);
	}
	free(c->successors);
	total_freed_bytes += c->successor_r_count  * sizeof(Link*);
	/*
	key = NULL;
	index = 0;
	bitmap = (Pvoid_t) NULL;
	JLF(key, c->successors, index);
	while (key != NULL) {
		bitmap = (Pvoid_t) *key;
		J1FA(freed_bytes, bitmap);
		total_freed_bytes += freed_bytes;
		JLN(key, c->successors, index);
	}
	JLFA(freed_bytes, c->successors);
	total_freed_bytes += freed_bytes;
	*/

	// free the filler of negative existentials hash
	// JLFA(freed_bytes, c->filler_of_negative_exists);
	// total_freed_bytes += freed_bytes;
	free(c->filler_of_negative_exists);
	total_freed_bytes += (tbox->atomic_role_count + tbox->unique_binary_role_composition_count) * sizeof(Concept*);

	// free the list of conjunctions where this concept occurs
	free(c->first_conjunct_of_list);
	total_freed_bytes += sizeof(Concept*) * c->first_conjunct_of_count;
	// free it if the first_conjunct_of hash exists
	if (c->first_conjunct_of != NULL) {
		freed_bytes = free_key_hash_table(c->first_conjunct_of);
		total_freed_bytes += freed_bytes;
	}

	// now for the second conjunct
	free(c->second_conjunct_of_list);
	total_freed_bytes += sizeof(Concept*) * c->second_conjunct_of_count;
	// free it if the second_conjunct_of hash exists
	if (c->second_conjunct_of != NULL) {
		freed_bytes = free_key_hash_table(c->second_conjunct_of);
		total_freed_bytes += freed_bytes;
	}

	// finally free this concept
	total_freed_bytes += sizeof(Concept);
	free(c);

	return total_freed_bytes;
}

int free_role(Role* r) {
	int total_freed_bytes = 0;
	Word_t freed_bytes;

	switch (r->type) {
	case ATOMIC_ROLE:
		total_freed_bytes += sizeof(char) * strlen(r->description.atomic->name);
		free(r->description.atomic->name);
		total_freed_bytes += sizeof(AtomicRole);
		free(r->description.atomic);
		break;
	case ROLE_COMPOSITION:
		// total_freed_bytes += sizeof(Role*) * r->description.role_composition->size;
		// free(r->description.role_composition->roles);
		total_freed_bytes += sizeof(RoleComposition);
		free(r->description.role_composition);
		break;
	}

	// free the told subsumers list
	total_freed_bytes += sizeof(Role*) * r->told_subsumer_count;
	free(r->told_subsumers);

	// free the  subsumers list
	total_freed_bytes += sizeof(Role*) * r->subsumer_count;
	free(r->subsumer_list);

	// free the subsumers hash
	freed_bytes = free_key_hash_table(r->subsumers);
	total_freed_bytes += freed_bytes;

	// free the  subsumees list
	total_freed_bytes += sizeof(Role*) * r->subsumee_count;
	free(r->subsumee_list);

	// free the subsumees hash
	freed_bytes = free_key_hash_table(r->subsumees);
	total_freed_bytes += freed_bytes;

	// free the list of role compositions where this role occurs
	total_freed_bytes += sizeof(Role*) * r->first_component_of_count;
	free(r->first_component_of_list);

	freed_bytes = free_key_hash_table(r->first_component_of);
	total_freed_bytes += freed_bytes;

	// now for the second component
	total_freed_bytes += sizeof(Role*) * r->second_component_of_count;
	free(r->second_component_of_list);

	freed_bytes = free_key_hash_table(r->second_component_of);
	total_freed_bytes += freed_bytes;

	// finally free this role
	total_freed_bytes += sizeof(Role);
	free(r);

	return total_freed_bytes;
}

int free_tbox(TBox* tbox) {
	int i, freed_bytes, total_freed_bytes = 0;

	// free subclass axioms
	total_freed_bytes += sizeof(SubClassAxiom) * tbox->subclass_axiom_count;
	for (i = 0; i < tbox->subclass_axiom_count; i++)
		free(tbox->subclass_axioms[i]);
	total_freed_bytes += sizeof(SubClassAxiom*) * tbox->subclass_axiom_count;
	free(tbox->subclass_axioms);

	// free equivalent class axioms
	total_freed_bytes += sizeof(EqClassAxiom) * tbox->eqclass_axiom_count;
	for  (i = 0; i < tbox->eqclass_axiom_count; i++)
		free(tbox->eqclass_axioms[i]);
	total_freed_bytes += sizeof(EqClassAxiom*) * tbox->eqclass_axiom_count;
	free(tbox->eqclass_axioms);

	// free role subrole axioms
	total_freed_bytes += sizeof(SubRoleAxiom) * tbox->subrole_axiom_count;
	for (i = 0; i < tbox->subrole_axiom_count; i++)
		free(tbox->subrole_axioms[i]);
	total_freed_bytes += sizeof(SubRoleAxiom*) * tbox->subrole_axiom_count;
	free(tbox->subrole_axioms);

	// free equivalent role axioms
	total_freed_bytes += sizeof(EqRoleAxiom) * tbox->eqrole_axiom_count;
	for  (i = 0; i < tbox->eqrole_axiom_count; i++)
		free(tbox->eqrole_axioms[i]);
	total_freed_bytes += sizeof(EqRoleAxiom*) * tbox->eqrole_axiom_count;
	free(tbox->eqrole_axioms);

	// free transitive role axioms
	total_freed_bytes += sizeof(TransitiveRoleAxiom) * tbox->transitive_role_axiom_count;
	for  (i = 0; i < tbox->transitive_role_axiom_count; i++)
		free(tbox->transitive_role_axioms[i]);
	total_freed_bytes += sizeof(TransitiveRoleAxiom*) * tbox->transitive_role_axiom_count;
	free(tbox->transitive_role_axioms);

	PWord_t key = NULL;
	uint8_t index[MAX_CONCEPT_NAME_LENGTH];
	// free the existentials
	strcpy((char*) index, "");
	JSLF(key, tbox->exists_restrictions, index);
	while (key != NULL) {
		total_freed_bytes += free_concept((Concept*) *key, tbox);
		JSLN(key, tbox->exists_restrictions, index);
	}
	JSLFA(freed_bytes, tbox->exists_restrictions);
	total_freed_bytes += freed_bytes;

	// free the conjunctions
	strcpy((char*) index, "");
	JSLF(key, tbox->conjunctions, index);
	while (key != NULL) {
		total_freed_bytes += free_concept((Concept*) *key, tbox);
		JSLN(key, tbox->conjunctions, index);
	}
	JSLFA(freed_bytes, tbox->conjunctions);
	total_freed_bytes += freed_bytes;

	// free the atomic concepts hash
	// note that here we just free place reserved for
	// the hash. the place for reserved for the atomic concepts themselves
	// is freed below
	total_freed_bytes += free_key_value_hash_table(tbox->atomic_concepts);

	// free the atomic concepts list
	// this is the list kept for efficiently traversing over atomic concepts
	// in hierarchy computation. the actual place for keeping atomic concepts
	// is the atomic_concepts hash.
	// here we also free the space allocated for the atomic concepts, not only the list
	for (i = 0; i < tbox->atomic_concept_count; ++i)
		total_freed_bytes += free_concept(tbox->atomic_concept_list[i], tbox);
	total_freed_bytes += sizeof(Concept*) * tbox->atomic_concept_count;
	free(tbox->atomic_concept_list);

	// free the atomic roles list
	total_freed_bytes += sizeof(Role*) * (tbox->atomic_role_count + tbox->unique_binary_role_composition_count);
	free(tbox->role_list);

	// free the atomic roles hash
	strcpy((char*) index, "");
	JSLF(key, tbox->atomic_roles, index);
	while (key != NULL) {
		total_freed_bytes += free_role((Role*) *key);
		JSLN(key, tbox->atomic_roles, index);
	}
	JSLFA(freed_bytes, tbox->atomic_roles);
	total_freed_bytes += freed_bytes;

	// free the role compositions
	strcpy((char*) index, "");
	JSLF(key, tbox->role_compositions, index);
	while (key != NULL) {
		total_freed_bytes += free_role((Role*) *key);
		JSLN(key, tbox->role_compositions, index);
	}
	JSLFA(freed_bytes, tbox->role_compositions);
	total_freed_bytes += freed_bytes;

	return total_freed_bytes;
}
