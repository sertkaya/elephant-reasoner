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

#include "datatypes.h"
#include "model.h"
#include "limits.h"


void print_atomic_concept(AtomicConcept* ac);
void print_exists(Exists* exists);
void print_conjunction(Conjunction* conjunction);

void print_atomic_role(AtomicRole* ar);

void print_concept(Concept* c) {
	switch (c->type) {
		case ATOMIC_CONCEPT:
			print_atomic_concept(c->description.atomic);
			break;
		case EXISTENTIAL_RESTRICTION:
			print_exists(c->description.exists);
			break;
		case CONJUNCTION:
			print_conjunction(c->description.conj);
			break;
		default:
			fprintf(stderr,"unknown concept type, aborting\n");
			exit(-1);
	}
}

void print_role(Role* r) {
	switch (r->type) {
		case ATOMIC_ROLE:
			print_atomic_role(r->description.atomic);
			break;
		default:
			fprintf(stderr,"unknown role type, aborting\n");
			exit(-1);
	}
}
	
void print_atomic_concept(AtomicConcept* ac) {
	printf("%s ", ac->name);
}

void print_exists(Exists* ex) {
	printf("(Some ");
	print_role(ex->role);
	print_concept(ex->filler);
	printf(")");
}

void print_conjunction(Conjunction* conj) {
	printf("(And  ");
	print_concept(conj->conjunct1);
	printf("  ");
	print_concept(conj->conjunct2);
	printf(")\n");
}

void print_conjunctions(TBox* tbox) {
	uint8_t index[16];
	PWord_t conj_p;

	index[0] = '\0';
	JSLF(conj_p, tbox->conjunctions, index);
	while (conj_p != NULL) {
		print_conjunction(((Concept*) *conj_p)->description.conj);
		JSLN(conj_p, tbox->conjunctions, index);
	}
}

void print_atomic_role(AtomicRole* ar) {
	printf("%s ", ar->name);
}


void print_subclass_axiom(SubClassAxiom* sc_ax) {
	printf("(SubClassOf ");
	print_concept(sc_ax->lhs);
	printf(" ");
	print_concept(sc_ax->rhs);
	printf(")\n");
}

void print_eqclass_axiom(EqClassAxiom* ec_ax) {
	printf("(EquivalentClasses ");
	print_concept(ec_ax->lhs);
	printf(" ");
	print_concept(ec_ax->rhs);
	printf(")\n");
}

// TODO
void print_subrole_axiom(SubRoleAxiom* subrole_ax) {
	printf("(SubObjectPropertyOf ");
	// print_role(subrole_ax->lhs);
	printf(", ");
	// print_role(subrole_ax->rhs);
	// printf("%d",subrole_ax->rhs);
	printf(")\n");
}

void print_tbox(TBox* tbox) {
	int i;

	for (i = 0; i < tbox->subclass_axiom_count; i++) {
		printf("ax %d:",i);
		print_subclass_axiom(tbox->subclass_axioms[i]);
	}
}

void print_equivalent_concepts(Concept* c, FILE* taxonomy_fp) {
	Word_t index ;
	int equivalents_list_nonempty;

	index = 0;
	J1F(equivalents_list_nonempty, c->equivalent_concepts, index);
	while (equivalents_list_nonempty) {
		if (c != (Concept*) index) // to avoid EquivalentConcepts(c,c)
			fprintf(taxonomy_fp, "EquivalentConcepts(%s, %s)\n", c->description.atomic->name,
					((Concept*) index)->description.atomic->name);
		J1N(equivalents_list_nonempty, c->equivalent_concepts, index);
	}
}

void print_direct_subsumers(Concept* c, FILE* taxonomy_fp) {
	Word_t index ;
	int direct_subsumers_list_nonempty;

	index = 0;
	J1F(direct_subsumers_list_nonempty, c->direct_subsumers, index);
	while (direct_subsumers_list_nonempty) {
		fprintf(taxonomy_fp, "SubClassOf(%s %s)\n", c->description.atomic->name,
				((Concept*) index)->description.atomic->name);
		J1N(direct_subsumers_list_nonempty, c->direct_subsumers, index);
	}
}

void print_concept_hierarchy(TBox* tbox, FILE* taxonomy_fp) {
	uint8_t atomic_concept_index[MAX_CONCEPT_NAME_LENGTH];
	PWord_t pvalue;
	// for keeping track of already printed equivalence classes
	Pvoid_t printed = (Pvoid_t) NULL;
	int added_to_printed;
	Word_t equivalent_class_index;
	int equivalents_list_nonempty;

	// start with the smallest concept name
	atomic_concept_index[0] = '\0';
	JSLF(pvalue, tbox->atomic_concepts, atomic_concept_index);

	while (pvalue != NULL) {
		// print_equivalent_concepts((Concept*) *pvalue, taxonomy_fp);
		// check if the equivalence class is already printed
		J1T(added_to_printed, printed, (Word_t) *pvalue);
		if (!added_to_printed) {
			print_direct_subsumers((Concept*) *pvalue, taxonomy_fp);

			equivalent_class_index = 0;
			J1F(equivalents_list_nonempty, ((Concept*) *pvalue)->equivalent_concepts, equivalent_class_index);
			char printing_equivalents = 0;
			if (equivalents_list_nonempty) {
				fprintf(taxonomy_fp, "EquivalentClasses(%s", ((Concept*) *pvalue)->description.atomic->name);
				printing_equivalents = 1;
			}
			while (equivalents_list_nonempty) {
				// mark the concepts in the equivalent classes as already printed
				J1S(added_to_printed, printed, (Word_t) equivalent_class_index);
				// now print it
				// if (((Concept*) *pvalue) != (Concept*) equivalent_class_index) // to avoid EquivalentConcepts(c,c)
				fprintf(taxonomy_fp, " %s", ((Concept*) equivalent_class_index)->description.atomic->name);
				J1N(equivalents_list_nonempty, ((Concept*) *pvalue)->equivalent_concepts, equivalent_class_index);
			}
			if (printing_equivalents)
				fprintf(taxonomy_fp, ")\n");
		}
		JSLN(pvalue, tbox->atomic_concepts, atomic_concept_index);
	}
}

void print_short_stats(TBox* tbox) {
	printf("\n---------- KB statistics ----------\n");
	printf("atomic concepts: %d\n"
			"atomic roles: %d\n"
			"existential restrictions: %d\n"
			"unique existential restrictions: %d\n"
			"conjunctions: %d\n"
			// "unique conjunctions: %d\n"
			"binary conjunctions: %d\n"
			"unique binary conjunctions: %d\n"
			"role compositions: %d\n"
			"binary role compositions: %d\n"
			"unique binary role compositions: %d\n"
			"subclass ax: %d\n"
			"eqclass ax: %d\n"
			"subrole ax: %d\n"
			"eqrole ax: %d\n"
			"transitive role ax: %d\n",
			tbox->atomic_concept_count,
			tbox->atomic_role_count,
			tbox->exists_restriction_count,
			tbox->unique_exists_restriction_count,
			tbox->conjunction_count,
			// tbox->unique_conjunction_count,
			tbox->binary_conjunction_count,
			tbox->unique_binary_conjunction_count,
			tbox->role_composition_count,
			tbox->binary_role_composition_count,
			tbox->unique_binary_role_composition_count,
			tbox->subclass_axiom_count,
			tbox->eqclass_axiom_count,
			tbox->subrole_axiom_count,
			tbox->eqrole_axiom_count,
			tbox->transitive_role_axiom_count);
}
