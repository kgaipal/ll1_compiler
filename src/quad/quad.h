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

#ifndef _QUAD_H_
#define _QUAD_H_

#include <defines.h>

#define SAS_SIZE 1024		/* semantic action stack (sas) size */
#define SAS_SIZE_IDSTCK 1024	/* ID stack (idstck) size */
#define QUADTBL_SIZE 1024	/* quad table size */

struct quad
{
  int op;			/* operator (subscript in symbol table) */
  int op1;			/* operand 1 */
  int op2;			/* operand 2 */
  int res;			/* result of op on op1 and op2 */
  int pc;			/* program counter in the output binary (CODEGEN updates this field) */

/* if set for a temporary variable indexing an array, operation involving it will be
   instead on the memory content it is pointing to */
  bool op1_dref;
  bool op2_dref;
  bool res_dref;
};

struct quad _quad_table[QUADTBL_SIZE];/* quad table */

int nq();			/* returns next_quad */
int gentemp();			/* generate temporary variable */
int genquad(int op, int op1, int op2, int res); /* generates a new quad and make the entry into quad table; return  */
void push(int quad);		/* push onto sas */
int pop();			/* pop from sas */
boolean empty();		/* checks whether SAS is empty */

int init_quad();
void destroy_quad();

int write_quad_table();	/* prints quad table on screen */

void assignquad(int symtbl_subscript, int new_res_val);

void push_idstck(int id);	/* push onto idstack */
int pop_idstck();		/* pop from idstack */
bool empty_idstck();	        /* checks whether idstack is emty */

void assign_pc(int quad, int pc);/* CODEGEN assigns program counter for each quad  */
struct quad get_quad_fields_val(int quad);
void set_quad_fields_val(int quad, struct quad data);

#endif 				/* _QUAD_H_ */
