%option noyywrap
%option nounput
%option noinput

%{

#include <cstdlib>
#include <string>
#include "sysy.tab.hpp"    // Some variables in sysy.y is used here

using namespace std;

%}

/* 空白符和注释 */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*
BlockComment   \/\*([^\*]*|[\*]+[^\*\/])*[\*]+\/

/* 标识符 */
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

%%

{WhiteSpace}    { /* Do nothing */ }
{LineComment}   { /* Do nothing */ }
{BlockComment}  { /* Do nothing*/}

"<="            { return LE; }
">="            { return GE; }
"=="            { return EQ; }
"!="            { return NE;}
"&&"            { return AND; }
"||"            { return OR; }

"int"           { return INT; }
"return"        { return RETURN; }
"const"         { return CONST; }
"if"            {return IF;}
"else"          {return ELSE;}
"while"         {return WHILE;}
"break"         {return BREAK;}
"continue"      {return CONTINUE;}
"void"          {return VOID;}

{Identifier}    { yylval.str_val = new string(yytext); return IDENT; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }

.               { return yytext[0]; }

%%
