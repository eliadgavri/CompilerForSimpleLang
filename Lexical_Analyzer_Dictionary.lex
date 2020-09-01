%option noyywrap

%{
#include "Parser.h"
#include "Token.h"
int line_num=1;
%}

DIGIT    [0-9]
ALPHA	 [a-zA-Z]
LOWALPHA	[a-z]
TOKEN_IF		if
TOKEN_INT	int
TOKEN_FLOAT	float
TOKEN_RETURN	return
TOKEN_VOID	void	
UNDERSCORE	_
DOT		.
COMMA		","
COLON		":"
SEMI_COLON	";"
LEFT_PARENTHESES	"("
RIGHT_PARENTHESES	")"
LEFT_CURLY_BRACES	"{"
RIGHT_CURLY_BRACES	"}"
LEFT_BRACKET	        "["
RIGHT_BRACKET	        "]"
PLUS	"+"
MULTIPLACTION	"*"
ASSIGNMENT	"="
BELOW	"<"
BELOW_EQUAL	"<="
EQUAL	"=="
ABOVE	">"
ABOVE_EQUAL	">="
NOT_EQUAL	"!="
START_COMMENT	"/*"
INT_NUM		0|[1-9][0-9]*
FLOAT_NUM	{INT_NUM}"."{DIGIT}+("e"|"E")("+"|"-")?{INT_NUM}
IDENTIFIER	{LOWALPHA}({ALPHA}|{DIGIT})*({UNDERSCORE}?({ALPHA}|{DIGIT})+)*
%x comment

%%

" "			{}
\t			{}
\r			{}
\n			{line_num++;}

{COMMA}	{ create_and_store_token(TOKEN_COMMA,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_COMMA, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{COLON}		{ create_and_store_token(TOKEN_COLON,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_COLON, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{SEMI_COLON}	{ create_and_store_token(TOKEN_SEMI_COLON,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_SEMI_COLON, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{LEFT_PARENTHESES}	{ create_and_store_token(TOKEN_LEFT_PARENTHESES,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_LEFT_PARENTHESES, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{RIGHT_PARENTHESES}	{ create_and_store_token(TOKEN_RIGHT_PARENTHESES,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_RIGHT_PARENTHESES, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{LEFT_CURLY_BRACES}	{ create_and_store_token(TOKEN_LEFT_CURLY_BRACES,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_LEFT_CURLY_BRACES, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{RIGHT_CURLY_BRACES}	{ create_and_store_token(TOKEN_RIGHT_CURLY_BRACES,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_RIGHT_CURLY_BRACES, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{LEFT_BRACKET}		{ create_and_store_token(TOKEN_LEFT_BRACKET,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_LEFT_BRACKET, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{RIGHT_BRACKET}		{ create_and_store_token(TOKEN_RIGHT_BRACKET,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_RIGHT_BRACKET, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{PLUS}	{ create_and_store_token(TOKEN_PLUS,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_PLUS, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{MULTIPLACTION}	{ create_and_store_token(TOKEN_MULTIPLACTION,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_MULTIPLACTION, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{ASSIGNMENT}	{ create_and_store_token(TOKEN_ASSIGNMENT,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_ASSIGNMENT, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{BELOW}	{ create_and_store_token(TOKEN_BELOW,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_BELOW, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{BELOW_EQUAL}	{ create_and_store_token(TOKEN_BELOW_EQUAL,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_BELOW_EQUAL, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{EQUAL}	{ create_and_store_token(TOKEN_EQUAL,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_EQUAL, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{ABOVE_EQUAL}	{ create_and_store_token(TOKEN_ABOVE_EQUAL,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_ABOVE_EQUAL, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{ABOVE}	{ create_and_store_token(TOKEN_ABOVE,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_ABOVE, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{NOT_EQUAL}	{ create_and_store_token(TOKEN_NOT_EQUAL,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_NOT_EQUAL, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{INT_NUM}     { create_and_store_token(TOKEN_INT_NUM,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_INT_NUM, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{FLOAT_NUM}     { create_and_store_token(TOKEN_FLOAT_NUM,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_FLOAT_NUM, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{TOKEN_IF}	{ create_and_store_token(TOKEN_IF,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_IF, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{TOKEN_FLOAT}	{ create_and_store_token(TOKEN_FLOAT,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_FLOAT, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{TOKEN_INT}	{ create_and_store_token(TOKEN_INT,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_INT, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{TOKEN_RETURN}	{ create_and_store_token(TOKEN_RETURN,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_RETURN, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{TOKEN_VOID}	{ create_and_store_token(TOKEN_VOID,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_VOID, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
{IDENTIFIER}	{ create_and_store_token(TOKEN_IDENTIFIER,  yytext, line_num); fprintf(yyout,"Token of type TOKEN_IDENTIFIER, lexeme: %s, found in line: %d\n", yytext, line_num); return 1;}
<<EOF>>	{ create_and_store_token(TOKEN_EOF,  "", line_num); fprintf(yyout,"Token of type TOKEN_EOF, lexeme: <<EOF>>, found in line: %d\n", line_num); BEGIN(0); return 0;}

{START_COMMENT}  BEGIN(comment);
<comment>[^*/\n \t/*/*]*
<comment>"\n"	++line_num;
<comment>" "
<comment>"\t"
<comment>"/*"
<comment>"/"
<comment>"*"
<comment>"*/"  BEGIN(0);

.	fprintf(yyout, "Character %s in line: %d does not begin any legal token in the language.\n", yytext, line_num);

%%

void main(int argc, char* argv[])
{
int select;
	do {
		select = 0;
		printf("Hello, please select which test you would like to run - 1 or 2\n");
		scanf("%d", &select);
		if ((select < 1) || (select > 2))
			printf("Wrong Choice.\n");
	} while ((select < 1) || (select > 2));
	switch (select)
	{
	case 1:
	{
		yyin = fopen("C:\\temp\\test1.txt", "r");
		yyout = fopen("C:\\temp\\test1_205645377_205702152_313268203_lex.txt", "w");
		yyoutSyntactic = fopen("C:\\temp\\test1_205645377_205702152_313268203_syntactic.txt", "w");
		yyoutSemantic = fopen("C:\\temp\\test1_205645377_205702152_313268203_semantic.txt", "w");
		InitializeParserGrammer();
		parse_PROG();
		freeArrays();
		fclose(yyin);
		fclose(yyout);
		fclose(yyoutSyntactic);
		fclose(yyoutSemantic);
		break;
	}
	case 2:
	{
		yyin = fopen("C:\\temp\\test2.txt", "r");
		yyout = fopen("C:\\temp\\test2_205645377_205702152_313268203_lex.txt", "w");
		yyoutSyntactic = fopen("C:\\temp\\test2_205645377_205702152_313268203_syntactic.txt", "w");
		yyoutSemantic = fopen("C:\\temp\\test2_205645377_205702152_313268203_semantic.txt", "w");
		InitializeParserGrammer();
		parse_PROG();
		freeArrays();
		fclose(yyin);
		fclose(yyout);
		fclose(yyoutSyntactic);
		fclose(yyoutSemantic);
		break;
	}
	default:
		break;
	}
}
