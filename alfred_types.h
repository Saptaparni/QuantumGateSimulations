typedef struct { 
  int ocnd;
  int icnd;
} cond;

typedef struct { 
  double re;
  double im;
} cplx;

typedef struct { 
  int ocnd;
  int icnd; 
  int pos; 
  cplx mat[4];
} gate;

typedef cplx mat[4];

#define NSYMS 40

struct symtab { 
  char *name;
  cplx mat[4];
} symtab[NSYMS];

extern int yylex(void);
extern int yyparse(void);
extern void yyerror(char *);
extern struct symtab *symlook(char *);
