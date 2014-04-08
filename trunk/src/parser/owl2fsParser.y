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
	#include "../model/datatypes.h"
	#include "../model/model.h"
	#include "../model/limits.h"
	
	#define YYSTYPE Expression
	#define YYPARSE_PARAM tbox
	// %parse-param {TBox* tbox}

	char* yytext;
	int yylex(void);
	int yylineno;
	void yyerror(char *);
	extern FILE *yyin;
	// extern TBox* tbox;

	// for parsing conjunction
	int cls_exp_count;      						/* number of conjuncts */
	Concept* cls_exps[MAX_CONJUNCT_COUNT];			/* conjuncts */

	// for parsing equivalent classes axioms containing more than 2 class expressions
	int eq_cls_exp_count;							/* number of class exps */
	Concept* eq_cls_exps[MAX_EQ_CLASS_EXP_COUNT];	/* class exps */
	
	// for parsing equivalent roles axioms containing more than 2 role expressions 
	int eq_role_exp_count;							/* number of roles */
	Role* eq_role_exps[MAX_EQ_ROLE_EXP_COUNT];		/* roles in the composition */
	
	// for parsing role composition
	int comp_role_exp_count;								/* number of roles in the role composition */
	Role* comp_role_exps[MAX_ROLE_COMPOSITION_SIZE];		/* roles in the composition */
	
	// for parsing role exps in HasKey
	int hasKey_role_exp_count;								/* number of roles */
	Role* hasKey_role_exps[MAX_ROLE_COMPOSITION_SIZE];		/* roles */
	
	int disj_cls_exp_count;								/* number of classes in a disjointclasses axiom */
	Concept* disj_cls_exps[MAX_DISJ_CLASS_EXP_COUNT];	/* the class exps in a disjointclasses axiom */

	void unsupported_feature(char* feature);
%}

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
	IRI_REF;	
	
prefixName:
	PNAME_NS;
	
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
	| prefixDeclaration PREFIX '(' prefixName '=' fullIRI ')';
	
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
	IRI	{ $$.concept = get_create_atomic_concept(yytext, tbox); };

Datatype:
	IRI;

ObjectProperty:
	IRI	{ $$.role = get_create_atomic_role(yytext, tbox); };

DataProperty:
	IRI { 
		unsupported_feature("DataProperty");
	};

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


/* for now only these three */
ClassExpression:
	Class 
	| ObjectIntersectionOf 
	| ObjectOneOf
	| ObjectSomeValuesFrom
	| ObjectHasValue
	| ObjectHasSelf
	| DataSomeValuesFrom
	| DataHasValue; 

classExpressions:
	| ClassExpression classExpressions {
		if ($1.concept != NULL)
			cls_exps[cls_exp_count++] = $1.concept;
	};


ObjectIntersectionOf:
	OBJECT_INTERSECTION_OF '(' ClassExpression ClassExpression classExpressions ')' {
		cls_exps[cls_exp_count++] = $3.concept;
		cls_exps[cls_exp_count++] = $4.concept;
		$$.concept = get_create_conjunction(cls_exp_count, cls_exps, tbox);
		cls_exp_count = 0;
	};


	// TODO:
ObjectOneOf:
	OBJECT_ONE_OF '(' Individual ')' {
		unsupported_feature("ObjectOneOf");
	};

ObjectSomeValuesFrom:
	OBJECT_SOME_VALUES_FROM '(' ObjectPropertyExpression ClassExpression ')' {
		$$.concept = get_create_exists_restriction($3.role, $4.concept, tbox);
	};

	// TODO:
ObjectHasValue:
	OBJECT_HAS_VALUE '(' ObjectPropertyExpression ClassExpression ')' {
		unsupported_feature("ObjecHasValue");
	};

	// TODO:
ObjectHasSelf:
	OBJECT_HAS_SELF '(' ObjectPropertyExpression ')' {
		unsupported_feature("ObjecHasSelf");
	};

	// TODO:
dataPropertyExpressions:
	| DataPropertyExpression dataPropertyExpressions {
		unsupported_feature("dataPropertyExpressions");
	};

	// 4 shift/reduce conflicts due to the dataPropertyExpressions in the middle
DataSomeValuesFrom:
	DATA_SOME_VALUES_FROM '(' DataPropertyExpression dataPropertyExpressions DataRange ')' {
		unsupported_feature("DataSomeValuesFrom");
	};

DataHasValue:
	DATA_HAS_VALUE '(' DataPropertyExpression Literal ')' {
		unsupported_feature("DataHasValue");
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
	SUB_CLASS_OF '(' axiomAnnotations ClassExpression ClassExpression ')' {
		add_subclass_axiom(create_subclass_axiom($4.concept, $5.concept), tbox);
	};

EquivalentClasses:
	EQUIVALENT_CLASSES '(' axiomAnnotations ClassExpression ClassExpression equivalentClassExpressions ')' {
		eq_cls_exps[eq_cls_exp_count++] = $4.concept;
		eq_cls_exps[eq_cls_exp_count++] = $5.concept;
		int i;
		for (i = 0; i < eq_cls_exp_count - 1; ++i)
			add_eqclass_axiom(create_eqclass_axiom(eq_cls_exps[i], eq_cls_exps[i+1]), tbox);
		eq_cls_exp_count = 0;
	};

// for parsing EquivalentClasses axioms containing more than 2 class expressions
equivalentClassExpressions:
	| ClassExpression equivalentClassExpressions {
		if ($1.concept != NULL)
			eq_cls_exps[eq_cls_exp_count++] = $1.concept;
	};

DisjointClasses:
	DISJOINT_CLASSES '(' axiomAnnotations ClassExpression ClassExpression disjointClassExpressions ')' {
		disj_cls_exps[disj_cls_exp_count++] = $4.concept;
		disj_cls_exps[disj_cls_exp_count++] = $5.concept;
		add_disjointclasses_axiom(create_disjointclasses_axiom(disj_cls_exp_count, disj_cls_exps), tbox);
		disj_cls_exp_count = 0;
	};


disjointClassExpressions:
	| ClassExpression disjointClassExpressions {
		if ($1.concept != NULL)
			disj_cls_exps[disj_cls_exp_count++] = $1.concept;
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
		add_subrole_axiom(create_subrole_axiom($4.role, $5.role), tbox);
	};

subObjectPropertyExpression:
	ObjectPropertyExpression 
	| propertyExpressionChain;

superObjectPropertyExpression:
	ObjectPropertyExpression;

// TODO: treat axioms with multiple eqObjectPropertyExpressions!
EquivalentObjectProperties:
	EQUIVALENT_OBJECT_PROPERTIES '(' axiomAnnotations ObjectPropertyExpression ObjectPropertyExpression eqObjectPropertyExpressions ')' {
		add_eqrole_axiom(create_eqrole_axiom($4.role, $5.role), tbox);
	};
	
eqObjectPropertyExpressions:
	| ObjectPropertyExpression eqObjectPropertyExpressions {
		if ($1.role != NULL)
			eq_role_exps[eq_role_exp_count++] = $1.role;
	};

ObjectPropertyDomain:
	OBJECT_PROPERTY_DOMAIN '(' axiomAnnotations ObjectPropertyExpression ClassExpression ')' {
		unsupported_feature("ObjectPropertyDomain");
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
		add_transitive_role_axiom(create_transitive_role_axiom($4.role), tbox);
	};

DataPropertyAxiom:
    SubDataPropertyOf 
    | EquivalentDataProperties 
    | DataPropertyDomain 
    | DataPropertyRange 
    | FunctionalDataProperty;
    
SubDataPropertyOf:
	SUB_DATA_PROPERTY_OF '(' axiomAnnotations DataPropertyExpression DataPropertyExpression ')' {
		unsupported_feature("SubDataPropertyOf");
	};
	
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
			hasKey_role_exps[hasKey_role_exp_count++] = $1.role;
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
		unsupported_feature("SameIndividual");
	};
	
	// TODO:
sameIndividuals:
	| Individual sameIndividuals {
	};

DifferentIndividuals:
	DIFFERENT_INDIVIDUALS '(' axiomAnnotations Individual Individual differentIndividuals  ')' {
		unsupported_feature("DifferentIndividuals");
	};

	// TODO:
differentIndividuals:
	| Individual differentIndividuals {
	};

ClassAssertion:
	CLASS_ASSERTION '(' axiomAnnotations ClassExpression Individual ')' {
		unsupported_feature("ClassAssertion");
	};

ObjectPropertyAssertion:
	OBJECT_PROPERTY_ASSERTION '(' axiomAnnotations ObjectPropertyExpression sourceIndividual targetIndividual ')' {
		unsupported_feature("ObjectPropertyAssertion");
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

AnnotationProperty:
	IRI;

Individual:
	NamedIndividual;

NamedIndividual:
	IRI;

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
	
/* Object Property Expression */
ObjectPropertyExpression:
	ObjectProperty;

/* Data Property Expression */
DataPropertyExpression:
	DataProperty;

propertyExpressionChain:
	OBJECT_PROPERTY_CHAIN '(' ObjectPropertyExpression ObjectPropertyExpression compObjectPropertyExpressions ')' {
		comp_role_exps[comp_role_exp_count++] = $4.role;
		comp_role_exps[comp_role_exp_count++] = $3.role;
		$$.role = get_create_role_composition(comp_role_exp_count, comp_role_exps, tbox);
		comp_role_exp_count = 0;
	}

compObjectPropertyExpressions:
	| ObjectPropertyExpression compObjectPropertyExpressions {
		if ($1.role != NULL)
			comp_role_exps[comp_role_exp_count++] = $1.role;
	};


%%

void yyerror(char *s) {
	fprintf(stderr, "\nline %d: %s\n", yylineno, s);
}

void unsupported_feature(char* feature) {
	fprintf(stderr, "unsupported feature: %s\n", feature);
}
