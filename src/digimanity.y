%{
#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include "Dictionary.h"
#include "Manager.h"

// para evitar warnings.
extern char *file;
extern int yylex();
int yyerror(char*);

/* Função para libertar growing array utilizado internamente */
void free_array(gpointer data);

/* Função de travessia pelos pares de relação. */
void foreach_par_relacao(gpointer key,
                gpointer value, gpointer user_data);

// Dados globais do sistema
// Dicionario de meta variaveis.
Dictionary meta = NULL;

// Gestor principal do programa.
Manager man = NULL;
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
Digimanity : Meta Caderno               { ; }
                                        /* Definição mais abrangente da gramática.
                                        Composta por uma secção de metados e um caderno.
                                        A secção meta pode ser omitida, porém, isso irá
                                        originar um warning. */
           ;

Meta : TitleTab MetaTag MetaList        { ; }
                                        /* Definição da secção meta. Esta é um sinalizador
                                        de titulo, seguido pela keyword meta e uma lista
                                        de elementos que são definidos na metalist. Deve
                                        surgiu um warning se len(TitleTab) != 3. */
     ;

Caderno : /* Vazio */
        | Caderno Documento TriplosSec  { ; }
                                        /* Define o que é entendido como um caderno, sendo
                                        que este corresponde a um conjunto de documentos
                                        e a sua respectiva secção de triplos. */
        ;

Documento : Conceito Titulo Texto       { add_title(man, $1, g_string_free($2, FALSE));
                                          add_description(man, $1, g_string_free($3, FALSE)); }
                                        /* Um documento é um triplo de <conceito, titulo, texto>
                                        pelo que apos processado, é adicionado na estrutura principal.
                                        O texto corresponde a um texto semi-processado em HTML, separando
                                        as linhas por paragrafos. */
          ;

Conceito : TitleTab CPal                { $$ = $2; }
                                        /* Definição de um novo conceito, o conceito fica com
                                        o nome da produção correspondente. */
         ;

Titulo : TituloTag EndingPalList        { $$ = $2; }
                                        /* Definicao do titulo per si. */
       ;

Texto : /* Vazio */                     { $$ = g_string_new(NULL); }
                                        /* Um texto é um conjunto de zero ou mais conjuntos
                                        de palavras terminados por um paragrafos. Quando
                                        é feito um shift de texto vazio, é inicializado um
                                        buffer vazio. */
      | Texto EndingPalList             { $$ = $1;
                                          g_string_append_printf($$,
                                                 "<p>%s</p>\n",
                                                 g_string_free($2, FALSE)); }
                                        /* Quando detetado um conjunto de palavras terminado
                                        por um paragrafo, este é adicionado ao texto, com um
                                        pré-processamento que permite a separação por paragráfos
                                        no HTML. */
      ;

TriplosSec : TriplosTag Triplos         { ; }
                                        /* A secção de triplos corresponde à tag de triplos e
                                        os respectivos triplos que esta possui. */
           ;

Triplos : /* Vazio */                   { ; }
                                        /* O conjunto de triplos explicitado pode ser vazio. */
        | Triplos CPal ParesRelacao     { g_hash_table_foreach($3, foreach_par_relacao, $2);
                                          g_hash_table_destroy($3); }
                                        /* Cada triplo corresponde a um sujeito, seguido por,
                                        potencialmente, vários pares relação separados por ';' */
        ;

ParesRelacao : PRList '.'               { $$ = $1; }
                                        /* Os pares relação devem ser obrigatoriamente
                                        terminados pelo '.' */
             ;

PRList : CPal ComplexObject             { $$ = g_hash_table_new_full(g_str_hash,
                                                        g_str_equal, free, free_array);
                                          g_hash_table_insert($$, $1, $2); }
                                        /* A lista deve ter pelo menos um elemento, a
                                        cada relação é associada, por via de tabela de hash,
                                        um conjunto de objetos que este engloba (separados
                                        entre si por ','). */
       | PRList ';' CPal ComplexObject  { g_hash_table_insert($1, $3, $4);
                                          $$ = $1; }
                                        /* Os elementos estão separados entre si por ';', de
                                        igual forma, são associado à respectiva relação por meio
                                        de tabela de hash. */
       ;

ComplexObject : CPal                    { $$ = g_array_new(FALSE, FALSE, sizeof(char*));
                                          $$ = g_array_append_val($$, $1); }
                                        /* O conjunto de elementos destino, deve ter pelo menos
                                        um elemento. */
              | ComplexObject ',' CPal  { $$ = g_array_append_val($1, $3); }
                                        /* Os objetos estão separados entre si por ','' */
              ;

MetaList : /* Vazio*/                   { ; }
                                        /* A lista de metadados pode ser vazia. */
     | MetaList MetaElem ';'            { ; }
                                        /* A lista de metadados é compostamente por
                                        vários elementos terminados por ';'. */
     ;

MetaElem : Pal Attrib PalList           { if(containsDictionary(meta, $1)) {
                                                 // emitir erro
                                          } else {
                                                insertDictionary(meta, $1, g_string_free($3, FALSE));
                                          } }
                                        /* Um palavra pode ser associada a um outro conjunto
                                        de palavras. Quando surge uma redifição, deve ser lançado
                                        um warning e ignorada a re-definição. */
         | CPal CPal CPal               { int tmp = add_inter_relation(man, $2, $1, $3);
                                          switch(tmp) {
                                                  case -1:
                                                        // Relacao nao existe.
                                                        break;
                                                  case 0:
                                                        // Elementos repetidos.
                                                        break;
                                                  default:
                                                        // Sucesso.
                                                        break;
                                          } }
                                        /* Descreve uma relação entre-relações, definição
                                        de relações entre relações repetidas devem ser ignoradas. */

         ;

EndingPalList : PalList Paragrafo       { $$ = $1; }
                                        /* corresponde a um conjunto de palavras terminado por
                                        paragrafo. */
              ;

PalList : CPal                          { $$ = g_string_new($1); }
                                        /* Uma lista de palavras deve ter menos uma palavra. */
        | PalList CPal                  { $$ = $1;
                                          g_string_append_printf($$, " %s", $2); }
                                        /* Adiciona novas palavras ao buffer existente, separado-as
                                        com espaços intermédios. */
        ;

CPal : Pal                              { $$ = $1;}
                                        /* Quando explicitada uma Pal, então é efetuada uma
                                        correspondencia direta entre o RHS e o LHS. */
     | Var                              { if(!containsDictionary(meta, $1)) {
                                                $$ = "";
                                                // emitir erro
                                          } else {
                                                $$ = (char*)getValueDictionary(meta, $1);
                                          } }
                                        /* Quando encontrada uma variavel, o seu significado
                                        deve ser pesquisado de entre as possiveis. */
     ;
%%

#include "../lex.yy.c"

void free_array(gpointer data) {
        g_array_free((GArray*)data, TRUE);

}

void foreach_par_relacao(gpointer key,
                gpointer value, gpointer user_data) {
        char *objeto, *relacao = (char*)key, *sujeito = (char*)user_data;
        GArray *arr = (GArray*)value;
        guint i, size = arr->len;

        for(i = 0; i < size; i++) {
                objeto = g_array_index(arr, char*, i);

                if( add_relation(man, relacao, sujeito, objeto) ) {
                        // Tudo certo.
                } else {
                        // warning, redinificao de uma relação repetida.
                }
        }
}

int main(int argc, char **argv)
{
        char *dumpdir = strdup("dump"), buff[1000];
        if(argc >= 2) {
                file = strdup(argv[1]);

                if(argc == 3) {
                        free(dumpdir);
                        dumpdir = strdup(argv[2]);
                }
        }
        else {
                printf("numero invalido de argumentos\n");
                return 0;
        }
        // Utilizado para gestão de erros.
        file = strdup(argv[1]);

        // Ficheiro de input atual.
        yyin = fopen(argv[1], "r");

        // Inicializa dados globais
        // Inicializa dicionario de metadados.
        meta = makeDictionary(NULL);

        // Inicializa manage principal.
        man = init_manager(dumpdir);

        yyparse();

        // Depois do parsing,
        // Aplicar inter-relações
        apply_inter_relations(man);

        // E, despejar o conteudo na diretoria indicada.
        dump_manager(man);

        // Limpar a memoria associada ao programa
        // Libertar o dicionario
        destroyDictionary(meta);

        // Liberta o manager
        destroy_manager(man);

        sprintf(buff, "cp %s %s/source.caderno", file, dumpdir);
        int x = system(buff);


        free(file);
        free(dumpdir);

        return 0;
}
