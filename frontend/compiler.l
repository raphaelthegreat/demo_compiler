%{
#include <print>
#include "parse_context.h"
#include "compiler_tab_autogen.h"
%}

%option yylineno
%option noyywrap

%%
[ \t]		;
[ \n]   { yylineno = yylineno + 1;}
int	{ std::print("Found INT token\n"); return INT; }
while { std::print("Found WHILE token\n"); return WHILE; }
if { std::print("Found IF token\n"); return IF; }
else { std::print("Found ELSE token\n"); return ELSE; }
true { std::print("Found TRUE token\n"); return TRUE; }
false { std::print("Found FALSE token\n"); return FALSE; }
return { std::print("Found RETURN token\n"); return RETURN; }
break { std::print("Found BREAK token\n"); return BREAK; }
[0-9]+ { std::print("Found NUM token\n"); yylval.val = atoi(yytext); return NUM; }
[a-zA-Z]([a-zA-Z]|[0-9]|'_')* { strcpy(yylval.str, yytext); return ID; }
"<=" { std::print("Found LE token\n"); return LE; }
">=" { std::print("Found GE token\n"); return GE; }
"==" { std::print("Found EQ token\n"); return EQ; }
"!=" { std::print("Found ME token\n"); return NE; }
">"	{ std::print("Found GT token\n"); return GT; }
"<"	{ std::print("Found LT token\n"); return LT; }
. return yytext[0];
%%
