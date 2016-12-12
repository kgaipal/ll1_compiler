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

#include <quad.h>
#include <defines.h>
#include <string.h>
#include <symbol_table.h>
#include <stdio.h>
#include <stdlib.h>


int _tos = -1;			/* top of semantic action stack */
int _nq;			/* next quad */
int _sas[SAS_SIZE];      	/* semantic action stack */

int _tos_idstck = -1;		/* top of ID stack */
int _sas_idstck[SAS_SIZE_IDSTCK];/* ID stack */

int _temp_vars;			/* count of temporary variables */
boolean _initialized_quad = false;

void set_index_4_array(int quad);

int init_quad()
{
  if (_initialized_quad)
    return EOK;

  _nq = 0;
  _tos = -1;
  _tos_idstck = -1;
  _temp_vars = 0;
  _initialized_quad = true;

  return EOK;
}

int nq()
{
  int retval = -1;

  if (!_initialized_quad)
    if ((retval = init_quad()) < 0)
      return retval;

  return _nq;
}

/** Generates a temporary variable for quad table entry.
 *  Behind the scenes it actualy makes a new entry for a
 *  variable into the symbol table and returns its subscript.
 */
int gentemp()
{
  int pos = 0;
  char pattern[SYMTBL_INDX_NAME_SIZE] = {0};

  if (!_initialized_quad)
    if ((pos = init_quad()) < 0)
      return pos;

  sprintf(pattern, "$temp%d", _temp_vars++);
  for (pos = SYMTBL_INDX_NAME_SIZE-1; pos >= 0 ; pos--)
    if (pattern[pos] == 0)
      pattern[pos] = ' ';

  /* For sane operations on symbol table we add temporary variables bottom-up!

     If we add a temporary variable at a new position after identifier/constant/
     reserved keyword, we need to guard our temporary variable against any match 
     (in string comparison) with the token read from input source file.

     One way is to start with non-alphanumeric symbol, other way is to add at the
     bottom of the symbol table and move upward for every new temporary variable
     (bottom-up).

     When the latter approach is used, we can avoid unnecessary string comparison 
     when new token is to be added into symbol table since for new token, comparison
     starts always top and move towards bottom (top-bottom) until we find a match,
     Thus, string comparison occurs only against tokens read from input source file
     not against our temporary variables.
  */

  for (pos = SYMTBL_ROWS-1; pos >= SYMTBL_RSRVD_TKNS; pos--)
    {
      /* Make sure we do not overwrite neither tokens read 
	 from input source file, nor our temporary variables */
      if (g_symtbl[pos].name[0] == 0)
	{
	  memcpy(g_symtbl[pos].name, pattern, SYMTBL_INDX_NAME_SIZE);
	  g_symtbl[pos].token_type = SYMTBL_TKNTYP_ID;
	  g_symtbl[pos].scope = g_scope;

	  return pos;
	}
    }

  printf("\nSymbol table exhausted, no more room to add a new temporary variable to it, aborting.");
  exit(ESYMTBL_OVRFL); /* FATAL: symbol table exhausted, bad judgment for table size ! Cannot add temporary variable, aborting.*/
}

/** Makes the entry into the symbol table and returns the quad number 
 *  where update operation was performed
 */
int genquad(int op, int op1, int op2, int res)
{
  int retval = -1;

  if (!_initialized_quad)
    if ((retval = init_quad()) < 0)
      return retval;

  if (_nq >= QUADTBL_SIZE)
    {
      printf("\nquad table exhausted, cannot save new quads anymore, aborting !");
      exit(EQUADTBL_OVRFL); /* fatal: quad table exhausted, bad judgment for table size ! */
    }

  _quad_table[_nq].op = op;
  _quad_table[_nq].op1 = op1;
  _quad_table[_nq].op2 = op2;
  _quad_table[_nq].res = res;

  set_index_4_array(_nq);

  _nq = _nq + 1;

  return (_nq-1);
}

void push(int quad)
{
  if (!_initialized_quad)
    if (init_quad() < 0)
      return;

  if (_tos >= SAS_SIZE)
    {
      printf("SAS exhausted, cannot push variables anymore, aborting!\n");
      exit(ESAS_OVRFL); /* fatal: quad table exhausted, bad judgment for table size ! */
    }

  _sas[++_tos] = quad;
}

int pop()
{
  int retval = -1;
  int x;
  if (!_initialized_quad)
    if ((retval = init_quad()) < 0)
      return retval;

  if (empty())
    {
      printf("FATAL: SAS is tried over-popped ! aborting.\n");
      exit (ESAS_NOTHING_TO_POP);
    }
  x = _sas[_tos];
  _sas[_tos] = 0;
  _tos--;
  return x;
}

bool empty()
{
  if (!_initialized_quad)
    if (init_quad() < 0)
      return false;

  if (_tos < 0)
    return true;

  return false;
}

void destroy_quad()
{
  int i = 0;

  _nq = 0;
  _tos = -1;
  _initialized_quad = false;

  for (i = 0; i < QUADTBL_SIZE; i++)
    memset((void*)&_quad_table[i], 0, sizeof(struct quad));
}

void assignquad(int symtbl_subscript, int new_res_val)
{
  if (!_initialized_quad)
    if (init_quad() < 0)
      return;

  _quad_table[symtbl_subscript].res = new_res_val;
}

int write_quad_table()
{
  int retval = EOK;
  FILE * fp = 0;
  char * dref1 = 0;
  char * dref2 = 0;
  char * dref3 = 0;
  int i = 0;

  if (!_initialized_quad)
    if ((retval = init_quad()) < 0)
      return retval;

  fp = fopen(QUADTBL_FILE, "w");

  if (!fp)
    {
      printf("[ERROR] QUAD: \"%s\" : No such file or directory\n", QUADTBL_FILE);
      return EFILE_IO;
    }

  fprintf(fp, "   |%15s|%15s|%15s|%8s", "operator","operand 1", "operand 2", "result");
  fprintf(fp, "\n------------------------------------------------------------------");

  for (i = 0; i < QUADTBL_SIZE; i++)
    {
      dref1 = (_quad_table[i].op1_dref?"@":"");
      dref2 = (_quad_table[i].op2_dref?"@":"");
      dref3 = (_quad_table[i].res_dref?"@":"");

      if (_quad_table[i].op == 0)
	break;

      if (_quad_table[i].op == QUADTBL_JMPFALSE)
	fprintf(fp, "\n %02d|%s|%s%s|%s|%02d", i,
	       g_symtbl[_quad_table[i].op].name,
	       dref1,g_symtbl[_quad_table[i].op1].name,
	       "       -       ", _quad_table[i].res);

      else if (_quad_table[i].op == QUADTBL_JMP)
	fprintf(fp,"\n %02d|%s|%s|%s|%02d", i,
	       g_symtbl[_quad_table[i].op].name,
	       "       -       ","       -       ",
	       _quad_table[i].res);

      else if (_quad_table[i].op == QUADTBL_HLT)
	fprintf(fp,"\n %02d|%-15s|%s|%s|%s", i,
	       "HLT","       -       ",
	       "       -       ","       -       ");

      else if (_quad_table[i].op == QUADTBL_VARADDR)
	fprintf(fp,"\n %02d|%s|%s%s|%s|%s%s", i,
	       g_symtbl[_quad_table[i].op].name,
	       dref1,g_symtbl[_quad_table[i].op1].name ,
	       "       -       ",
	       dref3,g_symtbl[_quad_table[i].res].name);

      else if (_quad_table[i].op == SYMTBL_TKNTYP_READ)
	fprintf(fp,"\n %02d|%s|%s|%s|%s%s", i,
	       g_symtbl[_quad_table[i].op].name,
	       "       -       ","       -       ",
	       dref3,g_symtbl[_quad_table[i].res].name);

      else if (_quad_table[i].op == SYMTBL_TKNTYP_READLN)
	fprintf(fp,"\n %02d|%s|%s|%s|%s%s", i,
	       g_symtbl[_quad_table[i].op].name,
	       "       -       ","       -       ",
	       dref3,g_symtbl[_quad_table[i].res].name);

      else if (_quad_table[i].op == SYMTBL_TKNTYP_WRITE)
	fprintf(fp,"\n %02d|%s|%s|%s|%s%s", i,
	       g_symtbl[_quad_table[i].op].name,
	       "       -       ","       -       ",
	       dref3,g_symtbl[_quad_table[i].res].name);

      else if (_quad_table[i].op == SYMTBL_TKNTYP_WRITELN)
	fprintf(fp,"\n %02d|%s|%s|%s|%s%s", i,
	       g_symtbl[_quad_table[i].op].name,
	       "       -       ","       -       ",
	       dref3,g_symtbl[_quad_table[i].res].name);

      else if (_quad_table[i].op == SYMTBL_TKNTYP_EXP)
	fprintf(fp,"\n %02d|%s|%s%s|%s%s|%s%s", i,
	       g_symtbl[_quad_table[i].op].name,
	       dref1,g_symtbl[_quad_table[i].op1].name,
	       dref2,g_symtbl[_quad_table[i].op2].name,
	       dref3,g_symtbl[_quad_table[i].res].name);

      else if (_quad_table[i].op == SYMTBL_TKNTYP_NOT)
	fprintf(fp,"\n %02d|%s|%s%s|%s|%s%s", i,
	       g_symtbl[_quad_table[i].op].name,
	       dref1,g_symtbl[_quad_table[i].op1].name,
	       "       -       ",
	       dref3,g_symtbl[_quad_table[i].res].name);

      else if (_quad_table[i].op == SYMTBL_TKNTYP_ASSIGN)
      	fprintf(fp,"\n %02d|%s|%s%s|%s|%s%s", i,
      	       g_symtbl[_quad_table[i].op].name,
      	       dref1,g_symtbl[_quad_table[i].op1].name,
      	       "       -       ",
      	       dref3,g_symtbl[_quad_table[i].res].name);
      else 
	fprintf(fp,"\n %02d|%s|%s%s|%s%s|%s%s",i,
	       g_symtbl[_quad_table[i].op].name,
	       dref1,g_symtbl[_quad_table[i].op1].name,
	       dref2,g_symtbl[_quad_table[i].op2].name,
	       dref3,g_symtbl[_quad_table[i].res].name);
    }

  fclose(fp);

  return EOK;
}

void push_idstck(int id)
{
  int retval = -1;

  if (!_initialized_quad)
    if ((retval = init_quad()) < 0)
      return;

  if (_tos_idstck >= SAS_SIZE_IDSTCK)
    {
      printf("ID Stack exhausted, cannot push variables anymore, aborting!\n");
      exit(ESAS_OVRFL_IDSTCK); /* fatal: id stack exhausted, bad judgment for stack size ! */
    }

  _sas_idstck[++_tos_idstck] = id;
}

int pop_idstck()
{
  int retval = -1;
  int x;
  if (!_initialized_quad)
    if ((retval = init_quad()) < 0)
      return retval;

  if (empty_idstck())
    {
      printf("FATAL: ID Stack is tried over-popped ! aborting.\n");
      exit (ESAS_NOTHING_TO_POP_IDSTCK);
    }
  x = _sas_idstck[_tos_idstck];
  _sas_idstck[_tos_idstck] = 0;
  _tos_idstck--;
  return x;
}

bool empty_idstck()
{
  if (!_initialized_quad)
    if (init_quad() < 0)
      return false;

  if (_tos_idstck < 0)
    return true;

  return false;
}

void assign_pc(int quad, int pc)
{
  _quad_table[quad].pc = pc;
}

void set_index_4_array(int quad)
{
  int op = _quad_table[quad].op;
  int op1 = _quad_table[quad].op1;
  int op2 = _quad_table[quad].op2;
  int res = _quad_table[quad].res;

  if (op == SYMTBL_TKNTYP_ASSIGN)
    {
      if (!g_symtbl[op1].index_var_4_array && g_symtbl[res].index_var_4_array)
  	_quad_table[quad].res_dref = true;

      if (g_symtbl[op1].index_var_4_array && !g_symtbl[res].index_var_4_array)
  	_quad_table[quad].op1_dref = true;

      if (g_symtbl[op1].index_var_4_array && g_symtbl[res].index_var_4_array)
  	_quad_table[quad].op1_dref = _quad_table[quad].res_dref = true;
    }
  else if (op == SYMTBL_TKNTYP_ADD || op == SYMTBL_TKNTYP_SUB
	   || op == SYMTBL_TKNTYP_DIV || op == SYMTBL_TKNTYP_MULT) 
    {
      if (g_symtbl[op1].index_var_4_array && !g_symtbl[op2].index_var_4_array && !g_symtbl[res].index_var_4_array)
  	_quad_table[quad].op1_dref = true;

      if (!g_symtbl[op1].index_var_4_array && g_symtbl[op2].index_var_4_array && !g_symtbl[res].index_var_4_array)
  	_quad_table[quad].op2_dref = true;

      if (g_symtbl[op1].index_var_4_array && g_symtbl[op2].index_var_4_array && !g_symtbl[res].index_var_4_array)
  	_quad_table[quad].op1_dref = _quad_table[quad].op2_dref = true;
    }
  else if (op == SYMTBL_TKNTYP_LESSTHAN || op == SYMTBL_TKNTYP_GRTRTHAN || op == SYMTBL_TKNTYP_EQUAL)
    {
      if (g_symtbl[op1].index_var_4_array && !g_symtbl[op2].index_var_4_array)
  	_quad_table[quad].op1_dref = true;

      if (!g_symtbl[op1].index_var_4_array && g_symtbl[op2].index_var_4_array)
  	_quad_table[quad].op2_dref = true;

      if (g_symtbl[op1].index_var_4_array && g_symtbl[op2].index_var_4_array)
  	_quad_table[quad].op1_dref = _quad_table[quad].op2_dref = true;
    }
  else if (op == SYMTBL_TKNTYP_WRITE || op == SYMTBL_TKNTYP_WRITELN
	   || op == SYMTBL_TKNTYP_READ || op == SYMTBL_TKNTYP_READLN)
    {
      if (g_symtbl[res].index_var_4_array)
  	_quad_table[quad].res_dref = true;
    }
  else if (op == SYMTBL_TKNTYP_EXP)
    {
      if (g_symtbl[op1].index_var_4_array)
  	_quad_table[quad].op1_dref = true;

      if (g_symtbl[op2].index_var_4_array)
  	_quad_table[quad].op2_dref = true;
    }
}

struct quad get_quad_fields_val(int quad)
{
  return _quad_table[quad];
}

void set_quad_fields_val(int quad, struct quad data)
{
  _quad_table[quad].op = data.op;
  _quad_table[quad].op1 = data.op1;
  _quad_table[quad].op2 = data.op2;
  _quad_table[quad].res = data.res;
  _quad_table[quad].pc = data.pc;
  _quad_table[quad].op1_dref = data.op1_dref;
  _quad_table[quad].op2_dref = data.op2_dref;
  _quad_table[quad].res_dref = data.res_dref;
}
