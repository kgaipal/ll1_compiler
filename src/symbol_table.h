/** This file is part of ll1_compiler. ll1_compiler is the LL(1) Compiler project
 *  for Compiler Construction class of spring 2010 (CS5353) at Texas Tech University,
 *  Lubbock (Instructor: Dr Daniel Cooke).
 *
 *  Copyright (C) 2010  Kshitij Gaipal (kgaipal@gmail.com)
 *
 *  ll1_compiler is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ll1_compiler is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ll1_compiler.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include <defines.h>

/* symbol table indices are same as token types for reserved keywords */
#define SYMTBL_TKNTYP_PROGRAM	0
#define SYMTBL_TKNTYP_IN	1
#define SYMTBL_TKNTYP_BEGIN	2
#define SYMTBL_TKNTYP_END	3
#define SYMTBL_TKNTYP_ARRAY	4
#define SYMTBL_TKNTYP_INTEGER	5
#define SYMTBL_TKNTYP_NOT	6
#define SYMTBL_TKNTYP_AND	7
#define SYMTBL_TKNTYP_OR	8
#define SYMTBL_TKNTYP_DO	9
#define SYMTBL_TKNTYP_ESAC	10
#define SYMTBL_TKNTYP_READ	11
#define SYMTBL_TKNTYP_WRITE	12
#define SYMTBL_TKNTYP_READLN	13
#define SYMTBL_TKNTYP_WRITELN	14
#define SYMTBL_TKNTYP_CASE	15
#define SYMTBL_TKNTYP_WHILE	16
#define SYMTBL_TKNTYP_OD	17
#define SYMTBL_TKNTYP_IF	18
#define SYMTBL_TKNTYP_FI	19
#define SYMTBL_TKNTYP_THEN	20
#define SYMTBL_TKNTYP_ELSE	21
#define SYMTBL_TKNTYP_FOREACH	22
#define SYMTBL_TKNTYP_COLON	23
#define SYMTBL_TKNTYP_SEMICOLON	24
#define SYMTBL_TKNTYP_COMMA	25
#define SYMTBL_TKNTYP_OPNSQBRKT	26
#define SYMTBL_TKNTYP_CLSQBRKT	27
#define SYMTBL_TKNTYP_ASSIGN	28
#define SYMTBL_TKNTYP_OPNPAREN	29
#define SYMTBL_TKNTYP_CLPAREN	30
#define SYMTBL_TKNTYP_EXP	31
#define SYMTBL_TKNTYP_DIV	32
#define SYMTBL_TKNTYP_MULT	33
#define SYMTBL_TKNTYP_ADD	34
#define SYMTBL_TKNTYP_SUB	35
#define SYMTBL_TKNTYP_LESSTHAN	36
#define SYMTBL_TKNTYP_GRTRTHAN	37
#define SYMTBL_TKNTYP_WITH	38
#define SYMTBL_TKNTYP_EQUAL	39

/* check against this to optimize for searching empty positions in symbol table;
   all reserved keywords have numerical value (token type) same as actual indices in symbol table */
#define SYMTBL_RSRVD_TKNS       (SYMTBL_TKNTYP_EQUAL+1) 

/* for quads, we need their for their operators */
#define QUADTBL_JMPFALSE        (SYMTBL_RSRVD_TKNS+0)
#define QUADTBL_JMP             (SYMTBL_RSRVD_TKNS+1)
#define QUADTBL_VARADDR         (SYMTBL_RSRVD_TKNS+2) /* operation (op) to get address of any variable x in t -> [op,x,_,t] */
#define QUADTBL_HLT             (SYMTBL_RSRVD_TKNS+3)
#define QUADTBL_MARKER          (SYMTBL_RSRVD_TKNS+4)

/* for these token types, indices in symbol table may not same as their numerical values (token type) */
#define SYMTBL_TKNTYP_ID	(QUADTBL_MARKER+1)
#define SYMTBL_TKNTYP_CONS	(QUADTBL_MARKER+2)

#define TOP_LEVEL_ID 1		/* for scope enumeration */

/* Symbol Table dimensions */
#define SYMTBL_ROWS             (SYMTBL_RSRVD_TKNS+1024) /* = temporary variables + tokens */
#define SYMTBL_INDX_NAME_SIZE   15

/* for identifying array indices */
struct dimension
{
  int size_subscript;		/* size of this array; sum of all indices; this is symbol table subscript */
  int dim_subscript;		/* numerical dimension; this is symbol table subscript */
  int dims;		        /* number of dimension this array will refer to; just for efficieny purposes in calculation(s) later */
  struct dimension *next;	/* next dimension value */
  struct dimension *prev;	/* previous dimension value */
  int subelements_subscript;	/* multiplcation of all subdimension, used for calculation of offset in array indices;
				   this is symbol table subscript */
};

/* for every scope at any level, use this structure */
struct scope
{
  struct scope *parent;
  struct scope *prev_sibling;	/* NULL for top level: only one declaration
				   statement at top level */

  int id;			/* complete identifier for this scope will
				   be computed by concatenating id's of all parents;
				   numerical value is incremented by 1 for siblings; 
				   minimum numerical value can be 0 */

  struct scope *next_sibling;	/* NULL for top level: only one declaration
				   statement at top level */

  struct scope *first_child;
};

struct item
{
  char name[SYMTBL_INDX_NAME_SIZE];  /* all data left justified;
					remaining padded with spaces;
					maximum size 15 allowed ! */

  int token_type;		/* one of the values defined above */

  struct scope *scope;		/* scope for 'with' statements,
				   this cannot be NULL for identifiers */

  struct dimension *dim;	/* to be used for array dimension; 
				   if 0 it is an integer */

  int mem_loc;			/* memory location */

  bool index_var_4_array;	/* if set then this is a variable pointing to an array index; 
				   used for the purpose of dereferencing only */

}typedef symitem;

symitem g_symtbl[SYMTBL_ROWS];

/* Global variables to be updated by lexical analyzer each time it is asked to read new token */
int g_token_subscript;	/* index in symbol table */
int g_token_type;       /* TT column value in symbol table pointed by subscript above */
boolean g_dec;		/* if set, we are in declaration section */

struct scope *g_scope;	/* Current scope where ssa/lex is reading tokens */
struct scope *g_toplevel_scope;	/* starting point of the linked list above */

int g_input_line;	/* line number in input file where lex/ssa excountered an error */

boolean g_dec_section;	/* if set, we are in declaratio nsection; add identifier to symbol table */

unsigned int g_errors; 	/* error count (=lex+ssa) */

#endif
