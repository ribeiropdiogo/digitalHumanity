%{
#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include "Dictionary.h"
#include "Manager.h"

// para evitar warnings.
extern int yylex();
int yyerror(char*);

// Dados globais do sistema
Dictionary meta = NULL;
%}

/* Definicao dos dados de yylval*/
%union {
        int count;              /* Conta o numero de = */
        char *word;             /* Para palavras, simples e complexas */
        GString *plist;         /* Para listas de palavras */
}

/* Define ponto de entrada da GIC */
%start Digimanity

/* Define a tipagem de todos os terminais */
%token <word> Var Pal
%token <count> TitleTab
%token MetaTag TituloTag TriplosTag Paragrafo

/* Define a tipagem de todos os nao-terminais */
%type <plist> PalList
%type <word> CPal

%%
Digimanity : Meta Caderno                {;}
           ;

Meta : TitleTab MetaTag MetaList             {;}
                                         /* Encontrou a meta tag, Por isso,
                                         processa toda a metalist. */
     ;

Caderno : /* Vazio */
        | Caderno Documento TriplosSec   {;}
        ;

Documento : Conceito Titulo Texto        {;}
          ;

Conceito : TitleTab CPal                 {;}
                                         /* Começou um novo conceito. */
         ;

Titulo : TituloTag ':' PalList           {;}
                                         /* Definicao do titulo per si. */
       ;

Texto : /* Vazio */
      | Texto PalList Paragrafo
      ;

TriplosSec : TriplosTag ':' Triplos      {;}
           ;

Triplos : /* Vazio */
        | Triplos Sujeito ParesRelacao   {;}
        ;

Sujeito : CPal                           {;}
        ;

ParesRelacao : PRList '.'                {;}
             ;

PRList : PRListElem                      {;}
       | PRList ';' PRListElem           {;}
       ;

PRListElem : Relacao ComplexObject       {;}
           ;

Relacao : CPal                           {;}
        ;

ComplexObject : CPal                     {;}
              | ComplexObject ',' CPal   {;}
              ;

MetaList : /* Vazio */
     | MetaList MetaElem                     {;}
     ;

MetaElem : Pal '=' PalList                { if(containsDictionary(meta, $1)) {
                                                 // emitir erro
                                            } else {
                                                insertDictionary(meta, $1, g_string_free($3, FALSE));
                                            } }
                                          /* Associa os metados a respectiva
                                          palavra. */

         ;

PalList : CPal                     {;}
        | PalList CPal         { $$ = g_string_append($1, " ");
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

#include "../lex.yy.c"

int yyerror(char *s)
{
	fprintf(stderr, "ERRO: %s \n", s);
	return 0;
}

int main(int argc, char **argv)
{
      // Inicializa dados globais
      meta = makeDictionary(NULL);

      yyparse();

      // Limpar a memoria associada ao programa

      // Libertar o dicionario
      destroyDictionary(meta);

	return 0;
}
