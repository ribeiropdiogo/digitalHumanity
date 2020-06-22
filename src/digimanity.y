%{
#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include "Dictionary.h"
#include "Manager.h"

// para evitar warnings.
extern int yylex();
int yyerror(char*);

void free_array(gpointer data);
void foreach_par_relacao(gpointer key,
                gpointer value, gpointer user_data);

// Dados globais do sistema
Dictionary meta = NULL;
%}

/* Definicao dos dados de yylval*/
%union {
        int count;              /* Conta o numero de = */
        char *word;             /* Para palavras, simples e complexas */
        GString *plist;         /* Para listas de palavras */
        GArray *array;          /* Para ComplexObject */
        GHashTable *table;      /* Para PRList e ParesRelacao */
}

/* Define ponto de entrada da GIC */
%start Digimanity

/* Define a tipagem de todos os terminais */
%token <word> Var Pal
%token <count> TitleTab
%token MetaTag TituloTag TriplosTag Paragrafo Attrib

/* Define a tipagem de todos os nao-terminais */
%type <plist> PalList EndingPalList Texto Titulo
%type <word> CPal Conceito
%type <array> ComplexObject
%type <table> PRList ParesRelacao

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

Documento : Conceito Titulo Texto        { printf("#################################\n");
                                           printf("-> conceito: |%s|, titulo |%s|:\n", $1, g_string_free($2, FALSE));
                                           printf("%s", g_string_free($3,FALSE));}
          ;

Conceito : TitleTab CPal                 { $$ = $2;}
                                         /* Começou um novo conceito. */
         ;

Titulo : TituloTag EndingPalList              {$$ = $2;}
                                         /* Definicao do titulo per si. */
       ;

Texto : /* Vazio */                     {$$ = g_string_new(NULL);}
      | Texto EndingPalList             {$$ = $1;
                                         g_string_append_printf($$,
                                                 "<p>%s</p>\n",
                                                 g_string_free($2, FALSE));}
      ;

TriplosSec : TriplosTag Triplos      {;}
           ;

Triplos : /* Vazio */
        | Triplos CPal ParesRelacao   {g_hash_table_foreach($3, foreach_par_relacao, $2);
                                       g_hash_table_destroy($3);}
        ;

ParesRelacao : PRList '.'                {$$ = $1;}
             ;

PRList : CPal ComplexObject              {$$ = g_hash_table_new_full(g_str_hash,
                                                        g_str_equal, free, free_array);
                                          g_hash_table_insert($$, $1, $2);}
       | PRList ';' CPal ComplexObject   {g_hash_table_insert($1, $3, $4);
                                          $$ = $1;}
       ;

ComplexObject : CPal                     {$$ = g_array_new(FALSE, FALSE, sizeof(char*));
                                          $$ = g_array_append_val($$, $1);}
              | ComplexObject ',' CPal   {$$ = g_array_append_val($1, $3);}
              ;

MetaList : /* Vazio*/
     | MetaList MetaElem ';'                     {;}
     ;

MetaElem : Pal Attrib PalList                { if(containsDictionary(meta, $1)) {
                                                 // emitir erro
                                            } else {
                                                insertDictionary(meta, $1, g_string_free($3, FALSE));
                                            } }
                                          /* Associa os metados a respectiva
                                          palavra. */
         | CPal CPal CPal                 {;}

         ;

EndingPalList : PalList Paragrafo         {$$ = $1;}
              ;

PalList : CPal                            { $$ = g_string_new($1); }
        | PalList CPal                    { $$ = $1;
                                            g_string_append_printf($$, " %s", $2); }
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

void free_array(gpointer data) {
        g_array_free((GArray*)data, TRUE);

}

void foreach_par_relacao(gpointer key,
                gpointer value, gpointer user_data) {
        char *elem, *relacao = (char*)key, *sujeito = (char*)user_data;
        GArray *arr = (GArray*)value;
        guint i, size = arr->len;

        //printf("\tpara sujeito |%s|:\n", sujeito);
        //printf("\t\tpara a relacao |%s|:\n", relacao);
        for(i = 0; i < size; i++) {
                elem = g_array_index(arr, char*, i);
                //printf("\t\t\tobjeto |%s|:\n", elem);
        }
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
