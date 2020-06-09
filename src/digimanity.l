%{
#include "y.tab.h"
%}

%option noyywrap

wd  [a-zA-Z0-9.]+

%%
[:space:]                     { ; }
                              /* Ignora qualquer espaco */

^[ \t]*#[^\n]*\n              { ; }
                              /* Linhas a comecar com hash sign sao ignoradas
                              na totalidade, garantindo assim que podem ser
                              utilizadas como comentarios. */

\{%{wd}%\}                    { yytext[yylen - 2] = '\0';
                                yylval.word = strdup(yytext + 2);
                                return Var; }
                              /* Define uma palavra complexa,
                              no respectivo campo fica so associado
                              a palavra em si, retirando as chavetas e % */

\"[^\"\n]*\"                  { yytext[yylen - 1] = '\0';
                                yylval.word = strdup(yytext + 1);
                                return Var; }
                              /* Palavras entre aspas são literais, pelo
                              que qualquer elemente dentro e ignorado. */

[^:space:]+                   { yylval.word = strdup(yytext);
                                return Pal; }
                              /* Uma palavra representa qualquer outro
                              conjunto de 1 ou + carateres que nao sejam
                              um espaco de qualquer tipo */

.                             { return yytext[0]; }
                              /* Qualquer outro e emitido como um token
                              desta forma o yacc e capaz de produzir erros
                              de syntax */
%%
