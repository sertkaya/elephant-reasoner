/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
     ANNOTATION = 268,
     ANNOTATION_ASSERTION = 269,
     SUB_ANNOTATION_PROPERTY_OF = 270,
     ANNOTATION_PROPERTY_DOMAIN = 271,
     ANNOTATION_PROPERTY_RANGE = 272,
     CLASS = 273,
     OBJECT_INTERSECTION_OF = 274,
     OBJECT_SOME_VALUES_FROM = 275,
     OBJECT_PROPERTY = 276,
     OBJECT_INVERSE_OF = 277,
     OBJECT_PROPERTY_CHAIN = 278,
     SUB_CLASS_OF = 279,
     EQUIVALENT_CLASSES = 280,
     SUB_OBJECT_PROPERTY_OF = 281,
     TRANSITIVE_OBJECT_PROPERTY = 282,
     EQUIVALENT_OBJECT_PROPERTIES = 283
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
#define ANNOTATION 268
#define ANNOTATION_ASSERTION 269
#define SUB_ANNOTATION_PROPERTY_OF 270
#define ANNOTATION_PROPERTY_DOMAIN 271
#define ANNOTATION_PROPERTY_RANGE 272
#define CLASS 273
#define OBJECT_INTERSECTION_OF 274
#define OBJECT_SOME_VALUES_FROM 275
#define OBJECT_PROPERTY 276
#define OBJECT_INVERSE_OF 277
#define OBJECT_PROPERTY_CHAIN 278
#define SUB_CLASS_OF 279
#define EQUIVALENT_CLASSES 280
#define SUB_OBJECT_PROPERTY_OF 281
#define TRANSITIVE_OBJECT_PROPERTY 282
#define EQUIVALENT_OBJECT_PROPERTIES 283




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


