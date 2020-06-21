%{
#include <gmodule.h>
#include "y.tab.h"

#define YY_USER_ACTION (tokenpos = tokenpos + yyleng);

int lineno = 1, tokenpos = 0;
char auxbuff[500];

%}

%option noyywrap

wd  [a-zA-Z0-9.]+

%%
^.*                             { tokenpos = 0;
                                  strcpy(auxbuff, yytext + 1);
                                  lineno++;
                                  yyless(1); }

[:space:]                       { ; }
                                /* Ignora qualquer espaco */

^[:space:]*#[^\n]*\n            { ; }
                                /* Linhas a comecar com hash sign sao ignoradas
                                na totalidade, garantindo assim que podem ser
                                utilizadas como comentarios. */

=+                             { printf("hit\n");
                                  yylval.count = yyleng;
                                  return TitleTab; }
                                /* Guarda o numero de carateres '=' encontrados. */

(?i:meta)                       { return MetaTag; }

@tit[:space:]*:                 { return TituloTag; }

@triplos[:space:]*:             { return TriplosTag; }

(^\n)+                          { return Paragrafo; }

\{%{wd}%\}                      { yytext[yyleng - 2] = '\0';
                                  yylval.word = strdup(yytext + 2);
                                  return Var; }
                                /* Define uma palavra complexa,
                                no respectivo campo fica so associado
                                a palavra em si, retirando as chavetas e % */

\"[^\"\n]*\"                    { yytext[yyleng - 1] = '\0';
                                  yylval.word = strdup(yytext + 1);
                                  return Pal; }
                                /* Palavras entre aspas são literais, pelo
                                que qualquer elemento dentro e ignorado. */

[^ =\n]+                     { printf("hit 2 %d size\n", yyleng);
                                  yylval.word = strdup(yytext);
                                  return Pal; }
                                /* Uma palavra representa qualquer outro
                                conjunto de 1 ou + carateres que nao sejam
                                um espaco de qualquer tipo */
%%

char *char_repeat(int n, char k) {
        int i;
        char *dest = malloc(sizeof(char) * (n + 1));

        for(i = 0; i < n; i++) {
                dest[i] = k;
        }

        dest[i] = '\0';

        return dest;
}

int yyerror(char *s)
{
        fprintf(stderr, "text is %s, length is %d\n", yytext, yyleng);
        int i;
        char *buff = char_repeat(yyleng, '^');

	fprintf(stderr, "%d: %s:\n%s\n",
                yylineno, s, auxbuff);

        fprintf(stderr, "%*s\n", tokenpos, buff);

        free(buff);
	return 0;
}
