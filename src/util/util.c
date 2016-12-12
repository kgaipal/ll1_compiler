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

#include <symbol_table.h>
#include <defines.h>
#include <util.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>

bool _symtbl_initialized = false;

int initialize_symtbl()
{
  int i = 0;

  if (_symtbl_initialized)
    return 0;

  for (i = 0; i < SYMTBL_ROWS; i++)
    {
      g_symtbl[i].token_type = i;
      g_symtbl[i].scope = 0;	/* for reserved keywords, no scope exists such */
    }


  snprintf(g_symtbl[SYMTBL_TKNTYP_PROGRAM].name,SYMTBL_INDX_NAME_SIZE,"%-15s","program");
  snprintf(g_symtbl[SYMTBL_TKNTYP_IN].name,SYMTBL_INDX_NAME_SIZE,"%-15s","in");
  snprintf(g_symtbl[SYMTBL_TKNTYP_BEGIN].name,SYMTBL_INDX_NAME_SIZE,"%-15s","begin");
  snprintf(g_symtbl[SYMTBL_TKNTYP_END].name,SYMTBL_INDX_NAME_SIZE,"%-15s","end");
  snprintf(g_symtbl[SYMTBL_TKNTYP_ARRAY].name,SYMTBL_INDX_NAME_SIZE,"%-15s","array");
  snprintf(g_symtbl[SYMTBL_TKNTYP_INTEGER].name,SYMTBL_INDX_NAME_SIZE,"%-15s","integer");
  snprintf(g_symtbl[SYMTBL_TKNTYP_NOT].name,SYMTBL_INDX_NAME_SIZE,"%-15s","not");
  snprintf(g_symtbl[SYMTBL_TKNTYP_AND].name,SYMTBL_INDX_NAME_SIZE,"%-15s","and");
  snprintf(g_symtbl[SYMTBL_TKNTYP_OR].name,SYMTBL_INDX_NAME_SIZE,"%-15s","or");
  snprintf(g_symtbl[SYMTBL_TKNTYP_DO].name,SYMTBL_INDX_NAME_SIZE,"%-15s","do");
  snprintf(g_symtbl[SYMTBL_TKNTYP_ESAC].name,SYMTBL_INDX_NAME_SIZE,"%-15s","esac");
  snprintf(g_symtbl[SYMTBL_TKNTYP_READ].name,SYMTBL_INDX_NAME_SIZE,"%-15s","read");
  snprintf(g_symtbl[SYMTBL_TKNTYP_WRITE].name,SYMTBL_INDX_NAME_SIZE,"%-15s","write");
  snprintf(g_symtbl[SYMTBL_TKNTYP_READLN].name,SYMTBL_INDX_NAME_SIZE,"%-15s","readln");
  snprintf(g_symtbl[SYMTBL_TKNTYP_WRITELN].name,SYMTBL_INDX_NAME_SIZE,"%-15s","writeln");
  snprintf(g_symtbl[SYMTBL_TKNTYP_CASE].name,SYMTBL_INDX_NAME_SIZE,"%-15s","case");
  snprintf(g_symtbl[SYMTBL_TKNTYP_WHILE].name,SYMTBL_INDX_NAME_SIZE,"%-15s","while");
  snprintf(g_symtbl[SYMTBL_TKNTYP_OD].name,SYMTBL_INDX_NAME_SIZE,"%-15s","od");
  snprintf(g_symtbl[SYMTBL_TKNTYP_IF].name,SYMTBL_INDX_NAME_SIZE,"%-15s","if");
  snprintf(g_symtbl[SYMTBL_TKNTYP_FI].name,SYMTBL_INDX_NAME_SIZE,"%-15s","fi");
  snprintf(g_symtbl[SYMTBL_TKNTYP_THEN].name,SYMTBL_INDX_NAME_SIZE,"%-15s","then");
  snprintf(g_symtbl[SYMTBL_TKNTYP_ELSE].name,SYMTBL_INDX_NAME_SIZE,"%-15s","else");
  snprintf(g_symtbl[SYMTBL_TKNTYP_FOREACH].name,SYMTBL_INDX_NAME_SIZE,"%-15s","foreach");
  snprintf(g_symtbl[SYMTBL_TKNTYP_COLON].name,SYMTBL_INDX_NAME_SIZE,"%-15s",":");
  snprintf(g_symtbl[SYMTBL_TKNTYP_SEMICOLON].name,SYMTBL_INDX_NAME_SIZE,"%-15s",";");
  snprintf(g_symtbl[SYMTBL_TKNTYP_COMMA].name,SYMTBL_INDX_NAME_SIZE,"%-15s",",");
  snprintf(g_symtbl[SYMTBL_TKNTYP_OPNSQBRKT].name,SYMTBL_INDX_NAME_SIZE,"%-15s","[");
  snprintf(g_symtbl[SYMTBL_TKNTYP_CLSQBRKT].name,SYMTBL_INDX_NAME_SIZE,"%-15s","]");
  snprintf(g_symtbl[SYMTBL_TKNTYP_ASSIGN].name,SYMTBL_INDX_NAME_SIZE,"%-15s",":=");
  snprintf(g_symtbl[SYMTBL_TKNTYP_OPNPAREN].name,SYMTBL_INDX_NAME_SIZE,"%-15s","(");
  snprintf(g_symtbl[SYMTBL_TKNTYP_CLPAREN].name,SYMTBL_INDX_NAME_SIZE,"%-15s",")");
  snprintf(g_symtbl[SYMTBL_TKNTYP_EXP].name,SYMTBL_INDX_NAME_SIZE,"%-15s","exp");
  snprintf(g_symtbl[SYMTBL_TKNTYP_DIV].name,SYMTBL_INDX_NAME_SIZE,"%-15s","/");
  snprintf(g_symtbl[SYMTBL_TKNTYP_MULT].name,SYMTBL_INDX_NAME_SIZE,"%-15s","*");
  snprintf(g_symtbl[SYMTBL_TKNTYP_ADD].name,SYMTBL_INDX_NAME_SIZE,"%-15s","+");
  snprintf(g_symtbl[SYMTBL_TKNTYP_SUB].name,SYMTBL_INDX_NAME_SIZE,"%-15s","-");
  snprintf(g_symtbl[SYMTBL_TKNTYP_LESSTHAN].name,SYMTBL_INDX_NAME_SIZE,"%-15s","<");
  snprintf(g_symtbl[SYMTBL_TKNTYP_GRTRTHAN].name,SYMTBL_INDX_NAME_SIZE,"%-15s",">");
  snprintf(g_symtbl[SYMTBL_TKNTYP_WITH].name,SYMTBL_INDX_NAME_SIZE,"%-15s","with");
  snprintf(g_symtbl[SYMTBL_TKNTYP_EQUAL].name,SYMTBL_INDX_NAME_SIZE,"%-15s","=");

  snprintf(g_symtbl[QUADTBL_JMPFALSE].name,SYMTBL_INDX_NAME_SIZE,"%-15s","#JMPFALSE");
  snprintf(g_symtbl[QUADTBL_JMP].name,SYMTBL_INDX_NAME_SIZE,"%-15s","#JMP");
  snprintf(g_symtbl[QUADTBL_VARADDR].name,SYMTBL_INDX_NAME_SIZE,"%-15s","#VARADDR");
  snprintf(g_symtbl[QUADTBL_HLT].name,SYMTBL_INDX_NAME_SIZE,"%-15s","#HLT");
  snprintf(g_symtbl[QUADTBL_MARKER].name,SYMTBL_INDX_NAME_SIZE,"%-15s","#MARKER");

  /* snprintf does not set the last character as space instead NULL string ! */
  for (i = 0; i < SYMTBL_ROWS; i++)
    {
      if (g_symtbl[i].name[0] == 0)
	break;

      g_symtbl[i].name[SYMTBL_INDX_NAME_SIZE-1] = ' ';
    }

   _symtbl_initialized = true;

   g_toplevel_scope = g_scope = malloc(sizeof(struct scope));

   g_scope->parent = 0;
   g_scope->prev_sibling = 0;
   g_scope->id = TOP_LEVEL_ID;
   g_scope->next_sibling = 0;
   g_scope->first_child = 0;

   g_input_line = 0;
   g_dec_section = false;

   error_codes_description[0] = "No Error";
   error_codes_description[1] = "End of file encountered";
   error_codes_description[2] = "Symbol Table exhausted, no more space to add new token or search failed";
   error_codes_description[3] = "File I/O error";
   error_codes_description[4] = "Token length is more than 15";
   error_codes_description[5] = "Invalid token, for example \'#token\', \'123token\', or \'tok#en\' are all invalid token";
   error_codes_description[6] = "Recursive descent function(s) failed, check syntax error file for more details";
   error_codes_description[7] = "Program should end with \"end\", instead some unwanted characters are present, may be syntax error";
   error_codes_description[8] = "Token search failed in Symbol Table";
   error_codes_description[9] = "Quad Table exhausted, no more space to add quad or search failed";
   error_codes_description[10] = "Semantic Action Stack exhausted, no more space to push items";
   error_codes_description[11] = "Semantic Action Stack is already empty, nothing to pop";
   error_codes_description[12] = "Undeclared variable found";
   error_codes_description[13] = "Variable accessed out of its scope";
   error_codes_description[14] = "Variable redeclared in same scope";
   error_codes_description[15] = "ID Stack exhausted, no more space to push items";
   error_codes_description[16] = "ID Stack is already empty, nothing to pop";
   error_codes_description[17] = "SSA module not initialized";
   error_codes_description[18] = "Zero length token found";
   error_codes_description[19] = "Underflow on IDTYPE stack";
   error_codes_description[20] = "Overflow on IDTYPE stack";
   error_codes_description[21] = "Data section offset is mis-calculated";
   error_codes_description[22] = "subscripts of array here does not match with its declaration";

   g_errors = 0;

  return 0;

}

int write_symtbl()
{
  int retval = EOK;
  FILE * fp = 0;
  int i = 0;

  /* printf("\nPrint symbol table. ESC to cancel..."); */
  /* if (getchar() == 27) */
  /*   return; */

  fp = fopen(SYMTBL_FILE, "w");

  if (!fp)
    {
      printf("[ERROR] SYMTBL: \"%s\" : No such file or directory\n", SYMTBL_FILE);
      return EFILE_IO;
    }

  fprintf(fp,"   |     %s       |%3s | %s | %s | %s | %s ", "NAME", "TT", "SCOPE", "DIM", "MEM LOC","INDX_4_ARR");
  fprintf(fp,"\n---------------------------------------------------------------");
  for (i=0; i < SYMTBL_ROWS; i++)
    {
      if (*(g_symtbl[i].name) == 0)
	continue;

      fprintf(fp,"\n[%02d] %s|%3d | %5d |%3d  | %7d | %5d ",i,g_symtbl[i].name, g_symtbl[i].token_type, get_complete_scope(g_symtbl[i].scope), (g_symtbl[i].dim?g_symtbl[i].dim->dims:0), g_symtbl[i].mem_loc, g_symtbl[i].index_var_4_array);
    }
  fprintf(fp,"\n---------------------------------------------------------------\n");

  fclose(fp);

  return EOK;
}

boolean is_id(const char *s)
{
  int len = SYMTBL_INDX_NAME_SIZE;
  int i = 0;

  if(isdigit((int)s[i]))
    return false;

  /* one thing is sure now that 's' is not a number, 
     check if it starts with a letter? */
  if (!isalpha((int)s[0]))
    return false;

  /* only check for alphanumerics */
  for (i = 0; i < len && s[i] != 0 && s[i] != ' '; i++)
    if (!isalnum((int)s[i]))
      return false;

  return true;
}

boolean is_cons(const char *s)
{
  int len = SYMTBL_INDX_NAME_SIZE;
  int i = 0;

  /* check if the first symbol is a number, if it is rest must be numbers */
  for (i = 0; i < len && s[i] != 0 && s[i] != ' '; i++)
    if(!isdigit((int)s[i]))
      return false;

  return true;
}

struct scope* create_new_scope(struct scope * parent)
{
  struct scope *child = 0;
  struct scope *sibling = 0;

  assert(parent != 0);		/* cannot add new scope to 'nothing' ! ;
				   caller must be crazy calling function this way */

  child  = malloc(sizeof(struct scope));

  if (child == 0)
    {
      printf("FATAL: Cannot allocate memory to child for scope:");
      abort();
    }

  child->parent = parent;
  child->next_sibling = 0;
  child->first_child = 0;

  /* find appropriate place to insert */
  sibling = parent->first_child;

  if (sibling == 0)
    {
      /* first child ever */
      child->prev_sibling = 0;
      child->id = TOP_LEVEL_ID;
      parent->first_child = child;

      return child;
    }

  while (sibling->next_sibling != 0) sibling = sibling->next_sibling;

  child->id = sibling->id+1;
  child->prev_sibling = sibling;
  sibling->next_sibling = child;

  return child;
}

int get_complete_scope(struct scope *scope)
{
  int stack[10] = {-1};
  int tos = -1;
  int complete_scope = 0;
  int n = 0;

  struct scope * s = scope;

  /* backtrack to parent */
  while (s != 0)
    {
      stack[++tos] = s->id;
      s = s->parent;
    }

  while (tos != -1)
    {
      n = stack[tos];
      complete_scope += n * exp10(tos);
      tos--;
    }

  return complete_scope;
}

int resolve_scope_conflict(int accessed_token_subscript)
{
  int i = -1;
  struct scope* scope_of_accessed_token = g_symtbl[accessed_token_subscript].scope;
  struct scope* current_scope = g_scope;
  char *accessed_token = g_symtbl[accessed_token_subscript].name;

  if (scope_of_accessed_token == current_scope)
    return accessed_token_subscript;

  while (current_scope != 0)
    {
      for (i = SYMTBL_ROWS-1; i >= SYMTBL_RSRVD_TKNS; i--)
	{
	  if (g_symtbl[i].scope == 0)
	    continue;		/* need to check only identifiers (where scope information is valid) */
	  /* reduce search area by looking into current scope only;
	     if match found, return the subscript */

	  if ((g_symtbl[i].scope == current_scope)
	      && (memcmp(g_symtbl[i].name, accessed_token, SYMTBL_INDX_NAME_SIZE) == 0))
	    return i;
	}

      current_scope = current_scope->parent; /* try to find out with a higher scope, 
						if match found in that scope,
						we have resolved the conflict :) */
    }

  return ESYMTBL_TKN_NOT_FOUND;	   /* nothing found */
}

/** If the scope of the token accessed, is the parent
 *  of the scope we are in right now, return true; 
 *  else it is violation.
 */
boolean is_referenced_out_of_scope(int accessed_token_subscript)
{
  struct scope* valid_scope_of_token = g_symtbl[accessed_token_subscript].scope;
  struct scope* current_scope = g_scope;

  if (valid_scope_of_token == current_scope)
    return false;		/* we are accessing variable defined in current scope itself */

  while (current_scope != 0)
    {
      if (valid_scope_of_token == current_scope)
	return false;

      current_scope = current_scope->parent;
    }

  return true;
}

boolean is_redeclared(const char *token)
{
  int i = 0;
  struct scope* current_scope = g_scope;

  for (i = SYMTBL_RSRVD_TKNS; i < SYMTBL_ROWS; i++)
    {
      if (g_symtbl[i].scope == 0)
	continue;		/* need to check only identifiers (where scope information is valid) */

      if ((g_symtbl[i].scope == current_scope)
	  && (memcmp(g_symtbl[i].name, token, SYMTBL_INDX_NAME_SIZE) == 0))
	return true;
    }

  return false;
}


/** find_in_symtbl: 
 *
 */
int find_in_symtbl(const char *token, const int len)
{
  int pos = 0;  /* position where inserted in table */
  char pattern[SYMTBL_INDX_NAME_SIZE] = {'\0'};
  boolean any_empty_pos_in_symbtbl = false;

  boolean isid = is_id(token);
  boolean iscons = is_cons(token);

  if(len > SYMTBL_INDX_NAME_SIZE)
    return ELEX_EXCSS_TKN_LNGTH; /* length cannot be accomodated in table; reject token! */

  /* atleast identifier or number */
  if (isalnum(token[0]))
    if (!(iscons|isid))
      return ELEX_INVALID_TKN;

  /* convert to lower case for comparison */
  for (pos = 0; pos < len; pos++) pattern[pos] = tolower(token[pos]);

  /* pad with spaces */
  for (pos = len; pos < SYMTBL_INDX_NAME_SIZE; pos++) pattern[pos] = ' ';

  /* now find the position in symbol table */
  for (pos = 0; pos < SYMTBL_ROWS; pos++)
    {
       if (!any_empty_pos_in_symbtbl && '\0' == g_symtbl[pos].name[0])
	 any_empty_pos_in_symbtbl = true;

       if (memcmp(g_symtbl[pos].name, pattern, SYMTBL_INDX_NAME_SIZE) == 0)
	{
	  /* match found, but check for scope */

	  if (g_symtbl[pos].scope == 0)
	    return pos;		/* reserved keyword/constant, scope concept is invalid */

	  else if (g_symtbl[pos].scope == g_scope)
	    return pos;		/* found exact identifier match! */

	  else 			
	    { 
	      /* case 1: We found an undeclared variable. 

		 case 2: We found reference to a variable which (if not an undeclared variable)
		         is already present in symbol table but may be with a different scope.

			 case 2.1: We are accessing an variable out of its scope! Issue error to callee.

			 case 2.2: scope conflict; variables with same name as in subscope is accessed;
			           give preference to local variable.
			   
			 case 2.3: no scope conflict; find subscript with scope in which it was declared.
	      */

	      if (is_referenced_out_of_scope(pos))
		return ELEX_ILLEGAL_SCOPE_ACCESS;	/* case 2.1 */

	      /* case 2.2 and 2.3 */
	      if ((pos = resolve_scope_conflict(pos)) < 0)
		return ELEX_UNDCLRD_VAR;	/* case 1: if there was an entry in symbol table, we would have found it! */
	      else 
		return pos;
	    }
	}

    }/* end for */

  if (!isalnum(token[0]))
    return ELEX_INVALID_TKN;	/* Some illegal token like '#token'; 
				   This neverthless cannot be in our symbol table */

  if (any_empty_pos_in_symbtbl)
      return ELEX_UNDCLRD_VAR;	/* case 1: if there was an entry in symbol table, we would have found it! */

  if(pos >= SYMTBL_ROWS)
    {
      printf("\nSymbol table exhausted, cannot add new symbol to it, aborting...");
      exit(ESYMTBL_OVRFL); /* fatal: symbol table exhausted, bad judgment for table size ! */
    }

  return pos; /* return index where inserted */
}

/** add_2_symtbl: 
 *
 */
int add_2_symtbl(const char *token, const int len)
{
  int pos = 0;  /* position where inserted in table */
  char pattern[SYMTBL_INDX_NAME_SIZE] = {'\0'};

  boolean isid = is_id(token);
  boolean iscons = is_cons(token);

  if(len > SYMTBL_INDX_NAME_SIZE)
    return ELEX_EXCSS_TKN_LNGTH; /* length cannot be accomodated in table; reject token! */

  /* atleast identifier or number */
  if (isalnum(token[0]))
    if (!(iscons|isid))
      return ELEX_INVALID_TKN;

  /* convert to lower case for comparison */
  for (pos = 0; pos < len; pos++) pattern[pos] = tolower(token[pos]);

  /* pad with spaces */
  for (pos = len; pos < SYMTBL_INDX_NAME_SIZE; pos++) pattern[pos] = ' ';

  /* now find the position in symbol table to insert */
  for (pos = 0; pos < SYMTBL_ROWS; pos++)
    {
       if ('\0' == g_symtbl[pos].name[0])
	{ 
	  /* empty position found, either it is constant or id */
	  if (isid)
	    {
	      g_symtbl[pos].token_type = SYMTBL_TKNTYP_ID;
	      memcpy(g_symtbl[pos].name, pattern, SYMTBL_INDX_NAME_SIZE);
	      g_symtbl[pos].scope = g_scope;
	    }
	  else if (iscons)
	    {
	      g_symtbl[pos].token_type = SYMTBL_TKNTYP_CONS;
	      memcpy(g_symtbl[pos].name, pattern, SYMTBL_INDX_NAME_SIZE);
	      g_symtbl[pos].scope = 0; /* constants do not have any thing related to scoping! */
	    }

	  return pos;
	}
       else if (memcmp(g_symtbl[pos].name, pattern, SYMTBL_INDX_NAME_SIZE) == 0)
	{
	  /* match found, but check for scope */

	  if (g_symtbl[pos].scope == 0)
	    return pos;		/* reserved keyword/constant, scope concept is invalid */

	  else if (g_symtbl[pos].scope == g_scope)
	    {
	      if (pos >= SYMTBL_RSRVD_TKNS && is_redeclared(pattern))
		return ELEX_RE_DCLRD_VAR; /* make sure it is not redeclared */
	      else
		return pos;		/* found exact identifier match! */
	    }

	  else 			
	    { 
	      /* Two possibilities: 
		 case 1: We found a new variable declaration in a new scope/subscope.
		         Potential for scope conflict whenever it or other conflicting 
			 variable will be accessed. Add to symbol table.

		 case 2: We found a variable redeclared. Issue warning or error to callee.
		         This information is useful for SSA, so preferably they should
		         deal with it.
	      */

	      if (is_redeclared(pattern))
		return ELEX_RE_DCLRD_VAR; /* case 2 */

	      continue;	/* case 1 */

	      /* For the sake of simplicity in code, in this
		 case we still continue to loop and wait till
		 empty position is found, which will be checked
		 in very first 'if' case in this 'for' loop;
	      */
	    }
	}

    }/* end for */

  if(pos >= SYMTBL_ROWS)
    {
      printf("Symbol table exhausted, cannot add new symbol to it. Aborted.\n");
      exit(ESYMTBL_OVRFL); /* fatal: symbol table exhausted, bad judgment for table size ! */
    }

  return pos; /* return index where inserted */
}

void eat_trailing_whitespace(const char* src, char* dest, int len)
{
  int i = 0;

  if (!(dest != 0 && src != 0 && len > 0))
    return;

  memset(dest, 0, len);

  for (i = 0; i < len; i++)
    {
      if (src[i] == ' ')
	break;

      dest[i] = src[i];
    }
}
