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


mycompiler:
	@cd ../src; make
	@mv ../src/ll1_compiler .
	@cd ..
clean: 
	-find .. -name \*.o | xargs rm
	-rm ./ll1_compiler
	-rm ./prog.dat

cleanall: clean
	-find .. -name \*~ | xargs rm 
	-find .. -name \*.txt | xargs rm 

