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

#include <ssa.h>
#include <defines.h>
#include <symbol_table.h>
#include <util.h>
#include <lexical_analyzer.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <quad.h>
#include <unistd.h>


#define ID_TYPE_STACK_SIZE 1024

FILE *_syntax_err_file_fp;
boolean _ssa_initialized = false;
boolean _print_error = true;
int _tos_id_type_stck;
int _tos_array_indices;
int _id_type_stck[ID_TYPE_STACK_SIZE]; /* always accessed when reading
					   tokens in declaration section */
int _array_indices[ID_TYPE_STACK_SIZE]; /* all indices of an array variable*/

void push_id_type_stck(int id);
int pop_id_type_stck();
bool empty_id_type_stck();
void push_array_indices(int index);
int pop_array_indices();
bool empty_array_indices();
void err_rcrsv_dscnt(const char* expected, int found);
struct dimension *add_2_dim_array(struct dimension *head, int dim);
void compute_array_index_var_increment(int token_subscript, int pointer_to_array_subscript);
int add_number_2_symtbl(int y);

/* grammar recurive descent functions */
boolean P();

boolean D();
boolean D1();
boolean IL();
boolean IL1();
boolean CL();
boolean CL1();

boolean ID();
boolean ID1();
boolean EL();
boolean EL1();
boolean IDL();
boolean IDL1();

boolean E();
boolean E1();
boolean T();
boolean T1();
boolean F();

boolean C();
boolean C1();
boolean X();
boolean X1();
boolean Y();
boolean Y1();

boolean S();
boolean S1();
boolean S2();

boolean M();


/** init_ssa: Initializes the syntax analyzer.
    returns 0 on success, otherwise negative integer.
*/
int init_ssa(const char *src_file)
{
  int retval = EOK;

  /* dont overdo stuff */
  if (_ssa_initialized)
    return EOK;

  if ((retval = init_lex(src_file)) < 0)
    return retval;

  if ((retval = init_quad()) < 0)
    return retval;

  unlink(SYNTX_ERR_FILE);
  _syntax_err_file_fp = fopen(SYNTX_ERR_FILE, "a");

  if (!_syntax_err_file_fp)
    {
      printf("[ERROR] SSA: \"%s\" : No such file or directory\n", SYNTX_ERR_FILE);
      return EFILE_IO;
    }

  _ssa_initialized = true;
  g_dec_section = false;
  _tos_id_type_stck = -1;
  _tos_array_indices = -1;
  _print_error = true;
  return EOK;
}

void destroy_ssa()
{
  if (_syntax_err_file_fp)
    fclose(_syntax_err_file_fp);

  _ssa_initialized = false;
  g_dec_section = false;
  _syntax_err_file_fp = 0;
}

/*       Rule           |    Selection Set
 * --------------------------------------
 * P -> program D S end |
 */
boolean P()
{
#ifdef _DEBUG
  printf("P()\n");
#endif
  next_token();			/* read the very first token */

  if (g_token_type == SYMTBL_TKNTYP_PROGRAM)
    {
      g_dec_section = true;
      next_token();

      if (D())
	{
	  if (S())
	    if (g_token_type == SYMTBL_TKNTYP_END)
	      {
		/* current scope ends, switch to parent */
		genquad(QUADTBL_HLT, -1, -1, -1);
		g_scope = g_scope->parent;
		return true;
	      }
	    else
	      {
		err_rcrsv_dscnt("end", g_token_type);
		return false;
	      }
	  else
	    return false;
	}
      else
	return false;
    }
  else
    {
      err_rcrsv_dscnt("program", g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 * D -> begin        |
 *   -> IL D'        |
 */
boolean D()
{
  if (g_token_type == SYMTBL_TKNTYP_BEGIN)
    {
      g_dec_section = false;

      next_token();
      return true;
    }
  else if (IL())
    {
      if (D1())
	return true;
      else
	return false;
    }
  else
    {
      err_rcrsv_dscnt("begin", g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 * D' -> array[CL D  |
 *    -> integer D   |
 */
boolean D1()
{
#ifdef _DEBUG
  printf("D1()\n");
#endif
  int x,i=0, j=0;
  struct dimension *arr_dim = 0;
  char *tol= 0;

  if (g_token_type == SYMTBL_TKNTYP_ARRAY)
    {
      next_token();
      if (g_token_type == SYMTBL_TKNTYP_OPNSQBRKT)
	{
	  next_token();
	  if (CL())
	    {
	      if (g_dec_section)
		{
		  /* find out the dimension of this series of identifiers in
		     id_type stack and update their entries in symbol table */

		  /* last few pushes were array(s) dimensions */
		  x = pop_id_type_stck(); /* for an array: a[3,4,...,6]; 6 is popped */
		  j = 1;		  /* size of the array */
		  bool warned = false;
		  while (g_symtbl[x].token_type == SYMTBL_TKNTYP_CONS)
		    {
		      i++;
		      j *= strtol(g_symtbl[x].name, &tol, 0);
		      arr_dim = add_2_dim_array(arr_dim, x);
		      x = pop_id_type_stck();

		      if (g_symtbl[x].token_type == SYMTBL_TKNTYP_CONS && strtol(g_symtbl[x].name,0,0) == 0 && !warned)
		      	{
		      	  warned = true;
		      	  printf("[WARNING] SSA: line(%d): array dimension(s) are ZERO! \n", g_input_line);
		      	}
		    }

		  /* All integer indices of array finished, now the identifiers of type array starts
		     (last one is already popped in 'x'), update their entries in symbol table */

		  g_symtbl[x].dim = arr_dim;
		  g_symtbl[x].dim->dims = i;

		  j = g_symtbl[x].dim->size_subscript = add_number_2_symtbl(j);

		  while (!empty_id_type_stck())
		    {
		      x = pop_id_type_stck();
		      g_symtbl[x].dim = arr_dim;
		      g_symtbl[x].dim->dims = i;
		      g_symtbl[x].dim->size_subscript = j;
		    }
		}
	      if (D())
		return true;
	      else
		return false;
	    }
	  else
	    return false;
	}
      else
	{
	  err_rcrsv_dscnt("[", g_token_type);
	  return false;
	}
    }
  else if (g_token_type == SYMTBL_TKNTYP_INTEGER)
    {
      if (g_dec_section)
	{
	  /* there is no dimension associated with the integer type 
	     identifiers just empty stack for future use */
	  while (!empty_id_type_stck())
	    {
	      x = pop_id_type_stck();
	      g_symtbl[x].dim = 0;
	    }
	}

      next_token();
      if (D())
	return true;
      else
	return false;
    }
  else
    {
      err_rcrsv_dscnt("array integer", g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 * IL -> id IL'      |
 */
boolean IL()
{
#ifdef _DEBUG
  printf("IL()\n");
#endif
  if (g_token_type == SYMTBL_TKNTYP_ID
      || g_token_subscript == ELEX_UNDCLRD_VAR
      || g_token_subscript == ELEX_ILLEGAL_SCOPE_ACCESS
      || g_token_subscript == ELEX_RE_DCLRD_VAR)
    {
      switch (g_token_subscript)
	{
	case ELEX_UNDCLRD_VAR:
	case ELEX_ILLEGAL_SCOPE_ACCESS:
	case ELEX_RE_DCLRD_VAR:
	  err_rcrsv_dscnt("id", g_token_type);
	  break;
	}

      if (!g_dec_section)	
	push(g_token_subscript); /* when in declaration section, 
				   we need not evaluate anything */

      /* we need to determine later this is integer or array type identfiers */
      if (g_dec_section)
	push_id_type_stck(g_token_subscript);

      next_token();
      if (IL1())
	return true;
      else
	return false;
    }
  else
    {
      err_rcrsv_dscnt("id", g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 * IL' -> , IL       |
 *     -> :          |
 */
boolean IL1()
{
#ifdef _DEBUG
  printf("IL1()\n");
#endif
  if (g_token_type == SYMTBL_TKNTYP_COMMA)
    {
      next_token();
      if (IL())
	return true;
      else
	return false;
    }
  else
    {
      if (g_token_type == SYMTBL_TKNTYP_COLON)
	{
	  next_token();
	  return true;
	}
      else
	{
	  err_rcrsv_dscnt(", :", g_token_type);
	  return false;
	}
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 * CL -> cons CL'    |
 */
boolean CL()
{
#ifdef _DEBUG
  printf("CL()\n");
#endif
  if (g_token_type != SYMTBL_TKNTYP_CONS)
    {
      err_rcrsv_dscnt("cons", ESSA_ARR_IMPRPR_SUBSCRIPT);
      g_token_subscript = find_in_symtbl("1",1);
      g_token_type = SYMTBL_TKNTYP_CONS;
    }

  /* we need to find out the dimension of the array */
  if (g_dec_section)
    push_id_type_stck(g_token_subscript);

  next_token();
  if (CL1())
    return true;

  return false;
}

/*       Rule        |    Selection Set
 * --------------------------------------
 * CL' -> , CL       | 
 *     -> ]          | 
 */
boolean CL1()
{
#ifdef _DEBUG
  printf("CL1()\n");
#endif
  if (g_token_type == SYMTBL_TKNTYP_COMMA)
    {
      next_token();
      if (CL())
	return true;
      else
	return false;
    }
  else
    {
      if (g_token_type == SYMTBL_TKNTYP_CLSQBRKT)
	{
	  next_token();
	  return true;
	}
      else
	{
	  err_rcrsv_dscnt(", ]", g_token_type);
	  return false;
	}
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 * ID -> id ID'      |
 */
boolean ID()
{
#ifdef _DEBUG
  printf("ID()\n");
#endif
  if (g_token_type == SYMTBL_TKNTYP_ID
      || g_token_subscript == ELEX_UNDCLRD_VAR
      || g_token_subscript == ELEX_ILLEGAL_SCOPE_ACCESS
      || g_token_subscript == ELEX_RE_DCLRD_VAR)
    {
      switch (g_token_subscript)
	{
	case ELEX_UNDCLRD_VAR:
	case ELEX_ILLEGAL_SCOPE_ACCESS:
	case ELEX_RE_DCLRD_VAR:
	  err_rcrsv_dscnt("id", g_token_type);
	  break;
	}

      push(g_token_subscript);
      push_id_type_stck(g_token_subscript);
      next_token();
      if (ID1())
	return true;
      else
	return false;
    }
  else
    {
      if (_print_error)
      err_rcrsv_dscnt("id", g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 * ID' -> [EL        |
 *     -> empty      | {:=+-* '/'  ),<=>] and or ; end fi od esac do then else}
 */
boolean ID1()
{
#ifdef _DEBUG
  printf("ID1()\n");
#endif
  int x,y,t,i=-1;

  /* make sure if the last identifier is multidimensional, then current token must be '[' */
  x = pop_id_type_stck();
  push_id_type_stck(x);

  if ((g_symtbl[x].dim && g_symtbl[x].dim->dims != 0) 
      && (g_token_type != SYMTBL_TKNTYP_OPNSQBRKT))
    {
      y = g_token_subscript;
      g_token_subscript = x;
      err_rcrsv_dscnt(0, ESSA_ARR_IMPRPR_SUBSCRIPT);
      g_token_subscript = y;
      return false;
    }

  if (g_token_type == SYMTBL_TKNTYP_OPNSQBRKT)
    {
      push_id_type_stck(QUADTBL_MARKER); /* to avoid popping off beyond indices */
      next_token();
      if (EL())
	{
	  /* you are done reading all the indices, pop them off and update the quad table */
	  /* note: for an array: a[3,4,...] 3 is at the bottom of the stack */
	  x = pop_id_type_stck();
	  i = 1;
	  while(x != QUADTBL_MARKER)
	    {
	      push_array_indices(x); /* this will make 3 at the top of the stack!;
					remember the order for computation later :) */
	      i++;
	      x = pop_id_type_stck();
	    }

	  i--;			  /* QUADTBL_MARKER was also counted*/
	  x = pop_id_type_stck(); /* this is our array identifier; we dont need this any further in index calculation */

	  if (x < 0)
	    {
	      /* this can be some error condition only because now we are also doing 
		 recovery phase now and pushed error condition instead of symbol table
		 index of this array identifier;
		 avoiding any further computation is the best option */

	      push(x);		/* dummy push */
	      return true;
	    }

	  if (i != g_symtbl[x].dim->dims)
	    {
	      y = g_token_subscript;
	      g_token_subscript = x;
	      err_rcrsv_dscnt(0, ESSA_ARR_IMPRPR_SUBSCRIPT);
	      g_token_subscript = y;
	      push(y);		/* dummy push */
	      return true;
	    }

	  /* now all fine, generate the quads */
	  x = pop();	                          /* array identifier */
	  t = gentemp();                          /* base address of array identifier */

	  genquad(QUADTBL_VARADDR, x, -1, t);
	  g_symtbl[t].index_var_4_array = true;

	  /* remember the order of indices on stack (see note above) */
	  compute_array_index_var_increment(x,t); /* generate address for exact element in array */

	  push(t);		/* push onto SAS the value at array[index, index,...] */

	  return true;
	}
      else
	return false;
    }
  else
    {
      if (g_token_type == SYMTBL_TKNTYP_ASSIGN || g_token_type == SYMTBL_TKNTYP_EQUAL
	  || g_token_type == SYMTBL_TKNTYP_ADD|| g_token_type == SYMTBL_TKNTYP_SUB
	  || g_token_type == SYMTBL_TKNTYP_MULT || g_token_type == SYMTBL_TKNTYP_DIV
	  || g_token_type == SYMTBL_TKNTYP_CLPAREN || g_token_type == SYMTBL_TKNTYP_CLSQBRKT
	  || g_token_type == SYMTBL_TKNTYP_COMMA || g_token_type == SYMTBL_TKNTYP_SEMICOLON
	  || g_token_type == SYMTBL_TKNTYP_LESSTHAN || g_token_type == SYMTBL_TKNTYP_GRTRTHAN
	  || g_token_type == SYMTBL_TKNTYP_AND|| g_token_type == SYMTBL_TKNTYP_OR
	  || g_token_type == SYMTBL_TKNTYP_END || g_token_type == SYMTBL_TKNTYP_FI
	  || g_token_type == SYMTBL_TKNTYP_OD || g_token_type == SYMTBL_TKNTYP_ESAC
	  || g_token_type == SYMTBL_TKNTYP_DO || g_token_type == SYMTBL_TKNTYP_THEN
	  || g_token_type == SYMTBL_TKNTYP_ELSE)
	{
	  pop_id_type_stck();	/* last push was not meant for array */
	  return true;
	}
      else
	{
	  err_rcrsv_dscnt(":=+-*/ ),<=>] and or ; end fi od esac do then else", g_token_type);
	  return false;
	}
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  EL -> E EL'      |
 */
boolean EL()
{
#ifdef _DEBUG
  printf("EL()\n");
#endif
  if (E())
    {
      push_id_type_stck(pop()); /* SAS contains the index to array, save it */
      if (EL1())
	return true;
      else
	return false;
    }
  else 
    return false; 
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  EL' -> ,EL       |
 *      -> ]         |
 */
boolean EL1()
{
#ifdef _DEBUG
  printf("EL1()\n");
#endif
  if (g_token_type == SYMTBL_TKNTYP_COMMA)
    {
      next_token();
      if (EL())
	return true;
      else
	return false;
    }
  else
    {
      if (g_token_type == SYMTBL_TKNTYP_CLSQBRKT)
	{
	  next_token();
	  return true;
	}
      else
	{
	  err_rcrsv_dscnt(", ]", g_token_type);
	  return false;
	}
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  IDL -> ID IDL'   |
 */
boolean IDL()
{
#ifdef _DEBUG
  printf("IDL()\n");
#endif
  if (ID())
    if (IDL1())
      return true;
    else
      return false;
  else 
    return false; 
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  IDL' -> ,IDL    |
 *       -> )        |
 */
boolean IDL1()
{
#ifdef _DEBUG
  printf("IDL1()\n");
#endif
  if (g_token_type == SYMTBL_TKNTYP_COMMA)
    {
      next_token();
      if (IDL())
	return true;
      else
	return false;
    }
  else
    {
      if (g_token_type == SYMTBL_TKNTYP_CLPAREN)
	{
	  next_token();
	  return true;
	}
      else
	{
	  err_rcrsv_dscnt(", )", g_token_type);
	  return false;
	}
    }
}


/*       Rule        |    Selection Set
 * --------------------------------------
 *  E -> TE'         |
 */
boolean E()
{
#ifdef _DEBUG
  printf("E()\n");
#endif
  if (T())
    if (E1())
      return true;
    else
      return false;
  else 
    return false; 
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  E' -> +TE'       |
 *     -> -TE'       |
 *     -> empty	     | {),<=>] and or ; end fi od esac do then else}
 */
boolean E1()
{
#ifdef _DEBUG
  printf("E1()\n");
#endif
  int x, y, t;

  if (g_token_type == SYMTBL_TKNTYP_ADD)
    {
      next_token();
      if (T())
	{
	  x = pop();		/* second operand */
	  y = pop();		/* first operand */
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_ADD, y, x, t);
	  push(t);

	  if (E1())
	    return true;
	  else
	    return false;
	}
      else
	return false;
    }
  else if (g_token_type == SYMTBL_TKNTYP_SUB)
    {
      next_token();
      if (T())
	{
	  x = pop();		/* second operand */
	  y = pop();		/* first operand */
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_SUB, y, x, t);
	  push(t);

	  if (E1())
	    return true;
	  else
	    return false;
	}
      else
	return false;
    }
  else
    {
      if (g_token_type == SYMTBL_TKNTYP_CLPAREN || g_token_type == SYMTBL_TKNTYP_COMMA
	  || g_token_type == SYMTBL_TKNTYP_LESSTHAN || g_token_type == SYMTBL_TKNTYP_EQUAL 
	  || g_token_type == SYMTBL_TKNTYP_GRTRTHAN || g_token_type == SYMTBL_TKNTYP_CLSQBRKT
	  || g_token_type == SYMTBL_TKNTYP_AND|| g_token_type == SYMTBL_TKNTYP_OR
	  || g_token_type == SYMTBL_TKNTYP_SEMICOLON
	  || g_token_type == SYMTBL_TKNTYP_END || g_token_type == SYMTBL_TKNTYP_FI
	  || g_token_type == SYMTBL_TKNTYP_OD || g_token_type == SYMTBL_TKNTYP_ESAC
	  || g_token_type == SYMTBL_TKNTYP_DO || g_token_type == SYMTBL_TKNTYP_THEN
	  || g_token_type == SYMTBL_TKNTYP_ELSE|| g_token_type == SYMTBL_TKNTYP_COLON)
	  return true;
      else
	{
	  err_rcrsv_dscnt("+ - ),<=>] and or ; end fi od esac do then else", g_token_type);
	  return false;
	}
    }

}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  T -> FT'         |
 */
boolean T()
{
#ifdef _DEBUG
  printf("T()\n");
#endif
  if (F())
    if (T1())
      return true;
    else
      return false;
  else 
    return false; 
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  T1 -> *FT'       |
 *     -> /FT'       |
 *     -> empty      | {+-),<=>] and or ; end fi od esac do then else}
 */
boolean T1()
{
#ifdef _DEBUG
  printf("T1()\n");
#endif
  int x, y, t;

  if (g_token_type == SYMTBL_TKNTYP_MULT)
    {
      next_token();
      if (F())
	{
	  x = pop();		/* second operand */
	  y = pop();		/* first operand */
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_MULT, y, x, t);
	  push(t);

	  if (T1())
	    return true;
	  else
	    return false;
	}
      else
	return false;
    }
  else if (g_token_type == SYMTBL_TKNTYP_DIV)
    {
      next_token();
      if (F())
	{
	  x = pop();		/* second operand */
	  y = pop();		/* first operand */
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_DIV, y, x, t);
	  push(t);

	  if (T1())
	    return true;
	  else
	    return false;
	}
      else
	return false;
    }
  else
    {
      if (g_token_type == SYMTBL_TKNTYP_ADD || g_token_type == SYMTBL_TKNTYP_SUB
	  || g_token_type == SYMTBL_TKNTYP_CLPAREN ||g_token_type == SYMTBL_TKNTYP_COMMA
	  || g_token_type == SYMTBL_TKNTYP_LESSTHAN || g_token_type == SYMTBL_TKNTYP_EQUAL 
	  || g_token_type == SYMTBL_TKNTYP_GRTRTHAN || g_token_type == SYMTBL_TKNTYP_CLSQBRKT
	  || g_token_type == SYMTBL_TKNTYP_AND || g_token_type == SYMTBL_TKNTYP_OR
	  || g_token_type == SYMTBL_TKNTYP_SEMICOLON
	  || g_token_type == SYMTBL_TKNTYP_END || g_token_type == SYMTBL_TKNTYP_FI
	  || g_token_type == SYMTBL_TKNTYP_OD || g_token_type == SYMTBL_TKNTYP_ESAC
	  || g_token_type == SYMTBL_TKNTYP_DO || g_token_type == SYMTBL_TKNTYP_THEN
	  || g_token_type == SYMTBL_TKNTYP_ELSE || g_token_type == SYMTBL_TKNTYP_COLON)
	return true;
      else
	{
	  err_rcrsv_dscnt("/ * +-),<=>] and or ; end fi od esac do then else", g_token_type);
	  return false;
	}
    }

}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  F -> (E)         |
 *    -> cons        |
 *    -> exp(E,E)    |
 *    -> ID          |
 */
boolean F()
{
#ifdef _DEBUG
  printf("F()\n");
#endif
  int x, y, t;

  if (g_token_type == SYMTBL_TKNTYP_OPNPAREN)
    {
      next_token();
      if(E())
	{
	  if (g_token_type == SYMTBL_TKNTYP_CLPAREN)
	    {
	      next_token();
	      return true;
	    }
	  else
	    {
	      err_rcrsv_dscnt(")", g_token_type);
	      return false;
	    }
	}
      else
	{
	  err_rcrsv_dscnt("(", g_token_type);
	  return false;
	}
    }
  else if(g_token_type == SYMTBL_TKNTYP_CONS)
    {
      push(g_token_subscript);
      next_token();
      return true;
    }
  else if (g_token_type == SYMTBL_TKNTYP_EXP)
    {
      next_token();
      if (g_token_type == SYMTBL_TKNTYP_OPNPAREN)
	{
	  next_token();
	  if(E())
	    {
	      if (g_token_type == SYMTBL_TKNTYP_COMMA)
		{
		  next_token();
		  if(E())
		    {
		      if (g_token_type == SYMTBL_TKNTYP_CLPAREN)
			{
			  x = pop(); /* result of second argument: E */
			  y = pop(); /* result of first argument: E*/
			  t = gentemp();
			  genquad(SYMTBL_TKNTYP_EXP, y, x, t);
			  push(t);
			  next_token();
			  return true;
			}
		      else
			{
			  err_rcrsv_dscnt(")", g_token_type);
			  return false;
			}
		    }
		  else 
		    return false;
		}
	      else
		{
		  err_rcrsv_dscnt(",", g_token_type);
		  return false;
		}
	    }
	  else
	    return false;
	}
      else
	{
	  err_rcrsv_dscnt("(", g_token_type);
	  return false;
	}
    }
  else if (ID())
    return true;
  else
    {
      err_rcrsv_dscnt("cons exp (", g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  C -> XC'         |
 */
boolean C()
{
#ifdef _DEBUG
  printf("C()\n");
#endif
  if(X())
    if(C1())
      return true;
    else
      return false;
  else
    return false;
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  C' -> or XC'     |
 *     -> empty      |	{do then )]}
 */
boolean C1()
{
#ifdef _DEBUG
  printf("C1()\n");
#endif
  int x, y, t;

  if(g_token_type == SYMTBL_TKNTYP_OR)
    {
      next_token();
      if(X())
	{
	  x = pop();		/* second operand for OR */
	  y = pop();		/* first operand for OR */
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_OR, y, x, t);
	  push(t);

	  if(C1())
	    return true;
	  else
	    return false;
	}
      else
	return false;
    }
  else if(g_token_type == SYMTBL_TKNTYP_DO 
	  || g_token_type == SYMTBL_TKNTYP_THEN
	  || g_token_type == SYMTBL_TKNTYP_CLPAREN
	  || g_token_type == SYMTBL_TKNTYP_CLSQBRKT)
      return true;
  else
    {
      err_rcrsv_dscnt("or do then ) ]",g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  X -> YX'         | 
 */
boolean X()
{
#ifdef _DEBUG
  printf("X()\n");
#endif
  if(Y())
    if(X1())
      return true;
    else
      return false;
  else
    return false;
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  X' -> and YX'    |
 *     -> empty      | {or do then )}
 */
boolean X1()
{
#ifdef _DEBUG
  printf("X1()\n");
#endif
  int x, y,t;

  if(g_token_type == SYMTBL_TKNTYP_AND)
    {
      next_token();
      if(Y())
	{
	  x = pop();		/* second operand for AND */
	  y = pop();		/* first operand for AND */
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_AND, y, x, t);
	  push(t);

	  if(X1())
	    return true;
	  else
	    return false;
	}
      else
	return false;
    }
  else if(g_token_type == SYMTBL_TKNTYP_OR
	  || g_token_type == SYMTBL_TKNTYP_DO 
	  || g_token_type == SYMTBL_TKNTYP_THEN
	  || g_token_type == SYMTBL_TKNTYP_CLPAREN
	  || g_token_type == SYMTBL_TKNTYP_CLSQBRKT)
      return true;
  else
    {
      err_rcrsv_dscnt("and or do then ) ]",g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  Y -> [C]         |
 *    -> not(C)      | 
 *    -> EY'         |
 */
boolean Y()
{
#ifdef _DEBUG
  printf("Y()\n");
#endif
  int x, t;

  if(g_token_type == SYMTBL_TKNTYP_OPNSQBRKT)
    {
      next_token();
      if(C())
	if(g_token_type == SYMTBL_TKNTYP_CLSQBRKT)
	  {
	    next_token();
	    return true;
	  }
	else
	  {
	    err_rcrsv_dscnt("]",g_token_type);
	    return false;
	  }
      else
	return false;
    }
  else if(g_token_type == SYMTBL_TKNTYP_NOT)
    {
      next_token();
      if(g_token_type == SYMTBL_TKNTYP_OPNPAREN)
	{
	  next_token();
	  if(C())
	    if(g_token_type == SYMTBL_TKNTYP_CLPAREN)
	      {
		x = pop();		/* operand for NOT, result of C*/
		t = gentemp();
		genquad(SYMTBL_TKNTYP_NOT, x, -1, t);
		push(t);	/* push the result of not(C) */

		next_token();
		return true;
	      }
	    else
	      {
		err_rcrsv_dscnt(")",g_token_type);
		return false;
	      }
	  else
	    return false;
	}
      else
	{
	  err_rcrsv_dscnt("(",g_token_type);
	  return false;
	}
    }
  else if (E())
    {
      if(Y1())
	return true;
      else
	return false;
    }
  else
    {
      err_rcrsv_dscnt("not [",g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  Y' -> <E         |
 *     -> >E         |
 *     -> =E         |
 */
boolean Y1()
{
#ifdef _DEBUG
  printf("Y1()\n");
#endif
  int x,y,t;

  if(g_token_type == SYMTBL_TKNTYP_LESSTHAN)
    {
      next_token();
      if(E())
	{
	  x = pop();
	  y = pop();
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_LESSTHAN, y, x, t);
	  push(t);

	  return true;
	}
      else
	return false;
    }
  else if(g_token_type == SYMTBL_TKNTYP_GRTRTHAN)
    {
      next_token();
      if(E())
	{
	  x = pop();
	  y = pop();
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_GRTRTHAN, y, x, t);
	  push(t);

	  return true;
	}
      else
	return false;
    }
  else if(g_token_type == SYMTBL_TKNTYP_EQUAL)
    {
      next_token();
      if(E())
	{
	  x = pop();
	  y = pop();
	  t = gentemp();
	  genquad(SYMTBL_TKNTYP_EQUAL, y, x, t);
	  push(t);

	  return true;
	}
      else
	return false;
    }
  else
    {
      err_rcrsv_dscnt("> < =",g_token_type);
      return false;
    }
}

/*       Rule                 |    Selection Set
 * -----------------------------------------------
 * S -> read(IDL S1           |
 *   -> write(IDL S1          |
 *   -> readln(IDL S1         |
 *   -> writeln(IDL S1        |
 *   -> case C do S  M S1     |
 *   -> while C do S od S1    |
 *   -> if C then S S2        | 
 *   -> foreach id in id do S od S1 |
 *   -> with D S end S1       |
 *   -> ID := E S1            |
 */
boolean S()
{
#ifdef _DEBUG
  printf("S()\n");
#endif
  int x, y, t;

  if (g_token_type == SYMTBL_TKNTYP_READ)
    {
      next_token();
      if (g_token_type == SYMTBL_TKNTYP_OPNPAREN)
	{
	  push(QUADTBL_MARKER);	/* push marker to avoid over-popping */
	  next_token();

	  if(IDL())
	    {
	      x = pop();
	      while (x != QUADTBL_MARKER)
		{
		  push_idstck(x);
		  x = pop();
		}

	      while (!empty_idstck())
		  genquad(SYMTBL_TKNTYP_READ, -1, -1, pop_idstck());

	      if(S1())
		return true;
	      else
		return false;
	    }
	  else
	    return false;
	}
      else
	{
	  err_rcrsv_dscnt("(",g_token_type);
	  return false;
	}
    }
  else if (g_token_type == SYMTBL_TKNTYP_WRITE)
    {
      next_token();
      if (g_token_type == SYMTBL_TKNTYP_OPNPAREN)
	{
	  push(QUADTBL_MARKER);	/* push marker to avoid over-popping */
	  next_token();

	  if(IDL())
	    {
	      x = pop();
	      while (x != QUADTBL_MARKER)
		{
		  push_idstck(x);
		  x = pop();
		}

	      while (!empty_idstck())
		  genquad(SYMTBL_TKNTYP_WRITE, -1, -1, pop_idstck());

	      if(S1())
		return true;
	      else
		return false;
	    }
	  else
	    return false;
	}
      else
	{
	  err_rcrsv_dscnt("(",g_token_type);
	  return false;
	}
    }
  else if (g_token_type == SYMTBL_TKNTYP_READLN)
    {
      next_token();
      if (g_token_type == SYMTBL_TKNTYP_OPNPAREN)
	{
	  push(QUADTBL_MARKER);	/* push marker to avoid over-popping */
	  next_token();

	  if(IDL())
	    {
	      x = pop();
	      while (x != QUADTBL_MARKER)
		{
		  push_idstck(x);
		  x = pop();
		}

	      while (!empty_idstck())
		  genquad(SYMTBL_TKNTYP_READLN, -1, -1, pop_idstck());

	      if(S1())
		return true;
	      else
		return false;
	    }
	  else
	    return false;
	}
      else
	{
	  err_rcrsv_dscnt("(",g_token_type);
	  return false;
	}
    }
  else if (g_token_type == SYMTBL_TKNTYP_WRITELN)
    {
      next_token();
      if (g_token_type == SYMTBL_TKNTYP_OPNPAREN)
	{
	  push(QUADTBL_MARKER);	/* push marker to avoid over-popping */
	  next_token();

	  if(IDL())
	    {
	      x = pop();
	      while (x != QUADTBL_MARKER)
		{
		  push_idstck(x);
		  x = pop();
		}

	      while (!empty_idstck())
		genquad(SYMTBL_TKNTYP_WRITE, -1, -1, pop_idstck());

	      /* just change the last one to writeln so that all privious prints come in a line */
	      struct quad qd = get_quad_fields_val(nq()-1);
	      qd.op = SYMTBL_TKNTYP_WRITELN;
	      set_quad_fields_val(nq()-1, qd);

	      if(S1())
		return true;
	      else
		return false;
	    }
	  else
	    return false;
	}
      else
	{
	  err_rcrsv_dscnt("(",g_token_type);
	  return false;
	}
    }
    else if (g_token_type == SYMTBL_TKNTYP_CASE)
    {
      next_token();

      if(C())
	{
	  x = pop();		/* result of condition C */
	  push(nq());		/* quad position when condition fails */
	  genquad(QUADTBL_JMPFALSE, x, -1, -1);

	  if (g_token_type == SYMTBL_TKNTYP_DO)
	    {
	      next_token();

	      if(S())
		{
		  if(M())
		    if(S1())
		      return true;
		    else
		      return false;
		  else
		    return false;
		}
	      else
		return false;
	    }
	  else
	    {
	      err_rcrsv_dscnt("do",g_token_type);
	      return false;
	    }
	}
      else
	return false;
    }
    else if (g_token_type == SYMTBL_TKNTYP_WHILE)
    {
      push(nq());		/* hook to come back */

      next_token();

      if(C())
	{
	  x = pop();
	  push(nq());
	  genquad(QUADTBL_JMPFALSE, x, -1, -1);

	  if (g_token_type == SYMTBL_TKNTYP_DO)
	    {
	      next_token();

	      if(S())
		{
		  if (g_token_type == SYMTBL_TKNTYP_OD)
		    {
		      x = pop();
		      y = pop();
		      genquad(QUADTBL_JMP, -1, -1, y);
		      assignquad(x, nq()); /* QUAD[x,4] = NQ */

		      next_token();

		      if(S1())
			return true;
		      else
			return false;
		    }
		  else
		    {
		      err_rcrsv_dscnt("od", g_token_type);
		      return false;
		    }
		}
	      else
		return false;
	    }
	  else
	    {
	      err_rcrsv_dscnt("do",g_token_type);
	      return false;
	    }
	}
      else
	return false;
    }
  else if(g_token_type == SYMTBL_TKNTYP_IF)
    {
      next_token();

      if(C())
	{
	  x = pop();		/* result of condition C */
	  push(nq());		/* remember to set later the jump false condition */
	  genquad(QUADTBL_JMPFALSE, x, -1, -1);

	  if(g_token_type == SYMTBL_TKNTYP_THEN)
	    {
	      next_token();

	      if(S())
		{
		  if(S2())
		    return true;
		  else
		    return false;
		}
	      else
		return false;
	    }
	  else
	    {
	      err_rcrsv_dscnt("then", g_token_type);
	      return false;
	    }
	}
      else
	return false;
    }
  else if(g_token_type == SYMTBL_TKNTYP_FOREACH)
    {
      next_token();
      if(g_token_type == SYMTBL_TKNTYP_ID)
	{
	  /* make sure id is not multidimensional array, 
	     we need only scalar variables as loop counter */
	  if (g_symtbl[g_token_subscript].dim != 0)
	    {
	      fprintf(_syntax_err_file_fp,
		      "[ERROR] SSA: line(%d): expected scalar identifier, but found it to be %d-dimensional array (%s)\n",
		      g_input_line, g_symtbl[g_token_subscript].dim->dims, g_symtbl[g_token_subscript].name);

#ifdef _DEBUG
	      printf("[ERROR] SSA: line(%d): expected scalar identifier, but found it to be %d-dimensional array (%s)\n",
		      g_input_line, g_symtbl[g_token_subscript].dim->dims, g_symtbl[g_token_subscript].name);
#endif

	      return false;
	    }

	  push(g_token_subscript); /* loop counter, save it */

	  next_token();
	  if (g_token_type == SYMTBL_TKNTYP_IN)
	    {
	      next_token();
	      if(g_token_type == SYMTBL_TKNTYP_ID)
		{
		  /* make sure id is multidimensional array, 
		     we need only array to process in foreach loops */
		  if (g_symtbl[g_token_subscript].dim == 0)
		    {
		      fprintf(_syntax_err_file_fp,
			      "[ERROR] SSA: line(%d): expected array identifier, but found it to be scalar (%s)\n",
			      g_input_line, g_symtbl[g_token_subscript].name);

#ifdef _DEBUG
		      printf("[ERROR] SSA: line(%d): expected array identifier, but found it to be scalar (%s)\n",
			      g_input_line, g_symtbl[g_token_subscript].name);
#endif

		      return false;
		    }

		  /* base address of the array */
		  x = pop();                            /* loop counter: base address of array */
		  genquad(QUADTBL_VARADDR, g_token_subscript, -1, x);
		  g_symtbl[x].index_var_4_array = true;

		  /* for checking if loop counter does not surpasses array boundary */
		  t = gentemp();                        /* array boundary */
		  genquad(QUADTBL_VARADDR, g_token_subscript, -1, t);
		  g_symtbl[t].index_var_4_array = true;
		  genquad(SYMTBL_TKNTYP_ADD, t, g_symtbl[g_token_subscript].dim->size_subscript, t);
		  push(nq());	                        /* hook to JMP back */
		  push(t);

		  t = gentemp();			/* result of condition [x < array_boundary] */
		  genquad(SYMTBL_TKNTYP_LESSTHAN, x, pop(), t);
		  struct quad q = get_quad_fields_val(nq()-1);
		  q.op1_dref = false;
		  q.op2_dref = false;
		  set_quad_fields_val(nq()-1, q);


		  push(x);				/* save loop counter */
		  push(nq());				/* Q: condition fails (JMPFALSE) so where to jump? */
		  genquad(QUADTBL_JMPFALSE, t, -1, -1); /* where to jump will be assigned soon */

		  next_token();
		  if (g_token_type == SYMTBL_TKNTYP_DO)
		    {
		      next_token();
		      if (S())
			{
			  if (g_token_type == SYMTBL_TKNTYP_OD)
			    {
			      y = pop();                           /* A: jump from here when condition fails (JMPFALSE) */
			      x = pop();                           /* loop counter */
			      genquad(SYMTBL_TKNTYP_ADD, x, find_in_symtbl("1",1), x); /* increment loop counter */
			      x = pop();                           /* quad containing loop condition */
			      genquad(QUADTBL_JMP, -1, -1, x);
			      assignquad(y, nq());                 /* QUAD[y,4] = NQ */

			      next_token();
			      if(S1())
				return true;
			      else
				return false;
			    }
			  else
			    {
			      err_rcrsv_dscnt("od", g_token_type);
			      return false;
			    }
			}
		      else
			return false;
		    }
		  else
		    {
		      err_rcrsv_dscnt("do", g_token_type);
		      return false;
		    }
		}
	      else
		{
		  err_rcrsv_dscnt("id", g_token_type);
		  return false;
		}
	    }
	  else
	    {
	      err_rcrsv_dscnt("in", g_token_type);
	      return false;
	    }
	}
      else
	{
	  err_rcrsv_dscnt("id", g_token_type);
	  return false;
	}
    }
  else if (g_token_type == SYMTBL_TKNTYP_WITH)
    {
      /* we are looking into a new scope */
      g_dec_section =  true;
      g_scope = create_new_scope(g_scope);

      next_token();
      if(D())
	{
	  if(S())
	    {
	      if (g_token_type == SYMTBL_TKNTYP_END)
		{
		  /* current scope ends, switch to parent */
		  g_scope = g_scope->parent;

		  next_token();
		  if(S1())
		    return true;
		  else
		    return false;
		}
	      else
		{
		  err_rcrsv_dscnt("end", g_token_type);
		  return false;
		}
	    }
	}
      else
	{
	  g_dec = false;
	  return false;
	}
    }
  else if(!(_print_error=false) && ID())
    {
      _print_error = true;
      if(g_token_type == SYMTBL_TKNTYP_ASSIGN)
	{
	  next_token();
	  if(E())
	    {
	      x = pop();	/* E */
	      y = pop();	/* ID */
	      genquad(SYMTBL_TKNTYP_ASSIGN, x, -1, y);

	      if(S1())
		return true;
	      else
		return false;
	    }
	  else
	    {
	      pop();		/* discard ID and this statement */
	      return S();
	    }
	}
      else
	{
	  err_rcrsv_dscnt(":=", g_token_type);
	  return false;
	}
    }
 
  err_rcrsv_dscnt("read write readln writeln case while if foreach with", g_token_type);
  return false;
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  S' -> ; S        |
 *     -> empty      |  {end : od fi esac else}
 */
boolean S1()
{
#ifdef _DEBUG
  printf("S1()\n");
#endif
  if(g_token_type == SYMTBL_TKNTYP_SEMICOLON)
    {
      next_token();
      if(S())
	return true;
      else
	return true;
    }
  else if(g_token_type == SYMTBL_TKNTYP_END || g_token_type == SYMTBL_TKNTYP_COLON
	  || g_token_type == SYMTBL_TKNTYP_OD || g_token_type == SYMTBL_TKNTYP_FI
	  || g_token_type == SYMTBL_TKNTYP_ESAC || g_token_type == SYMTBL_TKNTYP_ELSE)
      return true;
  else
    {
      err_rcrsv_dscnt("; end : od fi esac else", g_token_type);
      return S();		/* ignore this statement error completely ! */
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *  S2 -> fi S'      |
 *     -> else S fi S'|
 */
boolean S2()
{
#ifdef _DEBUG
  printf("S2()\n");
#endif
  int x, y;

  if (g_token_type == SYMTBL_TKNTYP_FI)
    {
      x = pop();		/* pop the jumpfalse condition quad */
      assignquad(x, nq());	/* QUAD[x,4] = NQ */

      next_token();

      if(S1())
	return true;
      else
	return false;
    }
  else if (g_token_type == SYMTBL_TKNTYP_ELSE)
    {
      y = nq();
      genquad(QUADTBL_JMP, -1, -1, -1); /* will update at the end of else statement(s) */
      x = pop();		/* pop the jumpfalse condition quad */
      assignquad(x, nq());	/* QUAD[x,4] = NQ */
      push(y);		/* to avoid executing else part we need
				   to have a JMP quad here pointing to
				   the end of else statement(s) */
      next_token();

      if(S())
	{
	  if (g_token_type == SYMTBL_TKNTYP_FI)
	    {
	      x = pop();	/* all else statment(s) end here, so pop JMP
				   quad and point it to statement(s) following 
				   end of this if-else construct */
	      assignquad(x, nq());	/* QUAD[x,4] = NQ */

	      next_token();
	      if(S1())
		return true;
	      else
		return false;
	    }
	  else
	    return false;
	}
      else
	return false;
    }
  else
    {
      err_rcrsv_dscnt("fi else", g_token_type);
      return false;
    }
}

/*       Rule        |    Selection Set
 * --------------------------------------
 *   M -> : C do S M | 
 *     -> esac       |
 */
boolean M()
{
#ifdef _DEBUG
  printf("M()\n");
#endif
  int x;

  if(g_token_type == SYMTBL_TKNTYP_COLON)
    {
      x = pop();		/* pop the jumpfalse condition quad */
      assignquad(x, nq());	/* QUAD[x,4] = NQ */

      next_token();

      if(C())
	{
	  x = pop();		/* result of condition C */
	  push(nq());		/* quad position when condition fails */
	  genquad(QUADTBL_JMPFALSE, x, -1, -1);

	  if(g_token_type == SYMTBL_TKNTYP_DO)
	    {
	      next_token();
	      if(S())
		if(M())
		  return true;
		else
		  return false;
	      else
		return false;
	    }
	  else
	    return false;
	}
      else
	return false;
    }
  else if (g_token_type == SYMTBL_TKNTYP_ESAC)
    {
      x = pop();		/* pop the jumpfalse condition quad */
      assignquad(x, nq());	/* QUAD[x,4] = NQ */

      next_token();

      return true;
    }
  else
    {
      err_rcrsv_dscnt(": esac", g_token_type);
      return false;
    }
}

/** start_rcrsv_dscnt_prsr: starting point of all the syntax analyzing
    returns 0 on success, otherwise some negative integer.
*/
int start_rcrsv_dscnt_prsr()
{
  if(!_ssa_initialized)
    return ESSA_NOT_INITIALIZED;

  add_2_symtbl("1",1);

  if (!P())			/* start recursive descent now */
    return ESSA_RCRSV_DSCNT_FNCTN_FLD;
  /* make sure there are now trailing tokens even when P() was successful */
  if (g_errors == 0 &&  next_token() != EOF)
    {
      err_rcrsv_dscnt("EOF", EEOF_IS_LAST_TKN);
      return EEOF_IS_LAST_TKN;
    }

  return EOK;
}

void err_rcrsv_dscnt(const char* expected, int found)
{
  static char err_msg[1024] = {0};
  static int token_type = -1;
  static int token_subscript = -1;
  static char token[SYMTBL_INDX_NAME_SIZE] = {0};

  /* make sure we do not print spurious error messages for this token */
  /* if (token_type != -1 && token_subscript != -1) */
  /*   if(token_type == g_token_type && token_subscript == g_token_subscript) */
  /*     return; */

  if (0 <= found && found < SYMTBL_RSRVD_TKNS)
    {
      eat_trailing_whitespace(g_symtbl[found].name, token, SYMTBL_INDX_NAME_SIZE);
      sprintf(err_msg, "[ERROR] SSA: line(%d): expected \'%s\', but found \'%s\'", g_input_line, expected, token);
    }
  else if (SYMTBL_RSRVD_TKNS <= found && found < SYMTBL_ROWS)
    {
      eat_trailing_whitespace(g_symtbl[g_token_subscript].name, token, SYMTBL_INDX_NAME_SIZE);
      sprintf(err_msg, "[ERROR] SSA: line(%d): expected \'%s\', but found \'%s\'", g_input_line, expected, token);
    }
  else if (found == ELEX_UNDCLRD_VAR)
    {
      sprintf(err_msg, "[ERROR] SSA: line(%d): expected \'%s\', but %s: \'%s\'", g_input_line, expected, error_codes_description[found*(-1)], get_token_str());
    }
  else if (found == ESSA_ARR_IMPRPR_SUBSCRIPT)
    {
      if (g_token_subscript < 0)
	{
	  sprintf(err_msg, "[ERROR] SSA: line(%d): array index not a constant", g_input_line);
	}
      else
	{
	  eat_trailing_whitespace(g_symtbl[g_token_subscript].name, token, SYMTBL_INDX_NAME_SIZE);
	  sprintf(err_msg, "[ERROR] SSA: line(%d): \'%s\' is a %d-dimensional array, but found error(\'%s\')", g_input_line, token, g_symtbl[g_token_subscript].dim->dims, error_codes_description[found*(-1)]);
	}
    }
  else
    {
      sprintf(err_msg, "[ERROR] SSA: line(%d): expected \'%s\', but found error (\"%s\")", g_input_line, expected, error_codes_description[found*(-1)]);
    }

  fprintf(_syntax_err_file_fp, "%s\n", err_msg);
  token_type = g_token_type;
  token_subscript = g_token_subscript;
  g_errors++;
#ifdef _DEBUG
  printf("%s\n", err_msg);
#endif
}

void push_id_type_stck(int id)
{
  if (_tos_id_type_stck >= ID_TYPE_STACK_SIZE)
    {
      printf("FATAL: ID TYPE stack exhausted, cannot push variables anymore, aborting!\n");
      exit(EIDTYPESTCK_OVRFL); /* fatal: ID Type stack exhausted, bad size judgment! */
    }

  _id_type_stck[++_tos_id_type_stck] = id;
}

int pop_id_type_stck()
{
  int x = 0;
  if (empty_id_type_stck())
    {
      printf("FATAL: ID TYPE Stack is tried over-popped ! aborting.");
      exit (EIDTYPESTCK_NOTHING_TO_POP);
    }
  x = _id_type_stck[_tos_id_type_stck];
  _id_type_stck[_tos_id_type_stck] = 0;
  _tos_id_type_stck--;
  return x;
}

bool empty_id_type_stck()
{
  if (_tos_id_type_stck < 0)
    return true;

  return false;
}

struct dimension *add_2_dim_array(struct dimension *head, int dim_subscript)
{
  struct dimension *a = malloc(sizeof(struct dimension));

  a->size_subscript = 0;
  a->dim_subscript = dim_subscript;
  a->dims = 0;
  a->next = 0;
  a->prev = 0;
  a->subelements_subscript = 0;

  if (head == 0)
      return a;		/* create list */

  /* add to the front of the list */
  a->next = head;
  head->prev = a;

  if (head->next == 0) 		/* very second element added to list */
    a->subelements_subscript = head->dim_subscript;

  else				/* mulitplication starts now */
    a->subelements_subscript = add_number_2_symtbl(strtol(g_symtbl[head->dim_subscript].name,0,0)
						   * strtol(g_symtbl[head->subelements_subscript].name,0,0));

  return a;
}

void push_array_indices(int index)
{
  if (_tos_array_indices >= ID_TYPE_STACK_SIZE)
    {
      printf("FATAL: ARRAY_INDICES stack overflow, aborting!\n");
      exit(EIDTYPESTCK_OVRFL); /* fatal: ARRAY_INDICES stack exhausted, bad size judgment! */
    }

  _array_indices[++_tos_array_indices] = index;
}

int pop_array_indices()
{
  int x = 0;
  if (empty_array_indices())
    {
      printf("FATAL: ARRAY_INDICES Stack is tried over-popped ! aborting.");
      exit (EIDTYPESTCK_NOTHING_TO_POP);
    }

  x = _array_indices[_tos_array_indices];
  _array_indices[_tos_array_indices] = 0;
  _tos_array_indices--;
  return x;
}

bool empty_array_indices()
{
  if (_tos_array_indices < 0)
    return true;

  return false;
}

void compute_array_index_var_increment(int token_subscript, int pointer_subscript)
{
  int x = 0, t;
  int retval = 0;
  bool warned = false;
  struct dimension *arr_dim = 0;

  if (token_subscript < 0 || pointer_subscript < 0 
      || token_subscript >= SYMTBL_ROWS || pointer_subscript > SYMTBL_ROWS
      || g_symtbl[token_subscript].dim == 0 || empty_array_indices())
    return ;

  {
    int i=0, a[ID_TYPE_STACK_SIZE];
    memset(a,-1,ID_TYPE_STACK_SIZE);

    while(!empty_array_indices())
      a[i++]= pop_array_indices();

    a[i] = -1;    
    i = 0;

    while(a[i] >= 0)
      push_array_indices(a[i++]);
  }

  x = pop_array_indices();     /* array: a[3,4,...,6], pop 6 from stack;
				  we are working in '<--' direction */

  /* go to the last in array dimension list */
  arr_dim = g_symtbl[token_subscript].dim;
  while(arr_dim != 0 && arr_dim->next != 0) arr_dim = arr_dim->next;

  genquad(SYMTBL_TKNTYP_ADD, x, pointer_subscript, pointer_subscript);
  if (g_symtbl[x].index_var_4_array)
    {
      /* handling case like: a[a[2,2,...], a[2,2,...],...] */
      struct quad qd = get_quad_fields_val(nq()-1);
      qd.op1_dref = true;
      set_quad_fields_val(nq()-1, qd);
    }

  if (g_symtbl[x].token_type == SYMTBL_TKNTYP_CONS 
      && strtol(g_symtbl[x].name,0,0) >= strtol(g_symtbl[arr_dim->dim_subscript].name,0,0))
    {
      printf("[WARNING] SSA: line(%d): subscript(s) for \'%s\' are exceeding its size.\n",
	     g_input_line, g_symtbl[token_subscript].name);
    }

  if (arr_dim->prev == 0 && empty_array_indices())  
    return ;			/* this is a single dimension array */

  /* This is multidimension array; both top of the stack (indices read from input) and dimension list
     pointer 'arr_dim' are at same dimension */
  t = gentemp();
  while (!empty_array_indices())
    {
      x = pop_array_indices();
      arr_dim = arr_dim->prev;

      if (arr_dim == 0) break;

      if (!warned && g_symtbl[x].token_type == SYMTBL_TKNTYP_CONS 
	  && strtol(g_symtbl[x].name,0,0) >= strtol(g_symtbl[arr_dim->dim_subscript].name,0,0))
  	{
  	  warned = true;
  	  printf("[WARNING] SSA: line(%d): subscript(s) for \'%s\' are exceeding its size\n",
  		 g_input_line, g_symtbl[token_subscript].name);
  	}

      retval += strtol(g_symtbl[x].name,0,0)*strtol(g_symtbl[arr_dim->subelements_subscript].name,0,0);
      genquad(SYMTBL_TKNTYP_MULT, x, arr_dim->subelements_subscript, t);
      genquad(SYMTBL_TKNTYP_ADD, t, pointer_subscript, pointer_subscript);
    }
}

int add_number_2_symtbl(int x)
{
  char index_size[SYMTBL_INDX_NAME_SIZE] = {0};
  int index_len = 0;

  memset(index_size, 0, SYMTBL_INDX_NAME_SIZE);
  snprintf(index_size, SYMTBL_INDX_NAME_SIZE,"%d", x);
  x = 0;

  /* count length of the token */
  while (index_size[x++] != '\0') index_len++;

  /* make its entry also in symbol table so that 
     we can refer to it in future in code generation */
  if ((x = find_in_symtbl(index_size, index_len)) < 0)
    x = add_2_symtbl(index_size, index_len);

  return x;
}
