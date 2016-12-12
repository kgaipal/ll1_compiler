# ll1_compiler
LL(1) compiler from academic work at Texas Tech University



Date  : April 29, 2010 
Author: Kshitij Gaipal
CS5353 Compiler Construction by Dr Daniel Cooke

====================================================================
                        CS5353 Compiler
====================================================================

Compile
-------
To compile the compiler, use following command: 

  $ make -B

This will generate the executable "ll1_compiler". 

Run ll1_compiler 
--------------
If no argument is provided as name of input source file, ll1_complier
will look for default file name : "sample_program.c" in current
directory. Use following command to run ll1_compiler:

  $ ./ll1_compiler <sample_program>

or use default :

  $ ./ll1_compiler

On any exception in mycompiler executable itself, a message will be 
printed on the screen and it will halt.

If any errors are found in input source file, no instructions/quads 
are generated and number of lexical/ssa errors will be printed on the 
console. For details you may have to look into "listing_file.txt" for
lexical errors and "syntax_errors.txt" for ssa errors.

If there are no errors in input source file, "prog.dat" will contain
generated code. The symbol table and quad table is also printed to
"symbol_table.txt" and "quad_table.txt" respectively only if there are
no error(s) in input source file.

Successfull Compilation
-----------------------
On 0 errors in input source file, "prog.dat" and "data.dat" are written.
Both are input for emulator (see below).

======================================================================
                           Emulator
======================================================================
The instruction for compliment (COP) is removed and a new opcode is 
added instead to load the memory address into register - LAD.

Emulator's Pascal function 'write' has one extra function call to write
space on console to saperate continous write instruction. Rest of the
emulator code is same as given by Dr Cooke originally.

Mnemonic: LAD R1, R2, offset. 

To compile the emulator, use following command: 

  $ gpc -o emulator EMULATOR.pas
