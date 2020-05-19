%{
#include "y.tab.h"
%}

%option noyywrap
%%
[ ]					{ ; }
\n 					{ return 0; }
[0-9]+				{ return num;}
\.\.				{ return doispontos; }
.					{ return yytext[0]; }
%%
