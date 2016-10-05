%{
#include <string.h>
#include "alfred_types.h"
#include "y.tab.h"
#include "sim.h"

int lineno = 0; 
char linebuf[512];
%}

%s ket

BIT [0-1]
DIG [0-9]
FLOAT [0-9]+\.[0-9]*
IDEN [A-Za-z]+ 

%% 
\|            { BEGIN(ket); } 
<ket>\)       { BEGIN(INITIAL);    }
<ket>{BIT}+   { yylval.inum = convert_label(yytext); return KET; } 
{DIG}+        { yylval.inum = atoi(yytext); return INUM; } 
{FLOAT}       { yylval.fnum = atof(yytext); return FNUM; } 
N             { return NBITS; } 
G             { return GATE; } 
M             { return MEASURE; } 
i             { return CPLXI; } 
show          { return SHOW; } 
print         { return PRINT; } 
normalize     { return NORMALIZE; } 
set           { return SET; } 
erase         { return ERASE; } 
[ \t]              ;
[\n\r]            { strcpy(linebuf,yytext+1); lineno++; yyless(1); } 
[A-Za-z]+     { yylval.symp = symlook(yytext); return NAME; }
\"[^\"]+\"    { yytext[yyleng-1]=0; yylval.strg = yytext+1; return STRING; } 
#.*           { /* ignore comment */ } 
.             { return yytext[0]; } 
%%

void yyerror(char *s) { 
  printf("line %d: %s at %s in this line\n%s\n",lineno,s,yytext,linebuf);
};


struct symtab *symlook(char *s) { 
  char *p; 
  struct symtab *sp;
  
  for(sp = symtab; sp < &symtab[NSYMS]; sp++) { 
    /* name already contained in table? */
    if(sp->name && !strcmp(sp->name, s))
      return sp;
    /* is the name field free? */ 
    if(!sp->name) { 
      sp->name = strdup(s);
      return sp;
    }
  }
  yyerror("Sorry, too many symbols in symtab");
  exit(1);
}



