%{
#include "structs.c"
#include "calc-sintaxis.tab.h"


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void setyylloc() {
	yylloc.first_line = yylineno;
	yylloc.first_column = yylloc.last_column;
	yylloc.last_column += yyleng;
}

void reset_yylloc_column() {
	yylloc.first_column = 1;
	yylloc.last_column = 1;
}
%}

%option noyywrap
%option yylineno

letter [a-zA-z]
digit [0-9]
id {letter}({letter}|{digit})*

START "{"
END "}"
SIMPLE [^}]
COMPLEX [{|\n]

%%

program {
	
	return _PROGRAM_;
}
begin {
	setyylloc();
	return _BEGIN_;
}
end {
	setyylloc();
	return _END_;
}
void {
	setyylloc();
	return _VOID_;
}
if {
	setyylloc();
	return _IF_;
}
else {
	setyylloc();
	return _ELSE_;
}
then {
	setyylloc();
	return _THEN_;
}
integer {
	setyylloc();
	return _INTEGER_;
}
return {
	setyylloc();
	return _RETURN_;
}
main {
	setyylloc();
	return _MAIN_;
}
bool {
	setyylloc();
	return _BOOL_;
}
true {
	setyylloc();
	return _TRUE_;
}
false {
	setyylloc();
	return _FALSE_;
}
while {
	setyylloc();
	return _WHILE_;
}
= {
	setyylloc();
	return _ASSIGNMENT_;
}
; {
	setyylloc();
	return _SEMICOLON_;
}
, {
	setyylloc();
	return _COMMA_;
}
\( {
	setyylloc();
	return _L_PARENTHESIS_;
}
\) {
	setyylloc();
	return _R_PARENTHESIS_;
}
\+ {
	setyylloc();
	return _PLUS_;
}
- {
	setyylloc();
	return _MINUS_;
}
\* {
	setyylloc();
	return _MULTIPLY_;
}
\/ {
	setyylloc();
	return _DIVIDE_;
}
% {
	setyylloc();
	return _MOD_;
}
> {
	setyylloc();
	return _GREATER_THAN_;
}
\< {
	setyylloc();
	return _LESSER_THAN_;
}
== {
	setyylloc();
	return _EQUALS_;
}
&& {
	setyylloc();
	return _AND_;
}
\|\| {
	setyylloc();
	return _OR_;
}
! {
	setyylloc();
	return _NOT_;
}
extern {
	setyylloc();
	return _EXTERN_;
}

{digit}+ { 
	setyylloc();
	yylval.i = atoi(yytext);
	return _INT_;
}

{id} { 
	setyylloc();
	yylval.s=(char *) malloc(sizeof(yytext)*yyleng);
	strcpy(yylval.s,yytext);
	return _ID_;
}

(\n)+ {
	reset_yylloc_column();
}

[ \t] {
	setyylloc();
}

"//".*\n  {
	reset_yylloc_column();
}

{START}({SIMPLE}|{COMPLEX})*{END}

. { 
	printf(ANSI_COLOR_RED "error!\n" ANSI_COLOR_RESET, yylineno);
	printf("\t" ANSI_COLOR_RED "'%s'" ANSI_COLOR_RESET " is not a valid / recognized input\n", yytext);
	yyterminate();
}

%%

void yyerror(const char *str) {
	fprintf(stderr,"Error | Line: %d\n%s\n",yylineno, str);
}

void yyerror2(char *s, int line_number) {
	fprintf(stderr, "line %d: %s\n", line_number, s);
}

int get_line_number() {
	return yylloc.first_line;
}

int get_column_number() {
	return yylloc.first_column;
}

int main(int argc,char *argv[]) {
	++argv,--argc;
	if (argc > 0)
		yyin = fopen(argv[0],"r");
	else
		yyin = stdin;
	yyparse();
}
