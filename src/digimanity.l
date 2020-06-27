%{
#include <gmodule.h>
#include "y.tab.h"

/* Ação predifinida antes de qualquer token. Visa incrementar a posição
atual dentro do buffer a ser processado, para poder indicar este no
processamento de errros. */
#define YY_USER_ACTION (tokenpos = tokenpos + yyleng);

/* Armazena a linha atual a ser processada. */
int lineno = 1;

/* Armazena, dentro da linha a ser processada, a posição deste. Para que
depois de possa apontar, ao certo a posição onde sucedeu um erro. */
int tokenpos = 0;

/* Armazena o conteudo da linha a ser atualmente processada. Para que depois
se possa realizar um print da linha onde surgiu um erro. */
char auxbuff[500];

/* Ficheiro a ser atualmente processado, este variavel é estabelecida como
variavel externa no segmento main do yacc. */
char *file="";

%}

%option noyywrap

%s TRIPLOS META TITLE

wd  [a-zA-Z0-9.]+
sp [ \t]
%%
<META>\<\-              { return Attrib; }
                        /* token de atribuição, dentro do estado META. */

<TRIPLOS>[.;,]          { return yytext[0]; }
                        /* No estado TRIPLOS, os carateres indicados tem um
                        significado especifico (e bem definido) pelo que devem
                        ser explicitamente retornados. */

<META>[;]               { return yytext[0]; }
                        /* No estado META, o carate ';' tem um significado
                        especifico (e bem definido) pelo que deve ser sinalizado. */

<INITIAL>(\n)+          { lineno += strlen(yytext);
                          return Paragrafo; }
                        /* Define-se um paragrafo como sendo um conjunto de
                        newlines seguidos. O número de linhas deve ser conta-
                        bilizado. Só surge no estado INITIAL, pois nos outros
                        não há noção de paragrafos. */

{sp}*#[^\n]*\n         { lineno++; }
                        /* Linhas a começar com zero ou mais espaços em branco,
                        seguidas de um '#' são consideradas comentários, pelo
                        que são totalmente ignoradas pelo analisador léxico. */


\n.*                    { tokenpos = 0;
                          strcpy(auxbuff, yytext + 1);
                          lineno++;
                          yyless(1); }
                        /* É armazenado um buffer em memória, contendo todos
                        os elementos a linha a ser atualmente processada. Este
                        mecanismo permite uma maior facilidade para gestão de
                        erros. */

{sp}                    { ; }
                        /* Ignora qualquer espaco */

=+                      { BEGIN(0);
                          yylval.count = yyleng;
                          return TitleTab; }
                        /* Se foi encontrado um conjunto de sinais '=', então
                        o analisador léxico retorna ao estado INITIAL. Conta-
                        bilizando todos os elementos necessários. */

(?i:meta)               { BEGIN(META);
                          return MetaTag; }
                        /* Se a tag, independente de caps lock, foi encontrada
                        assume-se que estamos a trabalhar no estado de META,
                        fornecendo o respectivo token. */

@tit{sp}*:              { return TituloTag; }
                        /* Formatação tipo da formatação de titulos, a tag '@tit'
                        seguida de espaços com um carater ':' no fim. */

@triplos{sp}*:          { BEGIN(TRIPLOS);
                          return TriplosTag; }
                        /* Semelhante à tag de titulo, passa ao estado TRIPLOS
                        pelo que certos carateres ganham significados explicitos. */

\{%{wd}%\}              { yytext[yyleng - 2] = '\0';
                          yylval.word = strdup(yytext + 2);
                          return Var; }
                        /* Define a tipicidade de uma variavel, pelo que esta
                        é transmitida ao yacc na sua forma passe (sem os delimitadores). */

\"[^\"\n]*\"            { yytext[yyleng - 1] = '\0';
                          yylval.word = strdup(yytext + 1);
                          return Pal; }
                        /* Palavras delimitadas por aspas, são consideradas como
                        literais. */

<TRIPLOS>[^ \t=\n,;.]+  { yylval.word = strdup(yytext);
                          return Pal; }
                        /* Uma palavra define-se como um conjunto de carateres,
                        sem espaços intermédios, e sem carateres de significado
                        explicito, no estado TRIPLOS. */

<INITIAL>[^ \t=\n]+     { yylval.word = strdup(yytext);
                          return Pal; }
                        /* Semelhante à produção acima, contabilizando carateres
                        de significado explicito distintos. */

<META>[^ \t=\n;]+       { yylval.word = strdup(yytext);
                          return Pal; }
                        /* Semelhante à produção acima, contabilizando carateres
                        de significado explicito distintos. */

.                       { return yytext[0]; }
                        /* Qualquer outro carater deve ser retornado como tal */
%%

/* Objetivo de definir um conjunto de carateres, todos iguais, com um certo
tamanho.*/
char *char_repeat(int n, char k) {
        int i;
        char *dest = malloc(sizeof(char) * (n + 1));

        for(i = 0; i < n; i++) {
                dest[i] = k;
        }

        dest[i] = '\0';

        return dest;
}

/* Função de error utilizada, imprime o ficheiro, linha e posição, onde surge o
erro. */
int yyerror(char *s)
{
        fprintf(stderr, "text is %s, length is %d\n", yytext, yyleng);
        int i;
        char *buff = char_repeat(yyleng, '^');

	fprintf(stderr, "%s:%d:%d: %s:\n%s\n",
                file, lineno, tokenpos, s, auxbuff);

        fprintf(stderr, "%*s\n", tokenpos, buff);

        free(buff);
	return 0;
}
