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

#ifndef _DEFINES_H_
#define _DEFINES_H_

typedef int bool;
typedef int boolean;
#define false 0
#define true  1
#define LISTING_FILE "listing_file.txt"
#define SYNTX_ERR_FILE "syntax_error.txt"
#define QUADTBL_FILE "quad_table.txt"
#define SYMTBL_FILE "symbol_table.txt"

/* error codes for the project */
#define EOK                         0 /* no error */

#ifndef EOF
#define EOF                        -1 /* end of input file */
#endif

#define ESYMTBL_OVRFL              -2 /* Symbol table exhausted, fatal error */
#define EFILE_IO                   -3 /* input output operations on file failed */
#define ELEX_EXCSS_TKN_LNGTH       -4 /* maximum token length violated */
#define ELEX_INVALID_TKN           -5 /* first letter must be character, rest can be digits or characters*/
#define ESSA_RCRSV_DSCNT_FNCTN_FLD -6 /* recursive descent functions failed, correspondingly there must be an entry in SYNTX_ERR_FILE */
#define EEOF_IS_LAST_TKN           -7 /* if there is any trailing token instead of end-of-file (EOF) */
#define ESYMTBL_TKN_NOT_FOUND      -8 /* returned when the requested token is not found */
#define EQUADTBL_OVRFL             -9 /* Quad table exhausted */
#define ESAS_OVRFL                -10 /* SAS exhausted */
#define ESAS_NOTHING_TO_POP       -11 /* SAS is empty, nothing to pop */
#define ELEX_UNDCLRD_VAR          -12 /* accessing undeclared variable */
#define ELEX_ILLEGAL_SCOPE_ACCESS -13 /* accessing variable out of its scope */
#define ELEX_RE_DCLRD_VAR         -14 /* redeclaration of variable */
#define ESAS_OVRFL_IDSTCK         -15 /* ID Stack exhausted */
#define ESAS_NOTHING_TO_POP_IDSTCK -16/* ID Stack is empty, nothing to pop */
#define ESSA_NOT_INITIALIZED      -17 /* SSA module not initialized */
#define ELEX_ZERO_LENGTH_TKN_FND  -18 /* Zero length token found */
#define EIDTYPESTCK_NOTHING_TO_POP -19/* underflow on IDTYPE stack */
#define EIDTYPESTCK_OVRFL         -20 /* overflow on IDTYPE stack */
#define EDATA_SECTION_CALC_FAILED -21 /* data section offset is mis-calculated */
#define ESSA_ARR_IMPRPR_SUBSCRIPT -22 /* subscripts of array in statement is either more or less than what was mentioned in its declaration */

/* array of error code strings for above numerical error codes */
const char* error_codes_description[1024];

#endif
