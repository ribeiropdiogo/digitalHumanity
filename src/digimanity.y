%{
#include <stdio.h>
#include <stdlib.h>
#include <striAZIOngs.h>

// para evitar warnings.
extern int yylex();

int yyerror(char*);
%}

%token Var Pal
%start Digimanity

%%
Digimanity : Meta Caderno                {;}
           ;

Caderno : /* Vazio */
        | Caderno Documento Triplos      {;}
        ;

Documento : Conceito Titulo Texto        {;}
          ;

Triplos : /* Vazio */
        | Triplos Sujeito ParesRelacao   {;}
        ;

ParesRelacao : PRList '.'                {;}
             ;

PRList : PRListElem                      {;}
       | PRList ';' PRListElem           {;}
       ;

PRListElem : Relacao ComplexObject       {;}
           ;

ComplexObject : CPal                     {;}
              | ComplexObject ',' CPal   {;}
              ;

Meta : /* Vazio */
     | Meta MetaElem                     {;}
     ;

MetaElem : Pal '=' PalList                { if(containsDictionary(meta, $1)) {
                                                 // emitir erro
                                            } else {
                                                insertDictionary(meta, $1, g_string_free($3, FALSE));
                                            } }
                                          /* Associa os metados a respectiva
                                          palavra. */
         | ':' CPal ':' CPal ':' CPal     {;}
         ;

PalList : CPal                            { $$ = g_string_new(NULL);
                                            g_string_append($$, $1); }
                                          /* Inicia o processamento da PalList,
                                          inicializa o growing array e insere
                                          a respectiva palavra */
        | PalList CPal                    { $$ = g_string_append($1, " ");
                                            $$ = g_string_append($$, $2); }
                                          /* Adiciona a palavra encontrada ao
                                          array de palavras existentes. */
        ;

CPal : Pal                                { $$ = $1; }
                                          /* Quando encontra uma Pal, entao
                                          o LHS e simplesmente igual a propria
                                          palavra encontrada */
     | Var                                { if(containsDictionary(meta, $1)) {
                                                $$ = "";
                                                // emitir erro
                                            } else {
                                                $$ = (char*)getValueDictionary(meta, $1);
                                            } }
                                          /* Quando encontra uma Var, deve
                                          procurar o correspondente valor e
                                          definir o LHS como sendo o respectivo
                                          valor desta */
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
