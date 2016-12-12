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

#include <lexical_analyzer.h>
#include <defines.h>
#include <symbol_table.h>
#include <util.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

FILE *_srcfile_fp;
FILE *_listing_file_fp;
bool _lex_initialized = false;
char *_line;  /* global line buffer from input source file */
char *_pos;
const char _delim[] = " \n\r()[]<>;:,=^+-*/";
ssize_t _nchars;
char *_token;
bool comment_on = false;

void print_2_listing_file(const char* msg, const int len);
int read_token();
void eat_comments();
int readln();

/** init_lex: Initializes the lexical analyzer.
    returns 0 on success, otherwise negative integer.
*/
int init_lex(const char * src_file)
{
  int retval = EOK;

  /* dont overdo stuff */
  if (_lex_initialized)
    return EOK;

  if ((retval = initialize_symtbl()) < 0)
    return retval;

  if (src_file == 0)
    {
      printf("[WARNING] LEX: No input file given, defaulting to \"sample_program.c\"\n");
      _srcfile_fp = fopen("sample_program.c", "r");
    }
  else
    _srcfile_fp = fopen(src_file, "r");


  if (!_srcfile_fp)
    {
      printf("[ERROR] LEX: \"%s\" : No such file or directory\n", (src_file==0?"sample_program.c":src_file));
      return EFILE_IO;
    }

  unlink(LISTING_FILE);
  _listing_file_fp = fopen(LISTING_FILE, "a");

  if (!_listing_file_fp)
    {
      printf("[ERROR] LEX: \"%s\" : No such file or directory\n", LISTING_FILE);
      return EFILE_IO;
    }

  _lex_initialized = true;

  /* just read the first line in buffer */
  return readln();
}

void destroy_lex()
{
  if (_srcfile_fp)
    fclose(_srcfile_fp);

  if (_listing_file_fp)
    fclose(_listing_file_fp);

  _lex_initialized = false;
  _srcfile_fp = 0;
  _listing_file_fp = 0;
}

void print_2_listing_file(const char* msg, const int len)
{
  fprintf(_listing_file_fp, "%s\n", msg);
#ifdef _DEBUG
  printf("%s\n", msg);
#endif
}

/** next_token(): reads the next token from the input source file and returns it to the callee.
 *  SSA can call it to get the next token one by one. 
 *  returns error (negative number) if any otherwise update the token in its global variable
 */

int next_token()
{
  int tkn_len;
  static char err_msg[1024] = {0};

  g_token_subscript = EOF;
  g_token_type = EOF;
  tkn_len = EOF;

  /* ensure lex is initialized */
  if (!_lex_initialized)
    if ((tkn_len = init_lex(0)) < 0)
      return tkn_len;

  /* get next token and add it to symbol table size */
  tkn_len = read_token();

  if (tkn_len == 0)             /* this is absurd situation, but do check it */
    {
      printf("[ERROR] LEX: Token of length of zero read!\n\
	Details >n\
	Token     : %s\n\
	Length    : %d\n\
	Input line: %d\n\
	Aborting.\n", _token, tkn_len, g_input_line);

      exit(ELEX_ZERO_LENGTH_TKN_FND);
    }

  if (tkn_len == EOF)		/* end of file, no token found */
    {
      g_token_subscript = EOF;
      g_token_type = EOF;
      tkn_len = EOF;
    }
  else if (tkn_len < 0)			/* just to make sure we catch some other error too */
    {
      /* print error to listing file */
      memset(err_msg, 0, 1024);
      snprintf(err_msg, 1024, "[ERROR] LEX: line(%d), error code description(\"%s\")", g_input_line, error_codes_description[g_token_subscript*(-1)]);
      print_2_listing_file(err_msg, 80);

      g_token_type = g_token_subscript = tkn_len; /* propogate error code */
    }
  else
    {
      if (g_dec_section && is_id(_token))
	g_token_subscript = add_2_symtbl(_token, tkn_len);
      else if (is_cons(_token))
	g_token_subscript = add_2_symtbl(_token, tkn_len);
      else 
	g_token_subscript = find_in_symtbl(_token, tkn_len);

      if (g_token_subscript >= 0)
	g_token_type = g_symtbl[g_token_subscript].token_type;
      else
	{
	  /* print error to listing file */
	  if (g_token_subscript == ELEX_EXCSS_TKN_LNGTH || g_token_subscript == ELEX_INVALID_TKN)
	    {
	      g_token_type = SYMTBL_TKNTYP_ID;
	      g_errors++;

	      memset(err_msg, 0, 1024);
	      snprintf(err_msg, 1024, "[ERROR] LEX: line(%d), error description(\"%s\")",
		       g_input_line, error_codes_description[g_token_subscript*(-1)]);

	      print_2_listing_file(err_msg, 1024);
	    }
	  else
	    {
	      g_token_type = g_token_subscript; /* propogate error code */
	    }
	}
    }

  return tkn_len;
}

/** Reads token from input and return its length (even if its is zero length, which is very unlikely)
 *  If EOF is encountered, returns EOF.
 */
int read_token()
{
  char *line, *line2, *d;
  char *token;
  int tknlen = 0;

  if ((0 == *_pos)
      && (readln() == EOF))
    return EOF;

  line = _pos;

  while(isspace(*line))line++; /* eat white space, 
				 after this 'line' points to either a delim other than white space or
				 a token */

  if (0 == *line)  /* read new line again */
    {
      *_pos = 0;
      return read_token();
    }

  /* keep looking for the delimeters including whitespace */
  if (strchr(_delim,(int)(*line)))
    {/* 'line' points to delimeter other than white space */
      line2 = line + 1;

      /* special sequence check for ":=" */
      if (':' == (*line) && '=' == *(line+1))
	line2++;
    }
  else
    {
      line2 = line + 1;
      while(0 != *line2 && !strchr(_delim,(int)(*line2))) line2++;
    }

  /* find token length */
  for(tknlen = 0,d = line; d != line2; d++)
    tknlen++;

  /* realocate */
  if (_token) free(_token);

  _token = calloc(tknlen+1,1);
  token = _token;

  /* copy token */
  for(d = line; d != line2; d++, token++) 
    *token = *d;

  /* update '_pos' */
  while(isspace(*line2)) line2++;
  _pos = line2;

  return tknlen;
}


/* eat_comments: looks for the begining of the comments starting and ending like these
   comments themselves. Every content inside the comment section is filled to space.

   May update the '_line' and other related pointers.
*/

void eat_comments()
{
  char *line, *d;
  char *start, *end;

  line = _pos;
  start = strstr(line,"/*");
  end = strstr(line,"*/");

  if (0 != end && 0 != start && end < start && comment_on)
    start = 0;
  else if (0 != end && 0 != start && end < start && !comment_on)
    end = 0;

  /*no comment section found in this line */
  if (!comment_on && start == 0)return ;

  if (comment_on)
    start = line;

  if (0 != end)
    { /* comment section in this very line, easy job :) */
      d = start;
      while(d != end)
	{
	  *d = ' ';
	  d++;
	}
      *d = ' ';
      d++;
      *d = ' ';

      comment_on = false;
    }
  else
    {
      /* comment may end on some other line, eitherway replace with space till the end  */
      d = start;
      while(*d != 0)
	{
	  *d = ' ';
	  d++;
	}

      comment_on = true;
    }

  /* eat more in this line */
  if (strstr(line,"/*") != 0)
    eat_comments();
}

/** reads a new line from file and updates the global pointers for lex
 *  retruns false if any error or end of file.
 */
int readln()
{
  size_t n = 0;

  if (_line)
    free(_line);

  _line = 0;
  _nchars = getline(&_line, &n, _srcfile_fp);
  g_input_line++;

  if (_nchars <= 0)return EOF;

  /* _line[_nchars-1] = 0; /\* last character is newline, remove it  *\/ */
  /* _nchars--;  /\* _nchars now refers to the actual number of characters we will deal with *\/ */
  _pos = _line;

  /* check for comment section; advantage is that after this call characters 
     in comment section are replaced with whitespace which we can eat easily
     in subsequent token searches ! :) */
  eat_comments();

  return _nchars;
}

const char* get_token_str()
{
  return _token;
}
