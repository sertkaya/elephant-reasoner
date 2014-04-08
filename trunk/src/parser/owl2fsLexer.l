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


DIGIT					[0-9]
PN_CHARS_BASE			[A-Za-z]
PN_CHARS_U				({PN_CHARS_BASE}|_)
PN_CHARS				({PN_CHARS_U}|-|{DIGIT})
PN_PREFIX				{PN_CHARS_BASE}(({PN_CHARS}|\.)*{PN_CHARS})?
PN_LOCAL				({PN_CHARS_U}|{DIGIT})(({PN_CHARS}|\.)*{PN_CHARS})?
PNAME_NS		 		{PN_PREFIX}?:
PNAME_LN				{PNAME_NS}{PN_LOCAL}
IRI_REF					<[^<>"{}|\^`\\ ]*>
BLANK_NODE_LABEL		_:{PN_LOCAL}
LANGTAG					@[A-Za-z]+(-[A-Za-z0-9]+)*
QUOTED_STRING			\"([^\"\\]|\\\"|\\\\)*\"
DOUBLE_CARET			"^^"
	// PrefixedName			({PNAME_LN}|{PNAME_NS})
	// IRIref					({IRI_REF}|{PrefixedName})


%{
	#include <stdio.h>
	#include "y.tab.h"
	#include "../model/datatypes.h"
	#include "../model/model.h"

	// #define YYSTYPE Expression
	extern YYSTYPE yylval;

	void yyerror(char *);
%}

%%

\n										{ ++yylineno; }
										/* skip whitespace */
[ \t]									;
										/* skip comments */
	/* "//".*\n								;*/
										
[()=]									{ return *yytext; }

										/* Ontology */
Prefix									{ return PREFIX; }
Ontology 								{ return ONTOLOGY; }
Import									{ return IMPORT; }

Declaration								{ return DECLARATION; }	
Class									{ return CLASS; }
ObjectProperty							{ return OBJECT_PROPERTY; }
Datatype								{ return DATATYPE; }
DatatypeDefinition						{ return DATATYPE_DEFINITION; }
HasKey									{ return HAS_KEY; }
NamedIndividual							{ return NAMED_INDIVIDUAL; }
SameIndividual							{ return SAME_INDIVIDUAL; }
DifferentIndividuals					{ return DIFFERENT_INDIVIDUALS; }

ClassAssertion							{ return CLASS_ASSERTION; }
ObjectPropertyAssertion					{ return OBJECT_PROPERTY_ASSERTION; }
NegativeObjectPropertyAssertion			{ return NEGATIVE_OBJECT_PROPERTY_ASSERTION; }

AnnotationAssertion						{ return ANNOTATION_ASSERTION; }
Annotation								{ return ANNOTATION; }
AnnotationProperty						{ return ANNOTATION_PROPERTY; }
SubAnnotationPropertyOf					{ return SUB_ANNOTATION_PROPERTY_OF; }
AnnotationPropertyDomain				{ return ANNOTATION_PROPERTY_DOMAIN; }
AnnotationPropertyRange					{ return ANNOTATION_PROPERTY_RANGE; }

										/* Class Expression */
ObjectIntersectionOf					{ return OBJECT_INTERSECTION_OF; }
ObjectSomeValuesFrom					{ return OBJECT_SOME_VALUES_FROM; }

ObjectPropertyChain						{ return OBJECT_PROPERTY_CHAIN; }

DataIntersectionOf						{ return DATA_INTERSECTION_OF; }
DataOneOf								{ return DATA_ONE_OF; }

DataProperty							{ return DATA_PROPERTY; }
DataPropertyDomain						{ return DATA_PROPERTY_DOMAIN; }
DataPropertyRange						{ return DATA_PROPERTY_RANGE; }
FunctionalDataProperty					{ return FUNCTIONAL_DATA_PROPERTY; }

DataPropertyAssertion					{ return DATA_PROPERTY_ASSERTION; }
NegativeDataPropertyAssertion			{ return NEGATIVE_DATA_PROPERTY_ASSERTION; }
 
SubDataPropertyOf						{ return SUB_DATA_PROPERTY_OF; }
EquivalentDataProperties				{ return EQUIVALENT_DATA_PROPERTIES; }
										/* Class Axiom */
SubClassOf								{ return SUB_CLASS_OF; }
EquivalentClasses						{ return EQUIVALENT_CLASSES; }
DisjointClasses							{ return DISJOINT_CLASSES; }

										/* Object Property Axiom */
EquivalentObjectProperties				{ return EQUIVALENT_OBJECT_PROPERTIES; }
SubObjectPropertyOf						{ return SUB_OBJECT_PROPERTY_OF; }
ObjectPropertyDomain					{ return OBJECT_PROPERTY_DOMAIN; }
ObjectPropertyRange						{ return OBJECT_PROPERTY_RANGE; }
ReflexiveObjectProperty					{ return REFLEXIVE_OBJECT_PROPERTY; }
TransitiveObjectProperty				{ return TRANSITIVE_OBJECT_PROPERTY; }


{IRI_REF}								{ return IRI_REF; }
{PNAME_NS}								{ return PNAME_NS; }
{PNAME_LN}								{ return PNAME_LN; }
{BLANK_NODE_LABEL}						{ return BLANK_NODE_LABEL; }
{LANGTAG}								{ return LANGTAG; }
{QUOTED_STRING}							{ return QUOTED_STRING; }
{DOUBLE_CARET}							{ return DOUBLE_CARET; }

%%

int yywrap(void) {
	return 1;
}
