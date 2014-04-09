/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PNAME_NS = 258,
     PNAME_LN = 259,
     IRI_REF = 260,
     BLANK_NODE_LABEL = 261,
     LANGTAG = 262,
     QUOTED_STRING = 263,
     DOUBLE_CARET = 264,
     PREFIX = 265,
     ONTOLOGY = 266,
     IMPORT = 267,
     DECLARATION = 268,
     ANNOTATION = 269,
     ANNOTATION_ASSERTION = 270,
     SUB_ANNOTATION_PROPERTY_OF = 271,
     ANNOTATION_PROPERTY = 272,
     ANNOTATION_PROPERTY_DOMAIN = 273,
     ANNOTATION_PROPERTY_RANGE = 274,
     CLASS = 275,
     OBJECT_INTERSECTION_OF = 276,
     OBJECT_ONE_OF = 277,
     OBJECT_SOME_VALUES_FROM = 278,
     OBJECT_HAS_VALUE = 279,
     OBJECT_HAS_SELF = 280,
     OBJECT_PROPERTY = 281,
     OBJECT_PROPERTY_CHAIN = 282,
     OBJECT_PROPERTY_DOMAIN = 283,
     OBJECT_PROPERTY_RANGE = 284,
     DATA_INTERSECTION_OF = 285,
     DATA_ONE_OF = 286,
     DATA_SOME_VALUES_FROM = 287,
     DATA_HAS_VALUE = 288,
     DATA_PROPERTY = 289,
     SUB_DATA_PROPERTY_OF = 290,
     EQUIVALENT_DATA_PROPERTIES = 291,
     DATA_PROPERTY_DOMAIN = 292,
     DATA_PROPERTY_RANGE = 293,
     FUNCTIONAL_DATA_PROPERTY = 294,
     DATA_PROPERTY_ASSERTION = 295,
     NEGATIVE_DATA_PROPERTY_ASSERTION = 296,
     DATATYPE = 297,
     DATATYPE_DEFINITION = 298,
     HAS_KEY = 299,
     NAMED_INDIVIDUAL = 300,
     SAME_INDIVIDUAL = 301,
     DIFFERENT_INDIVIDUALS = 302,
     OBJECT_PROPERTY_ASSERTION = 303,
     CLASS_ASSERTION = 304,
     NEGATIVE_OBJECT_PROPERTY_ASSERTION = 305,
     SUB_CLASS_OF = 306,
     EQUIVALENT_CLASSES = 307,
     DISJOINT_CLASSES = 308,
     SUB_OBJECT_PROPERTY_OF = 309,
     TRANSITIVE_OBJECT_PROPERTY = 310,
     EQUIVALENT_OBJECT_PROPERTIES = 311,
     REFLEXIVE_OBJECT_PROPERTY = 312
   };
#endif
/* Tokens.  */
#define PNAME_NS 258
#define PNAME_LN 259
#define IRI_REF 260
#define BLANK_NODE_LABEL 261
#define LANGTAG 262
#define QUOTED_STRING 263
#define DOUBLE_CARET 264
#define PREFIX 265
#define ONTOLOGY 266
#define IMPORT 267
#define DECLARATION 268
#define ANNOTATION 269
#define ANNOTATION_ASSERTION 270
#define SUB_ANNOTATION_PROPERTY_OF 271
#define ANNOTATION_PROPERTY 272
#define ANNOTATION_PROPERTY_DOMAIN 273
#define ANNOTATION_PROPERTY_RANGE 274
#define CLASS 275
#define OBJECT_INTERSECTION_OF 276
#define OBJECT_ONE_OF 277
#define OBJECT_SOME_VALUES_FROM 278
#define OBJECT_HAS_VALUE 279
#define OBJECT_HAS_SELF 280
#define OBJECT_PROPERTY 281
#define OBJECT_PROPERTY_CHAIN 282
#define OBJECT_PROPERTY_DOMAIN 283
#define OBJECT_PROPERTY_RANGE 284
#define DATA_INTERSECTION_OF 285
#define DATA_ONE_OF 286
#define DATA_SOME_VALUES_FROM 287
#define DATA_HAS_VALUE 288
#define DATA_PROPERTY 289
#define SUB_DATA_PROPERTY_OF 290
#define EQUIVALENT_DATA_PROPERTIES 291
#define DATA_PROPERTY_DOMAIN 292
#define DATA_PROPERTY_RANGE 293
#define FUNCTIONAL_DATA_PROPERTY 294
#define DATA_PROPERTY_ASSERTION 295
#define NEGATIVE_DATA_PROPERTY_ASSERTION 296
#define DATATYPE 297
#define DATATYPE_DEFINITION 298
#define HAS_KEY 299
#define NAMED_INDIVIDUAL 300
#define SAME_INDIVIDUAL 301
#define DIFFERENT_INDIVIDUALS 302
#define OBJECT_PROPERTY_ASSERTION 303
#define CLASS_ASSERTION 304
#define NEGATIVE_OBJECT_PROPERTY_ASSERTION 305
#define SUB_CLASS_OF 306
#define EQUIVALENT_CLASSES 307
#define DISJOINT_CLASSES 308
#define SUB_OBJECT_PROPERTY_OF 309
#define TRANSITIVE_OBJECT_PROPERTY 310
#define EQUIVALENT_OBJECT_PROPERTIES 311
#define REFLEXIVE_OBJECT_PROPERTY 312



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
