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


%{
	#include <stdio.h>
	#include <assert.h>
	#include <string.h>
	#include "datatypes.h"
	#include "../model/datatypes.h"
	#include "../model/model.h"
	#include "../model/limits.h"
	
	#define YYSTYPE Expression

	char* yytext;
	int yylex(void);
	int yylineno;
	// void yyerror(TBox* tbox, ABox* abox, char* msg);
	void yyerror(KB* kb, char* msg);
	extern FILE *yyin;
	// extern TBox* tbox;
	extern KB* kb;

	// for parsing conjunction
	// number of conjuncts in an ObjectIntersectionOf expression
	int conjunct_count;
	// the conjuncts in an ObjectIntersectionOf expression
	ClassExpression* conjuncts[MAX_CONJUNCT_COUNT];

	// for parsing EquivalentClasses axioms containing more than 2 class expressions
	// number of class expressions in an EquivalentClasses axiom 
	int equivalent_classes_count;
	// the number of class expressions in an EquivalentClasses axiom
	ClassExpression* equivalent_classes[MAX_EQ_CLASS_EXP_COUNT];
	
	// for parsing EquivalentObjectProperties axioms containing more than 2 object properties
	// the number of object property expressions in an EquivalentObjectProperties axiom
	int equivalent_objectproperties_count;
	// the object property expressions in an EquivalentObjectProperties axiom
	ObjectPropertyExpression* equivalent_objectproperties[MAX_EQ_ROLE_EXP_COUNT];
	
	// for parsing ObjectPropertyChain expressions
	// the number of object property expressions in an object property chain expression
	int objectproperty_chain_components_count;
	// the object property expressions in an object property chain expression
	ObjectPropertyExpression* objectproperty_chain_components[MAX_ROLE_COMPOSITION_SIZE];
	
	// for parsing HasKey expressions
	// the number of object properties in an HasKey expression
	int haskey_objectproperty_expression_count;
	// the object properties in an HasKey expression
	ObjectPropertyExpression* haskey_objectproperties[MAX_ROLE_COMPOSITION_SIZE];
	
	// for parsing DisjointClasses axioms
	// number of classes in a DisjointClasses axiom 
	int disjoint_classes_count;
	// the class expressions in a DisjointClasses axiom
	ClassExpression* disjoint_classes[MAX_DISJ_CLASS_EXP_COUNT];

	// for parsing SameIndividual axioms
	int same_individuals_count;
	// the individuals in a SameIndividual axiom
	Individual* same_individuals[MAX_SAME_INDIVIDUAL_COUNT];

	// for parsing DifferentIndividuals axioms
	int different_individuals_count;
	// the individuals in a DifferentIndividuals axiom
	Individual* different_individuals[MAX_DIFFERENT_INDIVIDUALS_COUNT];

	void unsupported_feature(char* feature);
%}

%parse-param {KB* kb} 

%start ontologyDocument

%token PNAME_NS PNAME_LN IRI_REF BLANK_NODE_LABEL LANGTAG QUOTED_STRING
%token DOUBLE_CARET

/* Ontology */
%token PREFIX ONTOLOGY IMPORT
%token DECLARATION

/* Annotation */
%token ANNOTATION ANNOTATION_ASSERTION SUB_ANNOTATION_PROPERTY_OF
%token ANNOTATION_PROPERTY ANNOTATION_PROPERTY_DOMAIN ANNOTATION_PROPERTY_RANGE

%token CLASS 
%token OBJECT_INTERSECTION_OF OBJECT_ONE_OF OBJECT_SOME_VALUES_FROM OBJECT_HAS_VALUE OBJECT_HAS_SELF
%token OBJECT_PROPERTY OBJECT_PROPERTY_CHAIN OBJECT_PROPERTY_DOMAIN OBJECT_PROPERTY_RANGE
%token DATA_INTERSECTION_OF DATA_ONE_OF DATA_SOME_VALUES_FROM DATA_HAS_VALUE 
%token DATA_PROPERTY SUB_DATA_PROPERTY_OF EQUIVALENT_DATA_PROPERTIES DATA_PROPERTY_DOMAIN DATA_PROPERTY_RANGE FUNCTIONAL_DATA_PROPERTY 
%token DATA_PROPERTY_ASSERTION NEGATIVE_DATA_PROPERTY_ASSERTION
%token DATATYPE DATATYPE_DEFINITION 
%token HAS_KEY 
%token NAMED_INDIVIDUAL SAME_INDIVIDUAL DIFFERENT_INDIVIDUALS 
%token OBJECT_PROPERTY_ASSERTION CLASS_ASSERTION NEGATIVE_OBJECT_PROPERTY_ASSERTION 
%token SUB_CLASS_OF EQUIVALENT_CLASSES DISJOINT_CLASSES
%token SUB_OBJECT_PROPERTY_OF TRANSITIVE_OBJECT_PROPERTY EQUIVALENT_OBJECT_PROPERTIES REFLEXIVE_OBJECT_PROPERTY

%%

/*****************************************************************************/
/* General Definitions */

languageTag:
	LANGTAG;

nodeID:
	BLANK_NODE_LABEL;	

fullIRI:
	IRI_REF { 
		$$.text = strdup(yytext); 
	};	
	
prefixName:
	PNAME_NS { 
		$$.text = strdup(yytext); 
	};
	
abbreviatedIRI:
	PNAME_LN;
	
IRI:
	fullIRI 
	| abbreviatedIRI;

/*****************************************************************************/
/* Ontology */
ontologyDocument:
	prefixDeclaration ontology;
	
prefixDeclaration:
	| prefixDeclaration PREFIX '(' prefixName '=' fullIRI ')' {
		create_prefix($4.text, $6.text, kb);
		/*
		// free the name, no need to keep it in the hash of prefixes
		// do not free the prefix itself, we need to store it in the hash
		free($4.text);
		*/
	};
	
ontology:
	ONTOLOGY '(' ontologyIRI versionIRI directlyImportsDocuments ontologyAnnotations axioms ')' 
	| ONTOLOGY '(' ontologyIRI directlyImportsDocuments ontologyAnnotations axioms ')' 
	| ONTOLOGY '(' directlyImportsDocuments ontologyAnnotations axioms ')';

ontologyIRI:
	IRI;

versionIRI:
	IRI;
	
directlyImportsDocuments:
	| directlyImportsDocuments IMPORT '(' IRI ')';
	
ontologyAnnotations:
	| ontologyAnnotations annotation;
	
axioms:
	| axioms Axiom;

Declaration:
	DECLARATION '(' axiomAnnotations Entity ')'

Entity:
	CLASS '(' Class ')'
    | DATATYPE '(' Datatype ')'
    | OBJECT_PROPERTY '(' ObjectProperty ')'
    | DATA_PROPERTY '(' DataProperty ')'
    | ANNOTATION_PROPERTY '(' AnnotationProperty ')'
    | NAMED_INDIVIDUAL '(' NamedIndividual ')';

/*****************************************************************************/
/* Annotation */

anonymousIndividual:
	nodeID;

annotationSubject:
	IRI 
	| anonymousIndividual;

annotationValue:
	anonymousIndividual 
	| IRI 
	| Literal;
	
axiomAnnotations:
	| axiomAnnotations annotation;
	
annotation:
	ANNOTATION '(' annotationAnnotations AnnotationProperty annotationValue ')';
	
annotationAnnotations:
	| annotationAnnotations annotation;

AnnotationAxiom:
	annotationAssertion
	| subAnnotationPropertyOf 
	| annotationPropertyDomain 
	| annotationPropertyRange;
	
annotationAssertion:
	ANNOTATION_ASSERTION '(' axiomAnnotations AnnotationProperty annotationSubject annotationValue ')';

subAnnotationPropertyOf:
	SUB_ANNOTATION_PROPERTY_OF '(' axiomAnnotations subAnnotationProperty superAnnotationProperty ')';
	
subAnnotationProperty:
	AnnotationProperty;
	
superAnnotationProperty:
	AnnotationProperty;

annotationPropertyDomain:
	ANNOTATION_PROPERTY_DOMAIN '(' axiomAnnotations AnnotationProperty IRI ')';

annotationPropertyRange:
	ANNOTATION_PROPERTY_RANGE '(' axiomAnnotations AnnotationProperty IRI ')'; 
	
/*****************************************************************************/
	

Class:
	IRI	{ $$.concept = get_create_atomic_concept(yytext, kb->tbox); };

Datatype:
	IRI;

ObjectProperty:
	IRI	{ $$.role = get_create_atomic_role(yytext, kb->tbox); };

DataProperty:
	IRI { 
		unsupported_feature("DataProperty");
	};

AnnotationProperty:
	IRI;

Individual:
	NamedIndividual;

NamedIndividual:
	IRI	{ $$.individual = get_create_individual(yytext, kb->abox); };

Literal:
	typedLiteral 
	| stringLiteralNoLanguage 
	| stringLiteralWithLanguage;
	
typedLiteral:
	lexicalForm DOUBLE_CARET Datatype;
	
lexicalForm:
	QUOTED_STRING;
	
stringLiteralNoLanguage:
	QUOTED_STRING;
	
stringLiteralWithLanguage:
	QUOTED_STRING languageTag;

	
ObjectPropertyExpression:
	ObjectProperty;

DataPropertyExpression:
	DataProperty;


DataRange:
	Datatype
	| DataIntersectionOf
	| DataOneOf;

	// TODO:
DataIntersectionOf:
	DATA_INTERSECTION_OF '(' DataRange DataRange dataRanges ')' {
		unsupported_feature("DataIntersectionOf");
	};

dataRanges:
	| DataRange dataRanges {
	};

	// TODO:
DataOneOf:
	DATA_ONE_OF '(' Literal ')' {
		unsupported_feature("DataOneOf");
	}


ClassExpression:
	Class 
	| ObjectIntersectionOf 
	| ObjectOneOf
	| ObjectSomeValuesFrom
	| ObjectHasValue
	| ObjectHasSelf
	| DataSomeValuesFrom
	| DataHasValue; 

conjuncts:
	| ClassExpression conjuncts {
		if ($1.concept != NULL)
			conjuncts[conjunct_count++] = $1.concept;
	};


ObjectIntersectionOf:
	OBJECT_INTERSECTION_OF '(' ClassExpression ClassExpression conjuncts ')' {
		conjuncts[conjunct_count++] = $3.concept;
		conjuncts[conjunct_count++] = $4.concept;
		$$.concept = get_create_conjunction(conjunct_count, conjuncts, kb->tbox);
		conjunct_count = 0;
	};


// OWL2 EL allows only one individual in an ObjectOneOf description
ObjectOneOf:
	OBJECT_ONE_OF '(' Individual ')' {
		$$.concept = get_create_nominal($3.individual, kb->tbox);
	};

ObjectSomeValuesFrom:
	OBJECT_SOME_VALUES_FROM '(' ObjectPropertyExpression ClassExpression ')' {
		$$.concept = get_create_exists_restriction($3.role, $4.concept, kb->tbox);
	};

	// TODO:
ObjectHasValue:
	OBJECT_HAS_VALUE '(' ObjectPropertyExpression Individual ')' {
		unsupported_feature("ObjecHasValue");
	};

	// TODO:
ObjectHasSelf:
	OBJECT_HAS_SELF '(' ObjectPropertyExpression ')' {
		unsupported_feature("ObjecHasSelf");
	};

	// 4 shift/reduce conflicts due to the dataPropertyExpressions in the middle
DataSomeValuesFrom:
	DATA_SOME_VALUES_FROM '(' DataPropertyExpression dataPropertyExpressions DataRange ')' {
		unsupported_feature("DataSomeValuesFrom");
	};

	// TODO:
dataPropertyExpressions:
	| DataPropertyExpression dataPropertyExpressions {
		unsupported_feature("dataPropertyExpressions");
	};

	// TODO:
DataHasValue:
	DATA_HAS_VALUE '(' DataPropertyExpression Literal ')' {
		unsupported_feature("DataHasValue");
		// for now just return the top concept
		$$.concept = kb->tbox->top_concept;
	};

Axiom:
	Declaration
	| ClassAxiom 
	| ObjectPropertyAxiom
	| DataPropertyAxiom
	| DatatypeDefinition
	| HasKey
	| Assertion
	| AnnotationAxiom;

ClassAxiom:
	SubClassOf 
	| EquivalentClasses
	| DisjointClasses;

SubClassOf:
	SUB_CLASS_OF '(' axiomAnnotations subClassExpression superClassExpression ')' {
		ADD_SUBCLASS_OF_AXIOM(create_subclass_axiom($4.concept, $5.concept), kb->tbox);
	};

subClassExpression:
	ClassExpression;

superClassExpression:
	ClassExpression;

// TODO: move the creation of binary axioms to preprocessing
EquivalentClasses:
	EQUIVALENT_CLASSES '(' axiomAnnotations ClassExpression ClassExpression equivalentClassExpressions ')' {
		equivalent_classes[equivalent_classes_count++] = $4.concept;
		equivalent_classes[equivalent_classes_count++] = $5.concept;
		int i;
		for (i = 0; i < equivalent_classes_count - 1; ++i)
			ADD_EQUIVALENT_CLASSES_AXIOM(create_eqclass_axiom(equivalent_classes[i], equivalent_classes[i+1]), kb->tbox);
		equivalent_classes_count = 0;
	};

// for parsing EquivalentClasses axioms containing more than 2 class expressions
equivalentClassExpressions:
	| ClassExpression equivalentClassExpressions {
		if ($1.concept != NULL)
			equivalent_classes[equivalent_classes_count++] = $1.concept;
	};

DisjointClasses:
	DISJOINT_CLASSES '(' axiomAnnotations ClassExpression ClassExpression disjointClassExpressions ')' {
		disjoint_classes[disjoint_classes_count++] = $4.concept;
		disjoint_classes[disjoint_classes_count++] = $5.concept;
		ADD_DISJOINT_CLASSES_AXIOM(create_disjointclasses_axiom(disjoint_classes_count, disjoint_classes), kb->tbox);
		disjoint_classes_count = 0;
	};


disjointClassExpressions:
	| ClassExpression disjointClassExpressions {
		if ($1.concept != NULL)
			disjoint_classes[disjoint_classes_count++] = $1.concept;
	};


ObjectPropertyAxiom:
	EquivalentObjectProperties 
	| SubObjectPropertyOf 
	| ObjectPropertyDomain 
	| ObjectPropertyRange 
	| ReflexiveObjectProperty 
	| TransitiveObjectProperty;

	
SubObjectPropertyOf:
	SUB_OBJECT_PROPERTY_OF '(' axiomAnnotations subObjectPropertyExpression superObjectPropertyExpression ')' {
		ADD_SUBOBJECTPROPERTY_OF_AXIOM(create_subrole_axiom($4.role, $5.role), kb->tbox);
	};

subObjectPropertyExpression:
	ObjectPropertyExpression 
	| propertyExpressionChain;

propertyExpressionChain:
	OBJECT_PROPERTY_CHAIN '(' ObjectPropertyExpression ObjectPropertyExpression chainObjectPropertyExpressions ')' {
		objectproperty_chain_components[objectproperty_chain_components_count++] = $4.role;
		objectproperty_chain_components[objectproperty_chain_components_count++] = $3.role;
		$$.role = get_create_role_composition(objectproperty_chain_components_count, objectproperty_chain_components, kb->tbox);
		objectproperty_chain_components_count = 0;
	}

chainObjectPropertyExpressions:
	| ObjectPropertyExpression chainObjectPropertyExpressions {
		if ($1.role != NULL)
			objectproperty_chain_components[objectproperty_chain_components_count++] = $1.role;
	};

superObjectPropertyExpression:
	ObjectPropertyExpression;

// TODO: move the creation of binary axioms to preprocessing
EquivalentObjectProperties:
	EQUIVALENT_OBJECT_PROPERTIES '(' axiomAnnotations ObjectPropertyExpression ObjectPropertyExpression equivalentObjectPropertyExpressions ')' {
		equivalent_objectproperties[equivalent_objectproperties_count++] = $4.role;
		equivalent_objectproperties[equivalent_objectproperties_count++] = $5.role;
		int i;
		for (i = 0; i < equivalent_objectproperties_count - 1; ++i)
			ADD_EQUIVALENT_OBJECTPROPERTIES_AXIOM(create_eqrole_axiom(equivalent_objectproperties[i], equivalent_objectproperties[i+1]), kb->tbox);
		equivalent_objectproperties_count = 0;
	};
	
equivalentObjectPropertyExpressions:
	| ObjectPropertyExpression equivalentObjectPropertyExpressions {
		if ($1.role != NULL)
			equivalent_objectproperties[equivalent_objectproperties_count++] = $1.role;
	};

ObjectPropertyDomain:
	OBJECT_PROPERTY_DOMAIN '(' axiomAnnotations ObjectPropertyExpression ClassExpression ')' {
		ADD_OBJECTPROPERTY_DOMAIN_AXIOM(create_objectproperty_domain_axiom($4.role, $5.concept), kb->tbox);
	};

ObjectPropertyRange:
	OBJECT_PROPERTY_RANGE '(' axiomAnnotations ObjectPropertyExpression ClassExpression ')' {
		unsupported_feature("ObjectPropertyRange");
	};

ReflexiveObjectProperty:
	REFLEXIVE_OBJECT_PROPERTY '(' axiomAnnotations ObjectPropertyExpression ')' {
		unsupported_feature("ReflexiveObjectProperty");
	};

TransitiveObjectProperty:
	TRANSITIVE_OBJECT_PROPERTY '(' axiomAnnotations ObjectPropertyExpression ')' {
		ADD_TRANSITIVE_OBJECTPROPERTY_AXIOM(create_transitive_role_axiom($4.role), kb->tbox);
	};

DataPropertyAxiom:
    SubDataPropertyOf 
    | EquivalentDataProperties 
    | DataPropertyDomain 
    | DataPropertyRange 
    | FunctionalDataProperty;
    
SubDataPropertyOf:
	SUB_DATA_PROPERTY_OF '(' axiomAnnotations subDataPropertyExpression superDataPropertyExpression ')' {
		unsupported_feature("SubDataPropertyOf");
	};

subDataPropertyExpression:
	DataPropertyExpression;

superDataPropertyExpression:
	DataPropertyExpression;
	
EquivalentDataProperties:
	EQUIVALENT_DATA_PROPERTIES '(' axiomAnnotations DataPropertyExpression DataPropertyExpression dataPropertyExpressions ')' {
		unsupported_feature("EquivalentDataProperties");
	};
	
DataPropertyDomain:
	DATA_PROPERTY_DOMAIN '(' axiomAnnotations DataPropertyExpression ClassExpression ')' {
		unsupported_feature("DataPropertyDomain");
	};

DataPropertyRange:
	DATA_PROPERTY_RANGE '(' axiomAnnotations DataPropertyExpression DataRange ')' {
		unsupported_feature("DataPropertyRange");
	};

FunctionalDataProperty:
	FUNCTIONAL_DATA_PROPERTY '(' axiomAnnotations DataPropertyExpression ')' {
		unsupported_feature("FunctionalDataProperty");
	};

DatatypeDefinition:
	DATATYPE_DEFINITION '(' axiomAnnotations Datatype DataRange ')' {
		unsupported_feature("DatatypeDefinition");
	};

HasKey:
	HAS_KEY '(' axiomAnnotations ClassExpression '(' hasKeyObjectPropertyExpressions ')' '(' dataPropertyExpressions ')' ')' {
		unsupported_feature("HasKey");
	};

hasKeyObjectPropertyExpressions:
	| ObjectPropertyExpression hasKeyObjectPropertyExpressions {
		if ($1.role != NULL)
			haskey_objectproperties[haskey_objectproperty_expression_count++] = $1.role;
	};

Assertion:
	SameIndividual 
	| DifferentIndividuals 
	| ClassAssertion 
	| ObjectPropertyAssertion 
	| NegativeObjectPropertyAssertion 
	| DataPropertyAssertion 
	| NegativeDataPropertyAssertion;
	
sourceIndividual: 
	Individual;

targetIndividual:
	Individual;
	
targetValue:
	Literal;
	
SameIndividual:
	SAME_INDIVIDUAL '(' axiomAnnotations Individual Individual sameIndividuals ')' {
		same_individuals[same_individuals_count++] = $4.individual;
		same_individuals[same_individuals_count++] = $5.individual;
		ADD_SAME_INDIVIDUAL_AXIOM(create_same_individual_axiom(same_individuals_count, same_individuals), kb->tbox);
		same_individuals_count = 0;
	};
	
sameIndividuals:
	| Individual sameIndividuals {
		if ($1.individual != NULL)
			same_individuals[same_individuals_count++] = $1.individual;
	};

DifferentIndividuals:
	DIFFERENT_INDIVIDUALS '(' axiomAnnotations Individual Individual differentIndividuals  ')' {
		different_individuals[different_individuals_count++] = $4.individual;
		different_individuals[different_individuals_count++] = $5.individual;
		ADD_DIFFERENT_INDIVIDUALS_AXIOM(create_different_individuals_axiom(different_individuals_count, different_individuals), kb->tbox);
		different_individuals_count = 0;
	};

differentIndividuals:
	| Individual differentIndividuals {
		if ($1.individual != NULL)
			different_individuals[different_individuals_count++] = $1.individual;
	};

ClassAssertion:
	CLASS_ASSERTION '(' axiomAnnotations ClassExpression Individual ')' {
		add_concept_assertion(create_concept_assertion($5.individual, $4.concept), kb->abox);
	};

ObjectPropertyAssertion:
	OBJECT_PROPERTY_ASSERTION '(' axiomAnnotations ObjectPropertyExpression sourceIndividual targetIndividual ')' {
		add_role_assertion(create_role_assertion($4.role, $5.individual, $6.individual), kb->abox);
	};

NegativeObjectPropertyAssertion:
	NEGATIVE_OBJECT_PROPERTY_ASSERTION '(' axiomAnnotations ObjectPropertyExpression sourceIndividual targetIndividual ')' {
		unsupported_feature("NegativeObjectPropertyAssertion");
	};

DataPropertyAssertion:
	DATA_PROPERTY_ASSERTION '(' axiomAnnotations DataPropertyExpression sourceIndividual targetValue ')' {
		unsupported_feature("DataPropertyAssertion");
	};
	
NegativeDataPropertyAssertion:
	NEGATIVE_DATA_PROPERTY_ASSERTION '(' axiomAnnotations DataPropertyExpression sourceIndividual targetValue ')' {
		unsupported_feature("NegativeDataPropertyAssertion");
	};

%%

void yyerror(KB* kb, char* msg) {
	fprintf(stderr, "\nline %d: %s\n", yylineno, msg);
}

void unsupported_feature(char* feature) {
	fprintf(stderr, "unsupported feature: %s\n", feature);
}
