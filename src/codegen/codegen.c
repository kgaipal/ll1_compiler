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

#include <codegen.h>
#include <defines.h>
#include <symbol_table.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <quad.h>

#define BINARY_SIZE 1024

bool _codegen_initialized = false;
int _binary[BINARY_SIZE];	/* final output of this compiler: the program binary */
int _data_section_offset;
int _pc;			/* program counter: where are we right now in writing instruction codes */

#define QP_STCK 1024
struct qp			/* Quad - Program counter stack */
{
  int quad;
  int pc;
};

struct qp _qp_stck[QP_STCK];
int _tos_qp = -1;

void push_qp(struct qp item);
struct qp pop_qp();
bool empty_qp();

int calculate_data_section_offset();
int memloc_tokens();

void gencode_not(int quad_pos);
void gencode_and(int quad_pos);
void gencode_or(int quad_pos);
void gencode_read(int quad_pos);
void gencode_write(int quad_pos);
void gencode_readln(int quad_pos);
void gencode_writeln(int quad_pos);
void gencode_assign(int quad_pos);
void gencode_exp(int quad_pos);
void gencode_div(int quad_pos);
void gencode_mult(int quad_pos);
void gencode_add(int quad_pos);
void gencode_sub(int quad_pos);
void gencode_lessthan(int quad_pos);
void gencode_grtrthan(int quad_pos);
void gencode_equal(int quad_pos);
void gencode_jmpfalse(int quad_pos);
void gencode_jmp(int quad_pos);
void gencode_varaddr(int quad_pos);
void gencode_halt(int quad_pos);


int init_codegen()
{
  if (_codegen_initialized)
    return EOK;

  _data_section_offset = calculate_data_section_offset();

  if (_data_section_offset < 0)
    return _data_section_offset;
 
  if (_data_section_offset <= 0)
    {
      printf("FATAL: Data section cannot be at begining");
      exit(EDATA_SECTION_CALC_FAILED);
    }

  if ((BINARY_SIZE - _data_section_offset - 1) <= 0)
    {
      printf("FATAL: Too large data section, %d is small for program binary", BINARY_SIZE);
      exit(EDATA_SECTION_CALC_FAILED);
    }

  _data_section_offset = BINARY_SIZE - _data_section_offset - 1;

  memset(_binary, 0, BINARY_SIZE);
  _pc = -1;
  _codegen_initialized = true;

  return EOK;
}

void destroy_codegen()
{
  _data_section_offset = -1;
  memset(_binary, 0, BINARY_SIZE);
  _pc = -1;
  _codegen_initialized = false;
}

int generate_code()
{
  int retval = EOK;
  int i = _pc+1;
  struct qp item;
  if (!_codegen_initialized)
    if ((retval = init_codegen()) < 0)
      return retval;

  if ((retval = memloc_tokens()) < 0)
    return retval;

  /* set 0 in reg 7 for comparison and make sure no body ever over writes it! */
  _binary[++_pc] = CODEGEN_SUB + R1_7 + R2_7;

  /* set 1 in reg 6 for comparison and make sure no body ever over writes it! */
  _binary[++_pc] = CODEGEN_LOD + R1_6 + R2_7 + _data_section_offset;

  for (i = 0; i < QUADTBL_SIZE; i++)
    {
      /* check if the next instruction is going to overwrite data section? */
      if (_pc+1 == _data_section_offset)
	{
	  printf("[FATAL] CODEGEN: Cannot overwrite data section with instruction! Aborting.\n \
                 Program counter at %d\n Data section starts: %d", _pc+1, _data_section_offset);
	  exit(-1);
	}

      switch(_quad_table[i].op)
	{
	case SYMTBL_TKNTYP_NOT:
	  {
	    gencode_not(i);
	    break;
	  }
	case SYMTBL_TKNTYP_AND:
	  {
	    gencode_and(i);
	    break;
	  }
	case SYMTBL_TKNTYP_OR:
	  {
	    gencode_or(i);
	    break;
	  }
	case SYMTBL_TKNTYP_READ:
	  {
	    gencode_read(i);
	    break;
	  }
	case SYMTBL_TKNTYP_WRITE:
	  {
	    gencode_write(i);
	    break;
	  }
	case SYMTBL_TKNTYP_READLN:
	  {
	    gencode_readln(i);
	    break;
	  }
	case SYMTBL_TKNTYP_WRITELN:
	  {
	    gencode_writeln(i);
	    break;
	  }
	case SYMTBL_TKNTYP_ASSIGN:
	  {
	    gencode_assign(i);
	    break;
	  }
	case SYMTBL_TKNTYP_EXP:
	  {
	    gencode_exp(i);
	    break;
	  }
	case SYMTBL_TKNTYP_DIV:
	  {
	    gencode_div(i);
	    break;
	  }
	case SYMTBL_TKNTYP_MULT:
	  {
	    gencode_mult(i);
	    break;
	  }
	case SYMTBL_TKNTYP_ADD:
	  {
	    gencode_add(i);
	    break;
	  }
	case SYMTBL_TKNTYP_SUB:
	  {
	    gencode_sub(i);
	    break;
	  }
	case SYMTBL_TKNTYP_LESSTHAN:
	  {
	    gencode_lessthan(i);
	    break;
	  }
	case SYMTBL_TKNTYP_GRTRTHAN:
	  {
	    gencode_grtrthan(i);
	    break;
	  }
	case SYMTBL_TKNTYP_EQUAL:
	  {
	    gencode_equal(i);
	    break;
	  }
	case QUADTBL_JMPFALSE:
	  {
	    gencode_jmpfalse(i);
	    break;
	  }
	case QUADTBL_JMP:
	  {
	    gencode_jmp(i);
	    break;
	  }
	case QUADTBL_VARADDR:
	  {
	    gencode_varaddr(i);
	    break;
	  }
	default:
	  {
	    /* bail out for any undefined operator */
	    gencode_halt(i);

	    while (!empty_qp())
	      {
		item = pop_qp();
		_binary[item.pc] = _binary[item.pc]|_quad_table[item.quad].pc;

		/* 'item' is actually the JMPFALSE/JMP instruction;
		   update its 3rd argument in '_binary[]' */

		/* first instruction for this quad 'i' is the place where we jump
		   if the JMPFALSE/JMP condition (='item') fails! */
	      }
	    return EOK;
	  }
	}
    }

  return EOK;
}

/** Data section in '_binary' is pushed to bottom of the array.
 * 
 *  Return Value: offset in the '_binary' where the data section
 *  declaration of all variable(s) and/or constant(s)) begins.
 *  Returns EOK on success otherwise negative number as error
 *  code.
 */
int calculate_data_section_offset()
{
  int i = 0;
  int ids = 0, cons = 0;

  /* find out how many identifiers and constants are in symbol table */
  for (i = SYMTBL_RSRVD_TKNS; i < SYMTBL_ROWS ; i++)
    {
      if (g_symtbl[i].token_type == SYMTBL_TKNTYP_ID)
	{
	  ids++;
	  if (g_symtbl[i].dim != 0) /* count array indices also */
	    ids += strtol(g_symtbl[g_symtbl[i].dim->size_subscript].name, 0, 0) - 1;
	}

      else if (g_symtbl[i].token_type == SYMTBL_TKNTYP_CONS)
	cons++;
    }

  return ids+cons;
}

/** Assign the memory locations to identifiers and constants
 *  (updates symbol table entries).
 *
 *  Returns EOK on success or error code on any error.
 */
int memloc_tokens()
{
  int i = 0;
  int memloc = _data_section_offset;

  for (i = SYMTBL_RSRVD_TKNS; i < SYMTBL_ROWS ; i++)
    {
      if (g_symtbl[i].token_type == SYMTBL_TKNTYP_ID)
	{
	  g_symtbl[i].mem_loc = memloc++;

	  if (g_symtbl[i].dim != 0) /* count array indices also */
	    memloc += strtol(g_symtbl[g_symtbl[i].dim->size_subscript].name, 0, 0) - 1;
	}
      else if (g_symtbl[i].token_type == SYMTBL_TKNTYP_CONS)
	{
	  _binary[memloc] = strtol(g_symtbl[i].name,0,0); /* constants in data section */
	  g_symtbl[i].mem_loc = memloc++;
	}
    }

  return EOK;
}

int write_generated_code(const char* src)
{
  int retval = EOK;
  FILE * fp = 0;
  int i = 0;

  /* dont overdo stuff */
  if (!_codegen_initialized)
    if ((retval = init_codegen()) < 0)
      return retval;

  if (src == 0)
    {
      printf("[INFO] CODEGEN: No output file specified, defaulting to \"prog.dat\"\n");
      fp = fopen("prog.dat", "w");
    }
  else
    fp = fopen(src, "w");


  if (!fp)
    {
      printf("[ERROR] CODEGEN: \"%s\" : No such file or directory\n", (src==0?"prog.dat":src));
      return EFILE_IO;
    }

  for (i = 0; i < BINARY_SIZE; i++)
    fprintf(fp, "%d\n", _binary[i]);

  fclose(fp);

  return i;
}

void gencode_not(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_7;                        /* reg 0: result may be 0 or +1 */
  _binary[++_pc] = CODEGEN_BEQ + R1_7 + (_pc+3);                     /* test if sign is 0? */
  _binary[++_pc] = CODEGEN_STO + R1_7 + R2_7 + g_symtbl[res].mem_loc;/* no; set reg 0 false(=0) */
  _binary[++_pc] = CODEGEN_BRN + (_pc+2);
  _binary[++_pc] = CODEGEN_STO + R1_6 + R2_7 + g_symtbl[res].mem_loc;/* yes; set reg 0 true(=1) */
}

void gencode_and(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc; /* load and test left side first */
  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_7;                         /* reg 0: result may be 0 or +1 */
  _binary[++_pc] = CODEGEN_BEQ + R1_7 + (_pc+6);                      /* test if sign is 0? */
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op2].mem_loc; /* no; test right side */
  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_7;                         /* reg 0: result may be 0 or +1 */
  _binary[++_pc] = CODEGEN_BEQ + R1_7 + (_pc+3);                      /* test if sign is 0? */
  _binary[++_pc] = CODEGEN_STO + R1_6 + R2_7 + g_symtbl[res].mem_loc; /* no; set reg 0 true(=1);
									 both left and right side were true */
  _binary[++_pc] = CODEGEN_BRN + (_pc+2);
  _binary[++_pc] = CODEGEN_STO + R1_7 + R2_7 + g_symtbl[res].mem_loc; /* yes; set reg 0 false(=0) */
}

void gencode_or(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc; /* load and test left side first */
  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_7;                         /* reg 0: result may be 0 or +1 */
  _binary[++_pc] = CODEGEN_BEQ + R1_7 + (_pc+3);                      /* test if sign is 0? */
  _binary[++_pc] = CODEGEN_STO + R1_6 + R2_7 + g_symtbl[res].mem_loc; /* no; set reg 0 true(=1) */
  _binary[++_pc] = CODEGEN_BRN + (_pc+7);

  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op2].mem_loc; /* yes; test right side */
  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_7;                         /* reg 0: result may be 0 or +1 */
  _binary[++_pc] = CODEGEN_BEQ + R1_7 + (_pc+3);                      /* test if sign is 0? */
  _binary[++_pc] = CODEGEN_STO + R1_6 + R2_7 + g_symtbl[res].mem_loc; /* no; set reg 0 true(=1) */
  _binary[++_pc] = CODEGEN_BRN + (_pc+2);

  _binary[++_pc] = CODEGEN_STO + R1_7 + R2_7 + g_symtbl[res].mem_loc; /* yes; set reg 0 false(=0) */
}

void gencode_read(int quad_pos)
{
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_INI + R1_0 + R2_7;

  if (_quad_table[quad_pos].res_dref)
    {
      _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[res].mem_loc;
      _binary[++_pc] = CODEGEN_STO + R1_0 + R2_1 + 0;
    }
  else
    {
      _binary[++_pc] = CODEGEN_STO + R1_0 + R2_7 + g_symtbl[res].mem_loc;
    }
}

void gencode_write(int quad_pos)
{
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);

  if (_quad_table[quad_pos].res_dref)
    {
      _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[res].mem_loc;
      _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_1 + 0;
    }
  else
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[res].mem_loc;

  _binary[++_pc] = CODEGEN_OUI + R1_0;
}

void gencode_readln(int quad_pos)
{
  assign_pc(quad_pos, _pc+1);
  gencode_read(quad_pos);
}

void gencode_writeln(int quad_pos)
{
  assign_pc(quad_pos, _pc+1);
  gencode_write(quad_pos);
  _binary[++_pc] = CODEGEN_WRL + 0 + 0 + 0;
}

void gencode_assign(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);

  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0;

  if (_quad_table[quad_pos].res_dref)
    {
      _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[res].mem_loc;
      _binary[++_pc] = CODEGEN_STO + R1_0 + R2_1 + 0;
    }
  else
    {
      _binary[++_pc] = CODEGEN_STO + R1_0 + R2_7 + g_symtbl[res].mem_loc;
    }
}

void gencode_exp(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);

  /* multiply reg 0 with itself and store result in reg 3 */
  /* incrememnt counter */
  /* write reg 3 in 'res' */

  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[op2].mem_loc;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0;

  if (_quad_table[quad_pos].op2_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_1 + 0;

  _binary[++_pc] = CODEGEN_SUB + R1_2 + R2_2; /* result of EXP */
  _binary[++_pc] = CODEGEN_ADD + R1_2 + R2_6; /* set result of EXP to 1 initially */
  _binary[++_pc] = CODEGEN_SUB + R1_3 + R2_3; /* counter */

  _binary[++_pc] = CODEGEN_SUB + R1_4 + R2_4; /* temporary reg for calculation; it always will contain op2 before computation */
  _binary[++_pc] = CODEGEN_ADD + R1_4 + R2_1;
  _binary[++_pc] = CODEGEN_SUB + R1_4 + R2_3;
  _binary[++_pc] = CODEGEN_BEQ + R1_7 + (_pc+4); /* test sign, is it 0(=op2); yes, EXP computed; BRN to STO */

  _binary[++_pc] = CODEGEN_MUL + R1_2 + R2_0; /* now multiply */
  _binary[++_pc] = CODEGEN_ADD + R1_3 + R2_6; /* increment counter */
  _binary[++_pc] = CODEGEN_BRN + (_pc-6);

  _binary[++_pc] = CODEGEN_STO + R1_2 + R2_7 + g_symtbl[res].mem_loc; /* store result of EXP */
}

void gencode_div(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[op2].mem_loc;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0; /* last load for this op1 actually loaded address not value */

  if (_quad_table[quad_pos].op2_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_1 + 0; /* last load for this op1 actually loaded address not value */

  _binary[++_pc] = CODEGEN_DIV + R1_0 + R2_1;
  _binary[++_pc] = CODEGEN_STO + R1_0 + R2_7 + g_symtbl[res].mem_loc;
}

void gencode_mult(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[op2].mem_loc;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0; /* last load for this op1 actually loaded address not value */

  if (_quad_table[quad_pos].op2_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_1 + 0; /* last load for this op1 actually loaded address not value */

  _binary[++_pc] = CODEGEN_MUL + R1_0 + R2_1;
  _binary[++_pc] = CODEGEN_STO + R1_0 + R2_7 + g_symtbl[res].mem_loc;
}

void gencode_add(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);

  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[op2].mem_loc;
 
  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0; /* last load for this op1 actually loaded address not value */

  if (_quad_table[quad_pos].op2_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_1 + 0; /* last load for this op1 actually loaded address not value */

  _binary[++_pc] = CODEGEN_ADD + R1_0 + R2_1;
  _binary[++_pc] = CODEGEN_STO + R1_0 + R2_7 + g_symtbl[res].mem_loc;
}

void gencode_sub(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[op2].mem_loc;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0; /* last load for this op1 actually loaded address not value */

  if (_quad_table[quad_pos].op2_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_1 + 0; /* last load for this op1 actually loaded address not value */

  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_1;
  _binary[++_pc] = CODEGEN_STO + R1_0 + R2_7 + g_symtbl[res].mem_loc;
}

void gencode_lessthan(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[op2].mem_loc;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_1 + 0;

  _binary[++_pc] = CODEGEN_SUB + R1_1 + R2_0;                        /* reg 1: result may be -1 or 0 or +1 */
  _binary[++_pc] = CODEGEN_BEQ + R1_6 + (_pc+3);                     /* test if sign is +1? */
  _binary[++_pc] = CODEGEN_STO + R1_7 + R2_7 + g_symtbl[res].mem_loc;/* no; set reg 0 false(=0) */
  _binary[++_pc] = CODEGEN_BRN + (_pc+2);
  _binary[++_pc] = CODEGEN_STO + R1_6 + R2_7 + g_symtbl[res].mem_loc;/* yes; set reg 0 true(=1) */
}

void gencode_grtrthan(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[op2].mem_loc;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0;

  if (_quad_table[quad_pos].op2_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_1 + 0;

  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_1;                        /* reg 0: result may be -1 or 0 or +1 */
  _binary[++_pc] = CODEGEN_BEQ + R1_6 + (_pc+3);                     /* test if sign is +1? */
  _binary[++_pc] = CODEGEN_STO + R1_7 + R2_7 + g_symtbl[res].mem_loc;/* no; set reg 0 false(=0) */
  _binary[++_pc] = CODEGEN_BRN + (_pc+2);
  _binary[++_pc] = CODEGEN_STO + R1_6 + R2_7 + g_symtbl[res].mem_loc;/* yes; set reg 0 true(=1) */
}

void gencode_equal(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int op2 = _quad_table[quad_pos].op2;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);

  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_7 + g_symtbl[op2].mem_loc;

  if (_quad_table[quad_pos].op1_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_0 + 0;

  if (_quad_table[quad_pos].op2_dref)
    _binary[++_pc] = CODEGEN_LOD + R1_1 + R2_1 + 0;

  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_1; /* reg 0: result may be -1 or 0 or +1 */
  _binary[++_pc] = CODEGEN_BEQ + R1_7 + (_pc+3); /* test if sign is 0? */
  _binary[++_pc] = CODEGEN_STO + R1_7 + R2_7 + g_symtbl[res].mem_loc;/* no; set reg 0 false(=0) */
  _binary[++_pc] = CODEGEN_BRN + (_pc+2);
  _binary[++_pc] = CODEGEN_STO + R1_6 + R2_7 + g_symtbl[res].mem_loc;/* yes; set reg 0 true(=1) */
}

void gencode_jmpfalse(int quad_pos)
{
  struct qp item;
  int _pcx = 0;
  int op1 = _quad_table[quad_pos].op1;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_LOD + R1_0 + R2_7 + g_symtbl[op1].mem_loc;
  _binary[++_pc] = CODEGEN_SUB + R1_0 + R2_7; /* reg 0: result may be 0(false) or +1(true) */
  _binary[++_pc] = CODEGEN_BEQ + R1_7 + (_pcx); /* test sign is 0? yes: jump to _pcx */

  /* we have to rewrite last instruction later (3rd argument)
     for proper jump position in '_binary[]' */
  item.pc = _pc;
  item.quad = res;
  push_qp(item);

  /* if reg 0 is true(=1) follow whatever is followed after this */
}

void gencode_jmp(int quad_pos)
{
  struct qp item;
  int _pcx = 0;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_BRN + (_pcx); /* jump to _pcx */

  /* we have to rewrite last instruction later (3rd argument)
     for proper jump position in '_binary[]' */
  item.pc = _pc;
  item.quad = res;
  push_qp(item);
}

void gencode_varaddr(int quad_pos)
{
  int op1 = _quad_table[quad_pos].op1;
  int res = _quad_table[quad_pos].res;

  assign_pc(quad_pos, _pc+1);

  /* copy the base address of array in memory location of 'res' */
  _binary[++_pc] = CODEGEN_LAD + R1_0 + R2_7 + g_symtbl[op1].mem_loc ;
  _binary[++_pc] = CODEGEN_STO + R1_0 + R2_7 + g_symtbl[res].mem_loc;
}

void gencode_halt(int quad_pos)
{
  assign_pc(quad_pos, _pc+1);
  _binary[++_pc] = CODEGEN_HLT;
}

void push_qp(struct qp item)
{
  if (_tos_qp >= QP_STCK)
    {
      printf("FATAL: Instruction Stack overflow in CODEGEN! Aborting.");
      /* exit(ECODEGEN_INSTR_STCK_OVRFL); */
    }

  _qp_stck[++_tos_qp] = item;
}

struct qp pop_qp()
{
  if (_tos_qp < 0)
    {
      printf("FATAL: Instruction Stack underflow in CODEGEN! Aborting.");
      /* exit(ECODEGEN_INSTR_STCK_UNDRFL); */
    }

    return _qp_stck[_tos_qp--];
}

bool empty_qp()
{
  if (_tos_qp < 0)
    return true;
  else 
    return false;
}
