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

#include <stdio.h>
#include <defines.h>
#include <util.h>
#include <lexical_analyzer.h>
#include <ssa.h>
#include <quad.h>
#include <codegen.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void print_gpl()
{
  printf("\n\
    LL(1) Compiler  Copyright (C) 2010 Kshitij Gaipal\n\
    This program comes with ABSOLUTELY NO WARRANTY; for details type 'w'.\n\
    This is free software, and you are welcome to redistribute it\n\
    under certain conditions; type 'c' for details.\n\n Type \'w\', \'c\' for options mentioned above or ENTER to continue ...\n");

  switch(getchar())
    {
    case 'w':
      {			/* show warranty section */
	printf("\n\
  15. Disclaimer of Warranty.\n\
\n\
  THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\n\
APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\n\
HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n\
OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n\
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n\
PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\n\
IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\n\
ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\n\
\n\
  16. Limitation of Liability.\n\
\n\
  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n\
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS\n\
THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY\n\
GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE\n\
USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF\n\
DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD\n\
PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\n\
EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF\n\
SUCH DAMAGES.\n");
	exit(1);
      }
    case 'c':
      {			/* show complete COPYING file */
	int lines = 0;
	int n = 80;
	int nchars = 0;
	char * line;
	char buffer[80];
	line = buffer;
	FILE *fp = fopen("COPYING","r");

	if (fp == 0)
	  exit(1);

	do
	  {
	    memset(buffer, 0, 80);
	    if ((nchars = getline(&line, &n, fp)) <= 0) break;
	    lines++;

	    if (lines == 589)break;

	    printf("%s",buffer);
	  } while(nchars > 0);

	if (fp)
	  fclose(fp);
	exit(1);
      }
    default:			/* normal */
      break;
    }
}

int main(int argc, char*argv[])
{
  int retval = EOK;

  print_gpl();

  if ((retval = init_ssa((argc>1?argv[1]:0))) < 0)
    {
      printf("[ERROR] MAIN->SSA: (%s)\n", error_codes_description[retval*(-1)]);
      destroy_ssa();
      return retval;
    }
  printf("Checking syntax/semantic... ");
  if ((retval = start_rcrsv_dscnt_prsr()) < 0)
    {
      printf("[ERROR] MAIN->SSA: %s\n", error_codes_description[retval*(-1)]); 
      destroy_ssa();
      /* return retval; */
    }

  printf("%d error(s) found.\n",g_errors);

  if (g_errors)
    {
      printf("No code generated.\n");
      return 0;
    }

  unlink(LISTING_FILE);
  unlink(SYNTX_ERR_FILE);
  write_symtbl();
  write_quad_table();

  if ((retval = init_codegen()) < 0)
    {
      printf("[ERROR] MAIN->CODEGEN: (%s)\n", error_codes_description[retval*(-1)]);
      destroy_codegen();
      destroy_ssa();
      return retval;
    }

  printf("Generating code... ");
  if ((retval = generate_code()) < 0)
    {
      printf("failed!\n");
      printf("[ERRRO] MAIN->CODEGEN: (%s)\n", error_codes_description[retval*(-1)]);
      destroy_codegen();
      destroy_ssa();
      return retval;
    }

  write_generated_code("prog.dat");
  printf("done.\n");

  destroy_codegen();
  destroy_ssa();

  return EOK;
}
