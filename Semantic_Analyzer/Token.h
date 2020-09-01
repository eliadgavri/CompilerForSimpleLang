#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE* yyin, * yyout;
FILE* yyoutSyntactic;
FILE* yyoutSemantic;

typedef enum eTOKENS
{
	TOKEN_LEFT_BRACKET,
	TOKEN_RIGHT_BRACKET,
	TOKEN_COMMA,
	TOKEN_COLON,
	TOKEN_SEMI_COLON,
	TOKEN_LEFT_PARENTHESES,
	TOKEN_RIGHT_PARENTHESES,
	TOKEN_LEFT_CURLY_BRACES,
	TOKEN_RIGHT_CURLY_BRACES,
	TOKEN_PLUS,
	TOKEN_MULTIPLACTION,
	TOKEN_ASSIGNMENT,
	TOKEN_BELOW,
	TOKEN_BELOW_EQUAL,
	TOKEN_EQUAL,
	TOKEN_ABOVE_EQUAL,
	TOKEN_ABOVE,
	TOKEN_NOT_EQUAL,
	TOKEN_INT_NUM,
	TOKEN_FLOAT_NUM,
	TOKEN_IF,
	TOKEN_INT,
	TOKEN_FLOAT,
	TOKEN_RETURN,
	TOKEN_VOID,
	TOKEN_IDENTIFIER,
	TOKEN_EOF
}eTOKENS;

typedef struct Token
{
	eTOKENS kind;
	char* lexeme;
	int lineNumber;
}Token;

typedef struct Node
{
	Token* tokensArray;
	struct Node* prev;
	struct Node* next;
} Node;

void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine);
Token* next_token();
Token* back_token();
char* getKindStr(int stringTypeIndex);

#endif