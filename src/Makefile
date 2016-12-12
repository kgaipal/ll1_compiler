# This file is part of ll1_compiler. ll1_compiler is the LL(1) Compiler project
# for Compiler Construction class of spring 2010 (CS5353) at Texas Tech University,
# Lubbock (Instructor: Dr Daniel Cooke).

# Copyright (C) 2010  Kshitij Gaipal (kgaipal@gmail.com)

# ll1_compiler is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# ll1_compiler is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with ll1_compiler.  If not, see <http://www.gnu.org/licenses/>.


CFLAGS = -ansi -I. -Iutil -Issa -Ilex -Iquad -Icodegen
CC = gcc -D_GNU_SOURCE

ll1_compiler: main.o
	$(CC) -lm *.o -o ll1_compiler

lexical_analyzer.o: util.o lex/lexical_analyzer.h lex/lexical_analyzer.c defines.h symbol_table.h
	$(CC) $(CFLAGS) -c lex/lexical_analyzer.c

util.o: util/util.h util/util.c defines.h symbol_table.h
	$(CC) $(CFLAGS) -c util/util.c

ssa.o: util.o lexical_analyzer.o quad.o ssa/ssa.h ssa/ssa.c defines.h symbol_table.h
	$(CC) $(CFLAGS) -c ssa/ssa.c

quad.o: quad/quad.h quad/quad.c
	$(CC) $(CFLAGS) -c quad/quad.c

codegen.o: codegen/codegen.h codegen/codegen.c
	$(CC) $(CFLAGS) -c codegen/codegen.c

main.o: util.o lexical_analyzer.o quad.o ssa.o codegen.o main.c 
	$(CC) $(CFLAGS) -c main.c
