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
#include <stdlib.h>

#include "datatypes.h"
#include "model.h"
#include "limits.h"
#include "../hashing/hash_table.h"
#include "../hashing/hash_map.h"


void print_atomic_concept(Class* ac);
void print_exists(ObjectSomeValuesFrom* exists);
void print_conjunction(ObjectIntersectionOf* conjunction);
void print_nominal(ObjectOneOf* n);

void print_atomic_role(ObjectProperty* ar);
void print_role_composition(ObjectPropertyChain* rc);

void print_concept(ClassExpression* c) {
	switch (c->type) {
		case CLASS_TYPE:
			print_atomic_concept(&(c->description.atomic));
			break;
		case OBJECT_SOME_VALUES_FROM_TYPE:
			print_exists(&(c->description.exists));
			break;
		case OBJECT_INTERSECTION_OF_TYPE:
			print_conjunction(&(c->description.conj));
			break;
		case OBJECT_ONE_OF_TYPE:
			print_nominal(&(c->description.nominal));
			break;
		default:
			fprintf(stderr,"unknown concept type, aborting\n");
			exit(-1);
	}
}

void print_role(ObjectPropertyExpression* r) {
	switch (r->type) {
		case OBJECT_PROPERTY_TYPE:
			print_atomic_role(&(r->description.atomic));
			break;
		case OBJECT_PROPERTY_CHAIN_TYPE:
			print_role_composition(&(r->description.object_property_chain));
			break;
		default:
			fprintf(stderr,"unknown role type, aborting\n");
			exit(-1);
	}
}
	
void print_atomic_concept(Class* ac) {
	printf("%s ", ac->IRI);
}

void print_exists(ObjectSomeValuesFrom* ex) {
	printf("(Some ");
	print_role(ex->role);
	print_concept(ex->filler);
	printf(")");
}

void print_conjunction(ObjectIntersectionOf* conj) {
	printf("(And  ");
	print_concept(conj->conjunct1);
	printf("  ");
	print_concept(conj->conjunct2);
	printf(")\n");
}

void print_nominal(ObjectOneOf* n) {
	printf("{%s} ", n->individual->IRI);
}

void print_conjunctions(TBox* tbox) {

	MapIterator map_it;
	MAP_ITERATOR_INIT(&map_it, &(tbox->object_intersection_of_exps));
	void* map_element = MAP_ITERATOR_NEXT(&map_it);
	while (map_element) {
		print_conjunction(&(((ClassExpression*) map_element)->description.conj));
		map_element = MAP_ITERATOR_NEXT(&map_it);
	}
}

void print_atomic_role(ObjectProperty* ar) {
	printf("%s ", ar->IRI);
}

void print_role_composition(ObjectPropertyChain* rc) {
	printf("ObjectPropertyChain( ");
	print_role(rc->role1);
	printf(" ");
	print_role(rc->role2);
	printf(")\n");
}

void print_subclass_axiom(SubClassOfAxiom* sc_ax) {
	printf("(SubClassOf ");
	print_concept(sc_ax->lhs);
	printf(" ");
	print_concept(sc_ax->rhs);
	printf(")\n");
}

void print_eqclass_axiom(EquivalentClassesAxiom* ec_ax) {
	printf("(EquivalentClasses ");
	print_concept(ec_ax->lhs);
	printf(" ");
	print_concept(ec_ax->rhs);
	printf(")\n");
}

// TODO
void print_subrole_axiom(SubObjectPropertyOfAxiom* subrole_ax) {
	printf("(SubObjectPropertyOf ");
	// print_role(subrole_ax->lhs);
	printf(", ");
	// print_role(subrole_ax->rhs);
	// printf("%d",subrole_ax->rhs);
	printf(")\n");
}

void print_tbox(TBox* tbox) {
	SetIterator it;
	SET_ITERATOR_INIT(&it, &(tbox->subclass_of_axioms));
	void* ax = SET_ITERATOR_NEXT(&it);
	while (ax) {
		print_subclass_axiom((SubClassOfAxiom*) ax);
		ax = SET_ITERATOR_NEXT(&it);
	}
}

void print_direct_subsumers(TBox* tbox, ClassExpression* c, FILE* taxonomy_fp) {
	SetIterator direct_subsumers_iterator;
	SET_ITERATOR_INIT(&direct_subsumers_iterator, &(c->description.atomic.direct_subsumers));
	void* direct_subsumer = SET_ITERATOR_NEXT(&direct_subsumers_iterator);

	while (direct_subsumer != NULL) {
		fprintf(taxonomy_fp, "SubClassOf(%s %s)\n", c->description.atomic.IRI,
				((ClassExpression*) direct_subsumer)->description.atomic.IRI);
		direct_subsumer = SET_ITERATOR_NEXT(&direct_subsumers_iterator);
	}
}

void print_concept_hierarchy(KB* kb, FILE* taxonomy_fp) {

	// the prefixes
	int i;
	for (i = 0; i < kb->prefix_count; ++i)
		fprintf(taxonomy_fp, "Prefix(%s=%s)\n", kb->prefix_names_list[i], kb->prefix_list[i]);

	// the ontology tag
	fprintf(taxonomy_fp, "\nOntology(\n");


	if (kb->inconsistent) {
		fprintf(taxonomy_fp, "EquivalentClasses(owl:Thing owl:Nothing)\n");
		// the closing parentheses for the ontology tag
		fprintf(taxonomy_fp, ")\n");
		return;
	}

	// for keeping track of already printed equivalence classes
	Set* printed = SET_CREATE(10);

	MapIterator map_it;
	SetIterator equivalent_classes_iterator;
	MAP_ITERATOR_INIT(&map_it, &(kb->tbox->classes));
	void* atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	while (atomic_concept) {
		// print_equivalent_concepts((Concept*) *pvalue, taxonomy_fp);
		// check if the equivalence class is already printed
		if (!SET_CONTAINS(((ClassExpression*) atomic_concept), printed)) {
			// do not print the direct subsumers of bottom
			if ((ClassExpression*) atomic_concept != kb->tbox->bottom_concept)
				print_direct_subsumers(kb->tbox, (ClassExpression*) atomic_concept, taxonomy_fp);

			char printing_equivalents = 0;
			if (((ClassExpression*) atomic_concept)->description.atomic.equivalent_classes.element_count > 0) {
				fprintf(taxonomy_fp, "EquivalentClasses(%s", ((ClassExpression*) atomic_concept)->description.atomic.IRI);
				printing_equivalents = 1;
			}
			SET_ITERATOR_INIT(&equivalent_classes_iterator, &(((ClassExpression*) atomic_concept)->description.atomic.equivalent_classes));
			ClassExpression* equivalent_class = SET_ITERATOR_NEXT(&equivalent_classes_iterator);
			while (equivalent_class != NULL) {
				// mark the concepts in the equivalent classes as already printed
				SET_ADD(equivalent_class, printed);
				// now print it
				fprintf(taxonomy_fp, " %s", equivalent_class->description.atomic.IRI);
				equivalent_class = SET_ITERATOR_NEXT(&equivalent_classes_iterator);
			}
			if (printing_equivalents)
				fprintf(taxonomy_fp, ")\n");
		}
		atomic_concept = MAP_ITERATOR_NEXT(&map_it);
	}
	SET_FREE(printed);

	// the closing parentheses for the ontology tag
	fprintf(taxonomy_fp, ")\n");

	return;
}

void print_individual_types(KB* kb, FILE* taxonomy_fp) {

	// the prefixes
	int i;
	for (i = 0; i < kb->prefix_count; ++i)
		fprintf(taxonomy_fp, "Prefix(%s=%s)\n", kb->prefix_names_list[i], kb->prefix_list[i]);

	// the ontology tag
	fprintf(taxonomy_fp, "\nOntology(\n");

	if (kb->inconsistent) {
		fprintf(taxonomy_fp, "EquivalentClasses(owl:Thing owl:Nothing)\n");
		// the closing parentheses for the ontology tag
		fprintf(taxonomy_fp, ")\n");
		return;
	}

	// Traverse the hash of nominals that are generated during preprocessing.
	HashMapElement* node = HASH_MAP_LAST_ELEMENT(kb->generated_nominals);
	ClassExpression* nominal = NULL;
	SetIterator subsumers_iterator;
	while (node) {
		nominal = (ClassExpression*) node->value;


		SET_ITERATOR_INIT(&subsumers_iterator, &(nominal->subsumers));
		ClassExpression* subsumer = (ClassExpression*) SET_ITERATOR_NEXT(&subsumers_iterator);
		while (subsumer != NULL) {
			if (subsumer->type == CLASS_TYPE)
				fprintf(taxonomy_fp, "ClassAssertion(%s %s)\n",
						subsumer->description.atomic.IRI,
						nominal->description.nominal.individual->IRI);
			subsumer = (ClassExpression*) SET_ITERATOR_NEXT(&subsumers_iterator);
		}

		/*
		for (i = 0; i < nominal->subsumer_count; ++i)
			if (nominal->subsumer_list[i]->type == ATOMIC_CONCEPT)
				fprintf(taxonomy_fp, "ClassAssertion(%s %s)\n",
						nominal->subsumer_list[i]->description.atomic->name,
						nominal->description.nominal->individual->name);
		*/
		node = HASH_MAP_PREVIOUS_ELEMENT(node);
	}

	// the closing parentheses for the ontology tag
	fprintf(taxonomy_fp, ")\n");

	return;
}

// void print_short_stats(TBox* tbox, ABox* abox) {
void print_short_stats(KB* kb) {
	printf("\n---------- KB statistics ----------\n");
	printf( "Atomic concepts....................: %d\n"
			"Atomic roles.......................: %d\n"
			"Existential restrictions...........: %d\n"
			"Unique existential restrictions....: %d\n"
			"Conjunction........................: %d\n"
			// "unique conjunctions: %d\n"
			"Binary conjunctions................: %d\n"
			"Unique binary conjunctions.........: %d\n"
			"Role compositions..................: %d\n"
			"Binary role compositions...........: %d\n"
			"Unique binary role compositions....: %d\n"
			"SubClass axioms....................: %d\n"
			"EquivalentClass axioms.............: %d\n"
			"Disjointness axioms................: %d\n"
			"SubObjectProperty axioms...........: %d\n"
			"EquivalentObjectProperty axioms....: %d\n"
			"TransitiveObjectProperty axioms....: %d\n"
			"Individuals........................: %d\n"
			"Concept assertions.................: %d\n"
			"Role assertions....................: %d\n",
			kb->tbox->classes.element_count,
			kb->tbox->object_properties.element_count,
			kb->tbox->object_some_values_from_exps_count,
			kb->tbox->object_some_values_from_exps.element_count,
			kb->tbox->object_intersection_of_exps_count,
			kb->tbox->binary_object_intersection_of_exps_count,
			kb->tbox->object_intersection_of_exps.element_count,
			kb->tbox->role_composition_count,
			kb->tbox->binary_role_composition_count,
			kb->tbox->object_property_chains.element_count,
			kb->tbox->subclass_of_axioms.element_count,
			kb->tbox->equivalent_classes_axioms.element_count,
			kb->tbox->disjointclasses_axiom_count,
			kb->tbox->subobjectproperty_of_axioms.element_count,
			kb->tbox->equivalent_objectproperties_axioms.element_count,
			kb->tbox->transitive_objectproperty_axioms.element_count,
			kb->abox->individual_count,
			kb->abox->concept_assertion_count,
			kb->abox->role_assertion_count);
}
