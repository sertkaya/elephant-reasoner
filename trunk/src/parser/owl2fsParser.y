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
	
	// for parsing role composition
	int role_exp_count;								/* number of roles in the role composition */
	Role* role_exps[MAX_ROLE_COMPOSITION_SIZE];		/* roles in the composition */

%}

%start ontologyDocument

%token PNAME_NS PNAME_LN IRI_REF BLANK_NODE_LABEL LANGTAG QUOTED_STRING
%token DOUBLE_CARET

/* Ontology */
%token PREFIX ONTOLOGY IMPORT

/* Annotation */
%token ANNOTATION ANNOTATION_ASSERTION SUB_ANNOTATION_PROPERTY_OF
%token ANNOTATION_PROPERTY_DOMAIN ANNOTATION_PROPERTY_RANGE

%token CLASS OBJECT_INTERSECTION_OF OBJECT_SOME_VALUES_FROM 
%token OBJECT_PROPERTY OBJECT_INVERSE_OF OBJECT_PROPERTY_CHAIN
%token SUB_CLASS_OF EQUIVALENT_CLASSES 
%token SUB_OBJECT_PROPERTY_OF TRANSITIVE_OBJECT_PROPERTY EQUIVALENT_OBJECT_PROPERTIES

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
	| axioms axiom;

/*****************************************************************************/
/* Annotation */

annotationSubject:
	IRI 
	| anonymousIndividual;

annotationValue:
	anonymousIndividual 
	| IRI 
	| literal;
	
axiomAnnotations:
	| axiomAnnotations annotation;
	
annotation:
	ANNOTATION '(' annotationAnnotations annotationProperty annotationValue ')';
	
annotationAnnotations:
	| annotationAnnotations annotation;

annotationAxiom:
	annotationAssertion
	| subAnnotationPropertyOf 
	| annotationPropertyDomain 
	| annotationPropertyRange;
	
annotationAssertion:
	ANNOTATION_ASSERTION '(' axiomAnnotations annotationProperty annotationSubject annotationValue ')';

subAnnotationPropertyOf:
	SUB_ANNOTATION_PROPERTY_OF '(' axiomAnnotations subAnnotationProperty superAnnotationProperty ')';
	
subAnnotationProperty:
	annotationProperty;
	
superAnnotationProperty:
	annotationProperty;

annotationPropertyDomain:
	ANNOTATION_PROPERTY_DOMAIN '(' axiomAnnotations annotationProperty IRI ')';

annotationPropertyRange:
	ANNOTATION_PROPERTY_RANGE '(' axiomAnnotations annotationProperty IRI ')'; 
	
/*****************************************************************************/
	
/* Definitions of OWL2 Constructs */

/* TODO: here OWL_THING, OWL_NOTHING */
class:
	IRI	{ $$.concept = get_create_atomic_concept(yytext, tbox); };

dataType:
	IRI;

objectProperty:
	IRI	{ $$.role = get_create_atomic_role(yytext, tbox); };

annotationProperty:
	IRI;

anonymousIndividual:
	nodeID;

literal:
	typedLiteral 
	| stringLiteralNoLanguage 
	| stringLiteralWithLanguage;
	
typedLiteral:
	lexicalForm DOUBLE_CARET dataType;
	
lexicalForm:
	QUOTED_STRING;
	
stringLiteralNoLanguage:
	QUOTED_STRING;
	
stringLiteralWithLanguage:
	QUOTED_STRING languageTag;
	
/* for now only these three */
classExpression:
	class 
	| objectIntersectionOf 
	| objectSomeValuesFrom;

objectIntersectionOf:
	OBJECT_INTERSECTION_OF '(' classExpression classExpression classExpressions ')' {
		cls_exps[cls_exp_count++] = $3.concept;
		cls_exps[cls_exp_count++] = $4.concept;
		$$.concept = get_create_conjunction(cls_exp_count, cls_exps, tbox);
		cls_exp_count = 0;
	};

objectSomeValuesFrom:
	OBJECT_SOME_VALUES_FROM '(' objectPropertyExpression classExpression ')' {
		$$.concept = get_create_exists_restriction($3.role, $4.concept, tbox);
	};

classExpressions:
	| classExpression classExpressions {
		if ($1.concept != NULL)
			cls_exps[cls_exp_count++] = $1.concept;
	};


axiom:
	classAxiom 
	| objectPropertyAxiom
	| annotationAxiom;

classAxiom:
	subClassOf 
	| equivalentClasses;

subClassOf:
	SUB_CLASS_OF '(' axiomAnnotations classExpression classExpression ')' {
		add_subclass_axiom(create_subclass_axiom($4.concept, $5.concept), tbox);
	};

// for parsing EquivalentClasses axioms containing more than 2 class expressions
eqClassExpressions:
	| classExpression eqClassExpressions {
		if ($1.concept != NULL)
			eq_cls_exps[eq_cls_exp_count++] = $1.concept;
	};


equivalentClasses:
	EQUIVALENT_CLASSES '(' axiomAnnotations classExpression classExpression eqClassExpressions ')' {
		eq_cls_exps[eq_cls_exp_count++] = $4.concept;
		eq_cls_exps[eq_cls_exp_count++] = $5.concept;
		int i;
		for (i = 0; i < eq_cls_exp_count - 1; ++i)
			add_eqclass_axiom(create_eqclass_axiom(eq_cls_exps[i], eq_cls_exps[i+1]), tbox);
		eq_cls_exp_count = 0;
	};

/*
equivalentClasses:
	EQUIVALENT_CLASSES '(' axiomAnnotations classExpression classExpression ')' {
		add_eqclass_axiom(create_eqclass_axiom($4.concept, $5.concept), tbox);
	};
*/

objectPropertyExpression:
	objectProperty 
	| inverseObjectProperty;

inverseObjectProperty:
	OBJECT_INVERSE_OF '(' objectProperty ')';

objectPropertyAxiom:
	equivalentObjectProperties
	| subObjectPropertyOf 
	| transitiveObjectProperty;
	
subObjectPropertyOf:
	SUB_OBJECT_PROPERTY_OF '(' axiomAnnotations subObjectPropertyExpression superObjectPropertyExpression ')' {
		add_subrole_axiom(create_subrole_axiom($4.role, $5.role), tbox);
	};

subObjectPropertyExpression:
	objectPropertyExpression 
	| propertyExpressionChain;

superObjectPropertyExpression:
	objectPropertyExpression;

propertyExpressionChain:
	OBJECT_PROPERTY_CHAIN '(' objectPropertyExpression objectPropertyExpression objectPropertyExpressions ')' {
		role_exps[role_exp_count++] = $4.role;
		role_exps[role_exp_count++] = $3.role;
		$$.role = get_create_role_composition(role_exp_count, role_exps, tbox);
		role_exp_count = 0;
	}
	
objectPropertyExpressions:
	| objectPropertyExpression objectPropertyExpressions {
		if ($1.role != NULL)
			role_exps[role_exp_count++] = $1.role;
	};

equivalentObjectProperties:
	EQUIVALENT_OBJECT_PROPERTIES '(' axiomAnnotations objectPropertyExpression objectPropertyExpression objectPropertyExpressions ')' {
		add_eqrole_axiom(create_eqrole_axiom($4.role, $5.role), tbox);
	};

transitiveObjectProperty:
	TRANSITIVE_OBJECT_PROPERTY '(' axiomAnnotations objectPropertyExpression ')' {
		add_transitive_role_axiom(create_transitive_role_axiom($4.role), tbox);
	}

%%

void yyerror(char *s) {
	fprintf(stderr, "\nline %d: %s\n", yylineno, s);
}
