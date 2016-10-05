/* You might have to adapt the following list of include files 
 * on your system. For example, windows.h is only useful on a 
 * Windows environment
 */ 

%{
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
  /* #include <windows.h> */ 
#include "alfred_types.h"
#include "sim.h"
%}

/* yylval is a union, which is specified as follows: 
 * it might be an integer, double, or complex number;
 * a condition combining ocnd and icnd in one struct
 * a matrix containing four complex entries
 * a gate specified by ocnd, icnd, pos, and matrix
 * or an abbreviation of a matrix to be added in the 
 * symbol table
 */ 

%union {
  int    inum;
  double fnum;
  cplx   cnum;
  char * strg;
  cond   cond;
  cplx   mat[4];
  gate   gate;
  struct symtab *symp;
}

/* Definitions of tokens, see alfred.l for details */ 

%token <inum> INUM NBITS 
%token <fnum> FNUM
%token <inum> KET
%token <symp> NAME
%token <strg> STRING
%token SHOW PRINT NORMALIZE GATE MEASURE SET ERASE PRINT CPLXI

/* You need to study these types well to solve the task */ 

%type <cnum> cplx
%type <fnum> real sreal imag simag
%type <inum> gatepos 
%type <cond> conditions cond
%type <mat>  matrix
%type <gate> gate gatebody

%%

/* An Alfred program is given by a sequence of statements.
 * Two statements can be separated by a semicolon, but 
 * this is optional. 
 */ 

statements: statement
          | statements statement
          ;

statement: assign    { ; } 
         | gate      { ; } 
         | show      { ; }
         | print     { ; }  
         | normalize { ; } 
         | measure   { ; }
         | set       { ; }
         | erase     { ; }
         | istate    { ; } 
         ; 

normalize: NORMALIZE { normalize_state(); }; 

measure: MEASURE '(' INUM ')' { measure_state(1<<$3, state); } ;

set: SET '(' INUM ')' { set_bit( 1<<$3, state ); };

erase: ERASE '(' INUM ')' { erase_bit( 1<<$3, state); };  

istate: { clear_state(); } state
      ;
state: KET                { state[$1].re =    1.0; state[$1].im =  0.0;   }
     | state '+' KET      { state[$3].re =    1.0; state[$3].im =  0.0;   }
     | state '+' cplx KET { state[$4].re =  $3.re; state[$4].im =  $3.im; }
     | state '-' cplx KET { state[$4].re = -$3.re; state[$4].im = -$3.im; } 
     ; 
show: SHOW { print_state(); }; 

print: PRINT STRING { printf("%s\n",$2); };
   
assign: NBITS '=' INUM 
        { state = resize_state($3, state); } 
      | NAME '=' matrix 
        { $1->mat[0] = $3[0]; $1->mat[1] = $3[1];  
	  $1->mat[2] = $3[2]; $1->mat[3] = $3[3]; 
        } 
      ;

gate: GATE '(' gatebody ')' 
      { applygate(Nbits, $3.ocnd,$3.icnd, 
                  $3.pos, $3.mat, state); 
      } 
    ;
gatebody: gatepos ',' matrix 
          { $$.ocnd = 0; $$.icnd = 0; $$.pos = $1; 
            $$.mat[0]=$3[0]; $$.mat[1]=$3[1]; 
            $$.mat[2]=$3[2]; $$.mat[3]=$3[3]; } 
        | '[' conditions ']' ',' gatepos ',' matrix 
          { $$.ocnd = $2.ocnd; $$.icnd = $2.icnd; $$.pos = $5; 
	    $$.mat[0]=$7[0]; $$.mat[1]=$7[1]; 
            $$.mat[2]=$7[2]; $$.mat[3]=$7[3]; }
        ;
conditions: cond 
            { $$ = $1; } 
          | conditions ',' cond 
            { $$.ocnd = $1.ocnd + $3.ocnd; 
              $$.icnd = $1.icnd + $3.icnd; 
            }
          ;
cond: INUM     { $$.icnd = 1<<$1; $$.ocnd = 0; }  
    | '-' INUM { $$.icnd = 0; $$.ocnd = 1<<$2; } 
    ;
gatepos: INUM  { $$ = 1<<$1; } 
       ;
matrix: '[' cplx ','  cplx ','  cplx ','  cplx ']' 
         { $$[0] = $2; $$[1] = $4; $$[2] = $6; $$[3] = $8; } 
      | NAME 
	{ $$[0] = $1->mat[0]; $$[1] = $1->mat[1];  
	  $$[2] = $1->mat[2]; $$[3] = $1->mat[3]; }   
      ;

/* Complex numbers in Alfred.
 * The definition is narrowed than you might expect.  
 */ 
 
cplx: sreal                  { $$.re = $1; $$.im =   0; } 
    | simag                  { $$.re = 0;  $$.im =  $1; } 
    | sreal '+' imag         { $$.re = $1; $$.im =  $3; } 
    | sreal '-' imag         { $$.re = $1; $$.im = -$3; } 
    | '(' sreal '+' imag ')' { $$.re = $2; $$.im =  $4; } 
    | '(' sreal '-' imag ')' { $$.re = $2; $$.im = -$4; } 
    ;

sreal: real     { $$ =  $1; } 
     | '-' real { $$ = -$2; } 
     ;
real: INUM { $$ = (double)$1; } 
    | FNUM { $$ = $1; } 
    ; 
simag: imag     { $$ =  $1; } 
     | '-' imag { $$ = -$2; }  
     ;
imag: CPLXI               { $$ = 1.0; }
    | CPLXI '*' real      { $$ =  $3; }
    ; 

%%


int main(int argc, char *argv[]) { 
  unsigned int Seed;

  srand48(clock());

  state = init_state(2);
  state[0] = (cplx){1.0,0.0};  
  yyparse();
  return 0;
}





