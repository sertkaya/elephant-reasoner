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
#include <string.h>
#include <assert.h>

#include "datatypes.h"
#include "model.h"
#include "limits.h"
#include "utils.h"
#include "../hashing/hash_table.h"
#include "../hashing/hash_map.h"


char* iri_to_string(KB* kb, char* iri);
char* object_some_values_from_to_string(KB* kb, ObjectSomeValuesFrom* obj_some_values);
char* object_intersection_of_to_string(KB* kb, ObjectIntersectionOf* obj_intersection_of);
char* object_one_of_to_string(KB* kb, ObjectOneOf* nominal);

char* object_property_chain_to_string(KB* kb, ObjectPropertyChain* rc);

char* class_expression_to_string(KB* kb, ClassExpression* c) {
	switch (c->type) {
		case CLASS_TYPE:
			return iri_to_string(kb, c->description.atomic.IRI);
		case OBJECT_SOME_VALUES_FROM_TYPE:
			return object_some_values_from_to_string(kb, &(c->description.exists));
		case OBJECT_INTERSECTION_OF_TYPE:
			return object_intersection_of_to_string(kb, &(c->description.conj));
		case OBJECT_ONE_OF_TYPE:
			return object_one_of_to_string(kb, &(c->description.nominal));
		default:
			fprintf(stderr,"unknown class expression type, aborting\n");
			exit(-1);
	}
}

char* object_property_expression_to_string(KB* kb, ObjectPropertyExpression* r) {
	switch (r->type) {
		case OBJECT_PROPERTY_TYPE:
			return iri_to_string(kb, r->description.atomic.IRI);
		case OBJECT_PROPERTY_CHAIN_TYPE:
			return object_property_chain_to_string(kb, &(r->description.object_property_chain));
		default:
			fprintf(stderr,"unknown object property expression type, aborting\n");
			exit(-1);
	}
}
	
char* iri_to_string(KB* kb, char* iri) {
	char* str;

	// Check of the IRI is a full IRI
	if (iri[0] == '<') {
		// it is a full IRI, just return it
		int iri_length = strlen(iri);
		str = calloc(1, sizeof(char) * (iri_length + 1/* for the '\0' character */));
		assert(str != NULL);
		snprintf(str, iri_length + 1, "%s", iri);

		return str;

	}
	// Check if the IRI is a prefixed name (search for a ":")
	int index = 0;
	for (index = 0; iri[index] != ':' && iri[index] != '\0'; ++index);

	if (iri[index] == '\0') {
		// The IRI does not have a prefix, just return it
		str = calloc(1, sizeof(char) * (index + 1 + 1/* for the '\0' character */));
		assert(str != NULL);
		snprintf(str, index + 1, "%s", iri);

		return str;
	}

	// The IRI has a prefix
	char* prefix = NULL;
	char* prefix_name = calloc(1, sizeof(char) * (index + 1 /* for the ':' */ + 1 /* for the '\0' character */));
	assert(prefix_name != NULL);
	strncpy(prefix_name, iri, index + 1);

	if ((prefix = GET_ONTOLOGY_PREFIX(prefix_name, kb)) != NULL) {
		int prefix_length = strlen(prefix);
		str = calloc(1, sizeof(char) * (prefix_length + strlen(iri) - index /* IRI without the prefix name + 1 for the '\0' */));
		assert(str != NULL);
		// copy the prefix
		strncpy(str, prefix, prefix_length - 1 /* leave out the trailing > sign */);
		// now copy the IRI without the prefix name (part after the ':')
		int i;
		for (i = 0; iri[index + 1 + i] != '\0'; ++i) {
			str[prefix_length - 1 + i] = iri[index + 1 + i];
		}
		// append the '>'
		str[prefix_length - 1 + i] = '>';
		// append the '\0'
		++i;
		str[prefix_length - 1 + i] = '\0';

		return str;
	}
	// it is a prefixed IRI but the prefix name is not found in the prefix names map, just return the IRI
	str = calloc(1, sizeof (char) * (strlen(iri) + 1));
	assert(str != NULL);
	snprintf(str, strlen(iri), "%s", iri);

	return str;
}

char* object_some_values_from_to_string(KB* kb, ObjectSomeValuesFrom* ex) {
	char* str;

	char* object_property_str = object_property_expression_to_string(kb, ex->role);
	char* class_expression_str = class_expression_to_string(kb, ex->filler);
	int size = strlen("(ObjectSomeValuesFrom ") + strlen(object_property_str) + 1 /* for the space */ + strlen(class_expression_str) + 1 /* for the parenthesis*/;
	str = calloc(1, size);
	assert(str != NULL);
	snprintf(str, size, "(ObjectSomeValuesFrom %s %s)", object_property_str, class_expression_str);
	free(object_property_str);
	free(class_expression_str);

	return str;
}

char* object_intersection_of_to_string(KB* kb, ObjectIntersectionOf* obj_int) {
	char* str;

	char* conjunct_1_str = class_expression_to_string(kb, obj_int->conjunct1);
	char* conjunct_2_str = class_expression_to_string(kb, obj_int->conjunct2);
	int size = strlen("(ObjectIntersectionOf ") + strlen(conjunct_1_str) + 1 /* for the space */ + strlen(conjunct_2_str) + 1 /* for the parenthesis*/;
	str = calloc(1, size);
	assert(str != NULL);
	snprintf(str, size, "(ObjectIntersectionOf %s %s)", conjunct_1_str, conjunct_2_str);
	free(conjunct_1_str);
	free(conjunct_2_str);

	return str;
}

char* object_one_of_to_string(KB* kb, ObjectOneOf* nominal) {
	char* individual_str = iri_to_string(kb, nominal->individual->IRI);
	int size = strlen(individual_str) + 3 /* 2 for the curly braces, 1 for the \0 */;
	char* str = calloc(1, size);
	assert(str != NULL);
	snprintf(str, size, "{%s}", individual_str);
	free(individual_str);

	return str;
}

char* object_property_chain_to_string(KB* kb, ObjectPropertyChain* rc) {
	char* str;

	char* obj_prop_expr_1_str = object_property_expression_to_string(kb, rc->role1);
	char* obj_prop_expr_2_str = object_property_expression_to_string(kb, rc->role2);
	int size = strlen("(ObjectPropertyChain ") + strlen(obj_prop_expr_1_str) + 1 /* for the space */ + strlen(obj_prop_expr_2_str) + 1 /* for the parenthesis*/;
	str = calloc(1, size);
	assert(str != NULL);
	snprintf(str, size, "(ObjectPropertyChain %s %s)", obj_prop_expr_1_str, obj_prop_expr_2_str);
	free(obj_prop_expr_1_str);
	free(obj_prop_expr_2_str);

	return str;
}

char* subclass_of_axiom_to_string(KB* kb, SubClassOfAxiom* sc_ax) {
	char* str;

	char* class_exp_1_str = class_expression_to_string(kb, sc_ax->lhs);
	char* class_exp_2_str = class_expression_to_string(kb, sc_ax->rhs);
	int size = strlen("(SubClassOf ") + strlen(class_exp_1_str) + 1 /* for the space */ + strlen(class_exp_2_str) + 1 /* for the parenthesis*/;
	str = calloc(1, size);
	assert(str != NULL);
	snprintf(str, size, "(SubClassOf %s %s)", class_exp_1_str, class_exp_2_str);
	free(class_exp_1_str);
	free(class_exp_2_str);

	return str;
}

char* equivalent_classes_axiom_to_string(KB* kb, EquivalentClassesAxiom* ec_ax) {
	char* str;

	char* class_exp_1_str = class_expression_to_string(kb, ec_ax->lhs);
	char* class_exp_2_str = class_expression_to_string(kb, ec_ax->rhs);
	int size = strlen("(EquivalentClasses ") + strlen(class_exp_1_str) + 1 /* for the space */ + strlen(class_exp_2_str) + 1 /* for the parenthesis*/;
	str = calloc(1, size);
	assert(str != NULL);
	snprintf(str, size, "(EquivalentClasses %s %s)", class_exp_1_str, class_exp_2_str);
	free(class_exp_1_str);
	free(class_exp_2_str);

	return str;
}

char* sub_object_property_axiom_to_string(KB* kb, SubObjectPropertyOfAxiom* sub_obj_prop_ax) {
	char* str;

	char* obj_prop_exp_1_str = object_property_expression_to_string(kb, sub_obj_prop_ax->lhs);
	char* obj_prop_exp_2_str = object_property_expression_to_string(kb, sub_obj_prop_ax->rhs);
	int size = strlen("(SubObjectPropertyOf ") + strlen(obj_prop_exp_1_str) + 1 /* for the space */ + strlen(obj_prop_exp_2_str) + 1 /* for the parenthesis*/;
	str = calloc(1, size);
	assert(str != NULL);
	snprintf(str, size, "(SubObjectPropertyOf %s %s)", obj_prop_exp_1_str, obj_prop_exp_2_str);
	free(obj_prop_exp_1_str);
	free(obj_prop_exp_2_str);

	return str;
}

void print_tbox(KB* kb) {
	SetIterator it;
	SET_ITERATOR_INIT(&it, &(kb->tbox->subclass_of_axioms));
	SubClassOfAxiom* ax = (SubClassOfAxiom*) SET_ITERATOR_NEXT(&it);
	char* ax_str;
	while (ax) {
		ax_str = subclass_of_axiom_to_string(kb, ax);
		printf("%s\n", ax_str);
		free(ax_str);
		ax = SET_ITERATOR_NEXT(&it);
	}
}

void print_concept_hierarchy(KB* kb, FILE* taxonomy_fp) {

	// the prefixes
	int i;
	for (i = 0; i < kb->prefix_names.size; ++i)
		fprintf(taxonomy_fp, "Prefix(%s=%s)\n", (char*) kb->prefix_names.elements[i], (char*) GET_ONTOLOGY_PREFIX((char*) kb->prefix_names.elements[i], kb));

	// the ontology tag
	fprintf(taxonomy_fp, "\nOntology(\n");

	// just print EquivalentClasses(owl:Thing, owl:Nothing) if the ontology is inconsistent
	if (kb->inconsistent) {
		char* thing_str = class_expression_to_string(kb, kb->tbox->top_concept);
		char* nothing_str = class_expression_to_string(kb, kb->tbox->bottom_concept);
		fprintf(taxonomy_fp, "EquivalentClasses(%s %s)\n", thing_str, nothing_str);
		free(thing_str);
		free(nothing_str);
		// the closing parentheses for the ontology tag
		fprintf(taxonomy_fp, ")\n");
		return;
	}

	// for keeping track of already printed equivalence classes
	Set* printed = SET_CREATE(16);

	MapIterator map_it;
	SetIterator equivalent_classes_iterator;
	MAP_ITERATOR_INIT(&map_it, &(kb->tbox->classes));
	ClassExpression* atomic_concept = (ClassExpression*) MAP_ITERATOR_NEXT(&map_it);
	char* atomic_concept_str;
	SetIterator direct_subsumers_iterator;
	while (atomic_concept) {
		// check if the equivalence class is already printed
		if (!SET_CONTAINS(atomic_concept, printed)) {
			atomic_concept_str = class_expression_to_string(kb, atomic_concept);
			// do not print the direct subsumers of bottom
			if (atomic_concept != kb->tbox->bottom_concept) {
				// iterate over the direct subsumers and print them
				SET_ITERATOR_INIT(&direct_subsumers_iterator, &(atomic_concept->description.atomic.direct_subsumers));
				ClassExpression* direct_subsumer = (ClassExpression*) SET_ITERATOR_NEXT(&direct_subsumers_iterator);
				char* direct_subsumer_str;
				while (direct_subsumer != NULL) {
					direct_subsumer_str = class_expression_to_string(kb, direct_subsumer);
					fprintf(taxonomy_fp, "SubClassOf(%s %s)\n", atomic_concept_str, direct_subsumer_str);
					free(direct_subsumer_str);
					direct_subsumer = SET_ITERATOR_NEXT(&direct_subsumers_iterator);
				}
			}

			// print the equivalent classes
			if (atomic_concept->description.atomic.equivalent_classes.element_count > 0) {
				fprintf(taxonomy_fp, "EquivalentClasses(%s", atomic_concept_str);

				SET_ITERATOR_INIT(&equivalent_classes_iterator, &(atomic_concept->description.atomic.equivalent_classes));
				ClassExpression* equivalent_class = SET_ITERATOR_NEXT(&equivalent_classes_iterator);
				char* equivalent_class_str;
				while (equivalent_class != NULL) {
					// mark the concepts in the equivalent classes as already printed
					SET_ADD(equivalent_class, printed);
					// now print it
					equivalent_class_str = class_expression_to_string(kb, equivalent_class);
					fprintf(taxonomy_fp, " %s", equivalent_class_str);
					free(equivalent_class_str);
					equivalent_class = SET_ITERATOR_NEXT(&equivalent_classes_iterator);
				}
				fprintf(taxonomy_fp, ")\n");
			}
			free(atomic_concept_str);
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
	for (i = 0; i < kb->prefix_names.size; ++i)
		fprintf(taxonomy_fp, "Prefix(%s=%s)\n", (char*) kb->prefix_names.elements[i], (char*) GET_ONTOLOGY_PREFIX((char*) kb->prefix_names.elements[i], kb));

	// the ontology tag
	fprintf(taxonomy_fp, "\nOntology(\n");

	if (kb->inconsistent) {
		char* thing_str = class_expression_to_string(kb, kb->tbox->top_concept);
		char* nothing_str = class_expression_to_string(kb, kb->tbox->bottom_concept);
		fprintf(taxonomy_fp, "EquivalentClasses(%s %s)\n", thing_str, nothing_str);
		free(thing_str);
		free(nothing_str);
		// the closing parentheses for the ontology tag
		fprintf(taxonomy_fp, ")\n");
		return;
	}


	// Traverse the hash of nominals that are generated during preprocessing.
	MapIterator iterator;
	MAP_ITERATOR_INIT(&iterator, &(kb->generated_nominals));
	ClassExpression* nominal = (ClassExpression*) MAP_ITERATOR_NEXT(&iterator);
	SetIterator subsumers_iterator;
	char* nominal_str;
	while (nominal) {
		char* subsumer_str;
		nominal_str = iri_to_string(kb, nominal->description.nominal.individual->IRI);
		SET_ITERATOR_INIT(&subsumers_iterator, &(nominal->subsumers));
		ClassExpression* subsumer = (ClassExpression*) SET_ITERATOR_NEXT(&subsumers_iterator);
		while (subsumer != NULL) {
			if (subsumer->type == CLASS_TYPE) {
				subsumer_str = class_expression_to_string(kb, subsumer);
				fprintf(taxonomy_fp, "ClassAssertion(%s %s)\n", subsumer_str, nominal_str);
				free(subsumer_str);
			}
			subsumer = (ClassExpression*) SET_ITERATOR_NEXT(&subsumers_iterator);
		}
		free(nominal_str);
		nominal = (ClassExpression*) MAP_ITERATOR_NEXT(&iterator);
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
			"Role assertions....................: %d\n"
			"-----------------------------------\n\n",
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
			kb->tbox->disjoint_classes_axioms.element_count,
			kb->tbox->subobjectproperty_of_axioms.element_count,
			kb->tbox->equivalent_objectproperties_axioms.element_count,
			kb->tbox->transitive_objectproperty_axioms.element_count,
			kb->abox->individual_count,
			kb->abox->concept_assertion_count,
			kb->abox->role_assertion_count);
}
