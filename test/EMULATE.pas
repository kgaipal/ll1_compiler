{ This file is part of ll1_compiler. ll1_compiler is the LL(1) Compiler project }
{ for Compiler Construction class of spring 2010 (CS5353) at Texas Tech University, }
{ Lubbock (Instructor: Dr Daniel Cooke). }

{ Copyright (C) 2010  Kshitij Gaipal (kgaipal@gmail.com) and Dr. Daniel Cooke (daniel.cooke@ttu.edu) }

{ ll1_compiler is free software: you can redistribute it and/or modify }
{ it under the terms of the GNU General Public License as published by }
{ the Free Software Foundation, either version 3 of the License, or }
{ (at your option) any later version. }

{ ll1_compiler is distributed in the hope that it will be useful, }
{ but WITHOUT ANY WARRANTY; without even the implied warranty of }
{ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the }
{ GNU General Public License for more details. }

{ You should have received a copy of the GNU General Public License }
{ along with ll1_compiler.  If not, see <http://www.gnu.org/licenses/>. }   

program test(prog,data,out,output);
{*  size of memory is normally 1024                     *}
const s=1023;
var data,prog,out:text;
   p:array[0..s]of longint;
   reg:array[0..7]of integer;
   i,pc,n,sign:integer;
   halt:boolean;

procedure intake(var n:integer);
begin
   for i:=0 to 7 do
      reg[i]:=0;
   n:=-1;
   assign(prog,'prog.dat');
   reset(prog);
   while (not eof(prog)) and (n <= s) do
   begin
      n:=n+1;
      readln(prog,p[n])
   end
end;

function opcode(i:integer):longint;
begin
   opcode := p[i] div 65536
end;

function rop1(i:integer):longint;
var opcodet:longint;
begin
   opcodet := p[i] div 65536;
   rop1 := (p[i]-(opcodet*65536)) div 8192
end;

function rop2(i:integer):longint;
var opcodet,rop1t:longint;
begin
   opcodet := p[i] div 65536;
   rop1t := (p[i]-(opcodet*65536)) div 8192;
   rop2 := (p[i]-(opcodet*65536)-(rop1t*8192)) div 1024
end;

function mop(i:integer):longint;
var opcodet,rop1t,rop2t:longint;
begin
   opcodet := p[i] div 65536;
   rop1t := (p[i]-(opcodet*65536)) div 8192;
   rop2t := (p[i]-(opcodet*65536)-(rop1t*8192)) div 1024;
   mop := p[i]-(rop1t*8192)-(rop2t*1024)-(opcodet*65536)
end;

procedure load(pc:integer);
var r1,r2,m:integer;
begin
   r1:=rop1(pc);
   r2:=rop2(pc);
   m:=mop(pc)+reg[r2];
   reg[r1]:=p[m]
end;

procedure load_addr(pc:integer);
var r1,r2,m:integer;
begin
   r1:=rop1(pc);
   r2:=rop2(pc);
   m:=mop(pc)+reg[r2];
   reg[r1]:=m
end; { load_addr }

procedure store(pc:integer);
var r1,r2,m:integer;
begin
   r1:=rop1(pc);
   r2:=rop2(pc);
   m:=mop(pc) + reg[r2];
   p[m]:=reg[r1]
end;

procedure sub(pc:integer);
var r1,r2:integer;
begin
   r1:=rop1(pc);
   r2:=rop2(pc);
   reg[r1]:=reg[r1]-reg[r2];

   if reg[r1] < 0 then
      sign:=-1
   else
      if reg[r1] = 0 then
	 sign:=0
      else
	 sign:=1
end;

procedure add(pc:integer);
var r1,r2:integer;
begin
   r1:=rop1(pc);
   r2:=rop2(pc);
   reg[r1]:=reg[r1]+reg[r2]
end;

procedure dv(pc:integer);
var r1,r2:integer;
begin
   r1:=rop1(pc);
   r2:=rop2(pc);
   reg[r1]:=reg[r1] div reg[r2]
end;

procedure mul(pc:integer);
var r1,r2:integer;
begin
   r1:=rop1(pc);
   r2:=rop2(pc);
   reg[r1]:=reg[r1] * reg[r2]
end;

procedure beq(i:integer);
var r1:integer;
begin
   r1:=rop1(pc);
   if sign = reg[r1] then
      pc:=mop(i)-1
end;

procedure bne(i:integer);
var r1:integer;
begin
   r1:=rop1(pc);
   if sign <> reg[r1] then
      pc:=mop(i)-1
end;

procedure b(i:integer);
begin
   pc:=mop(i)-1
end;

procedure ini(pc:integer);
var
   r:integer;
begin
   r:=rop1(pc);
   read(data,reg[r])
end;

procedure cap(pc:integer);
var
   r:integer;
begin
   r:=rop1(pc);
   reg[r]:=sign
end;

procedure comp(pc:integer);
var
   r:integer;
begin
   r:=rop1(pc);
   reg[r]:=-reg[r]
end;

procedure oi(pc:integer);
var
   r:integer;
begin
   r:=rop1(pc);
   write(reg[r]);
   write(' ')
end;

begin
   assign(data,'data.dat');
   reset(data);
   sign:=0;
   intake(n);
   pc:=-1;
   halt:=false;
   while  not(halt) do
   begin
      pc:=pc+1;

      if (opcode(pc)>=0) and (opcode(pc)<=15) then
	 case opcode(pc) of
	   0  : load(pc);
	   1  : store(pc);
	   2  : sub(pc);
	   3  : add(pc);
	   4  : dv(pc);
	   5  : mul(pc);
	   6  : beq(pc);
	   7  : bne(pc);
	   8  : b(pc);
	   9  : load_addr(pc);
	   10 : ini(pc);
	   11 : cap(pc);
	   12 : oi(pc);
	   13 : readln(data);
	   14 : writeln;
	   15 : halt:=true;
	 end
	 else
	    writeln('error:  invalid opcode at pc=',pc);
   end
end.
