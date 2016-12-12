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

#ifndef _UTIL_H_
#define _UTIL_H_

/* Symbol Table is intialized to some predefined entries.
   This function must be called before any operation on symbol table.
 */

#include <defines.h>
#include <symbol_table.h>

int initialize_symtbl();
int write_symtbl();

boolean is_id(const char *s);
boolean is_cons(const char *s);

/* NOTE: only malloc's data structure and updates appropriate fields in parent and children (if any) */
struct scope* create_new_scope(struct scope *parent);

int get_complete_scope(struct scope *scope);
int resolve_scope_conflict(int pos);
boolean is_referenced_out_of_scope(int accessed_token_subscript);
boolean is_redeclared(const char *token);
int add_2_symtbl(const char *token, const int len);
int find_in_symtbl(const char *token, const int len);
void eat_trailing_whitespace(const char* src, char* dest, int len);
#endif /* _UTIL_H_ */
