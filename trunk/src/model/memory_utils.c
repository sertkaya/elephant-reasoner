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



int free_concept(Concept* c) {
	int total_freed_bytes = 0;
	Word_t freed_bytes;

	// free the 3 different description types
	switch (c->type) {
	case ATOMIC_CONCEPT:
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
	J1FA(freed_bytes, c->subsumers);
	total_freed_bytes += freed_bytes;

	// free the equivalent concepts hash
	J1FA(freed_bytes, c->equivalent_concepts);
	total_freed_bytes += freed_bytes;

	// free the direct subsumers hash
	J1FA(freed_bytes, c->direct_subsumers);
	total_freed_bytes += freed_bytes;

	// free the predecessors hash. note that this is
	// 2 dimensional
	PWord_t key;
	Word_t index = 0;
	Pvoid_t bitmap;
	JLF(key, c->predecessors, index);
	while (key != NULL) {
		bitmap = (Pvoid_t) *key;
		J1FA(freed_bytes, bitmap);
		total_freed_bytes += freed_bytes;
		JLN(key, c->predecessors, index);
	}
	JLFA(freed_bytes, c->predecessors);
	total_freed_bytes += freed_bytes;

	// similarly free the successors hash.
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

	// free the filler of negative existentials hash
	JLFA(freed_bytes, c->filler_of_negative_exists);
	total_freed_bytes += freed_bytes;

	// free the list of conjunctions where this concept occurs
	total_freed_bytes += sizeof(Concept*) * c->first_conjunct_of_count;
	free(c->first_conjunct_of_list);
	J1FA(freed_bytes, c->first_conjunct_of);
	total_freed_bytes += freed_bytes;

	// now for the second conjunct
	total_freed_bytes += sizeof(Concept*) * c->second_conjunct_of_count;
	free(c->second_conjunct_of_list);
	J1FA(freed_bytes, c->second_conjunct_of);
	total_freed_bytes += freed_bytes;

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
	J1FA(freed_bytes, r->subsumers);
	total_freed_bytes += freed_bytes;

	// free the subsumees hash
	J1FA(freed_bytes, r->subsumees);
	total_freed_bytes += freed_bytes;

	// free the list of role compositions where this role occurs
	total_freed_bytes += sizeof(Role*) * r->first_component_of_count;
	free(r->first_component_of_list);
	J1FA(freed_bytes, r->first_component_of);
	total_freed_bytes += freed_bytes;

	// now for the second component
	total_freed_bytes += sizeof(Role*) * r->second_component_of_count;
	free(r->second_component_of_list);
	J1FA(freed_bytes, r->second_component_of);
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
		total_freed_bytes += free_concept((Concept*) *key);
		JSLN(key, tbox->exists_restrictions, index);
	}
	JSLFA(freed_bytes, tbox->exists_restrictions);
	total_freed_bytes += freed_bytes;

	// free the conjunctions
	strcpy((char*) index, "");
	JSLF(key, tbox->conjunctions, index);
	while (key != NULL) {
		total_freed_bytes += free_concept((Concept*) *key);
		JSLN(key, tbox->conjunctions, index);
	}
	JSLFA(freed_bytes, tbox->conjunctions);
	total_freed_bytes += freed_bytes;

	// free the atomic concepts list
	// this is the list kept for efficiently traversing over atomic concepts
	// in hierarchy computation. the actual place for keeping atomic concepts
	// is the atomic_concepts hash. note that here we just free place reserved for
	// the atomic_concept_list. the place for reserved for the atomic concepts themselves
	// is freed below
	total_freed_bytes += sizeof(Concept*) * tbox->atomic_concept_count;
	free(tbox->atomic_concept_list);

	// free the atomic concepts
	strcpy((char*) index, "");
	JSLF(key, tbox->atomic_concepts, index);
	while (key != NULL) {
		total_freed_bytes += free_concept((Concept*) *key);
		JSLN(key, tbox->atomic_concepts, index);
	}
	JSLFA(freed_bytes, tbox->atomic_concepts);
	total_freed_bytes += freed_bytes;


	// free the atomic roles
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
