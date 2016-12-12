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

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

/* Instruction layout:
   1. 'x' are ignored bits.
   2. Numerical indices are bit positions.

   ------------------------------------------------------------ 
   |xxxxxxxxxxxxxx|  opcode  |  R1  |  R2  |  memory offset   | 
   ------------------------------------------------------------ 
   31             20         16     13     10                  0

*/

/* opcodes */
#define	CODEGEN_LOD     0x00000	/* LOD r1,r2,offset */
#define	CODEGEN_STO     0x10000	/* STO r1,r2,offset */
#define	CODEGEN_SUB	0x20000	/* SUB r1,r2     */
#define	CODEGEN_ADD	0x30000	/* ADD r1,r2     */
#define	CODEGEN_DIV	0x40000	/* DIV r1,r2     */
#define	CODEGEN_MUL	0x50000	/* MUL r1,r2     */
#define	CODEGEN_BEQ	0x60000	/* BEQ r1,offset */
#define	CODEGEN_BNE	0x70000	/* BNE r1,offset */
#define	CODEGEN_BRN	0x80000	/* BRN offset    */
#define	CODEGEN_LAD	0x90000	/* LAD r1,r2,offset  */
#define	CODEGEN_INI	0xA0000	/* INI r1        */
#define	CODEGEN_CAP	0xB0000	/* CAP r1        */
#define	CODEGEN_OUI	0xC0000	/* OUI r1        */
#define	CODEGEN_RDL 	0xD0000	/* RDL           */
#define	CODEGEN_WRL 	0xE0000	/* WRL           */
#define	CODEGEN_HLT	0xF0000	/* HLT           */

/* Register location for R1 */
#define R1_0	0x0000		/* field R1 points to register 0 */
#define R1_1	0x2000
#define R1_2	0x4000
#define R1_3	0x6000
#define R1_4	0x8000
#define R1_5	0xA000
#define R1_6	0xC000
#define R1_7	0xE000		/* field R1 points to register 7 */

/* Register location for R2 */
#define R2_0	0x0000		/* field R2 points to register 0 */
#define R2_1	0x0400
#define R2_2	0x0800
#define R2_3	0x0C00
#define R2_4	0x1000
#define R2_5	0x1400
#define R2_6	0x1800
#define R2_7	0x1C00		/* field R2 points to register 7 */

/* never use register 0, its contents must remain ZERO always as our
   pascal emulator is using it for address computation in mop() */


int init_codegen();
void destroy_codegen();
int generate_code();
int write_generated_code(const char* src);

#endif /* _CODEGEN_H_ */
