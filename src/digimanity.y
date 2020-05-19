%{
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// para evitar warnings.
extern int yylex();

int yyerror(char*);
%}

%token num doispontos
%start Haskell

%%
Haskell : Haskell Lista
        | Lista
        ;

Lista : '[' LNums ']'
      | '['']'
      ;

LNums : LNums ',' Fator
      | Fator
      ;

Fator : num
      | Lista
      | num doispontos num
      ;
%%

#include "lex.yy.c"

int yyerror(char *s)
{
	fprintf(stderr, "ERRO: %s \n", s);
	return 0;
}

int main()
{
	yyparse();
	return 0;
}
