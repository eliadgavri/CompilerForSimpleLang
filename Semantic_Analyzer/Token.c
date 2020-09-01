#define _CRT_SECURE_NO_WARNINGS
#include "Token.h"

/* This package describes the storage of tokens identified in the input text.
* The storage is a bi-directional list of nodes.
* Each node is an array of tokens; the size of this array is defined as TOKEN_ARRAY_SIZE.
* Such data structure supports an efficient way to manipulate tokens.

There are three functions providing an external access to the storage:
- function create_and_store_tokens ; it is called by the lexical analyzer when it identifies a legal token in the input text.
- functions next_token and back_token; they are called by parser during the syntax analysis (the second stage of compilation)
*/


int currentIndex = 0;
Node* currentNode = NULL;
//counts the number of back token operations 
int backCounter = 0;
int firstError = 0;

#define TOKEN_ARRAY_SIZE 3

//stringTypes holds an array of tokens names
//note: the strings names are compatible with the enum eTOKENS, 
//meaning that each enum must have a compatible string in the same order
char* stringTypes[] = { "TOKEN_LEFT_BRACKET",
	"TOKEN_RIGHT_BRACKET",
	"TOKEN_COMMA",
	"TOKEN_COLON",
	"TOKEN_SEMI_COLON",
	"TOKEN_LEFT_PARENTHESES",
	"TOKEN_RIGHT_PARENTHESES",
	"TOKEN_LEFT_CURLY_BRACES",
	"TOKEN_RIGHT_CURLY_BRACES",
	"TOKEN_PLUS",
	"TOKEN_MULTIPLACTION",
	"TOKEN_ASSIGNMENT",
	"TOKEN_BELOW",
	"TOKEN_BELOW_EQUAL",
	"TOKEN_EQUAL",
	"TOKEN_ABOVE_EQUAL",
	"TOKEN_ABOVE",
	"TOKEN_NOT_EQUAL",
	"TOKEN_INT_NUM",
	"TOKEN_FLOAT_NUM",
	"TOKEN_IF",
	"TOKEN_INT",
	"TOKEN_FLOAT",
	"TOKEN_RETURN",
	"TOKEN_VOID",
	"TOKEN_IDENTIFIER",
	"TOKEN_EOF" };

/*
* This function creates a token and stores it in the storage.
*/
void create_and_store_token(eTOKENS kind, char* lexeme, int numOfLine)
{
	// case 1: there is still no tokens in the storage.
	if (currentNode == NULL)
	{
		currentNode = (Node*)malloc(sizeof(Node));

		if (currentNode == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->tokensArray = (Token*)malloc(sizeof(Token) * TOKEN_ARRAY_SIZE);
		if (currentNode->tokensArray == NULL)
		{
			fprintf(yyout, "\nUnable to allocate memory! \n");
			exit(0);
		}
		currentNode->prev = NULL;
		currentNode->next = NULL;
	}

	// case 2: at least one token exists in the storage.
	else
	{
		// the array (the current node) is full, need to allocate a new node
		if (currentIndex == TOKEN_ARRAY_SIZE - 1)
		{
			currentIndex = 0;
			currentNode->next = (Node*)malloc(sizeof(Node));

			if (currentNode == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next->prev = currentNode;
			currentNode = currentNode->next;
			currentNode->tokensArray = (Token*)malloc(sizeof(Token) * TOKEN_ARRAY_SIZE);

			if (currentNode->tokensArray == NULL)
			{
				fprintf(yyout, "\nUnable to allocate memory! \n");
				exit(0);
			}
			currentNode->next = NULL;
		}

		// the array (the current node) is not full
		else
		{
			currentIndex++;
		}
	}


	currentNode->tokensArray[currentIndex].kind = kind;
	currentNode->tokensArray[currentIndex].lexeme = (char*)malloc(sizeof(char) * (strlen(lexeme) + 1));
	strcpy(currentNode->tokensArray[currentIndex].lexeme, lexeme);
	currentNode->tokensArray[currentIndex].lineNumber = numOfLine;
}

/*
* This function returns the token in the storage that is stored immediately before the current token (if exists).
*/
Token* back_token()
{
	if (currentNode == NULL)
		return NULL;

	//increase back counter
	backCounter++;

	if (currentIndex > 0)
	{
		//return previous token in the current node array
		return &currentNode->tokensArray[--currentIndex];
	}
	if (currentIndex == 0)
	{
		//first node in the list(head)
		if (currentNode->prev == NULL)
		{
			//error, no previous for first node
			if (firstError == 0) // if it's the first error reset the back counter and raise a flag
			{
				backCounter = 0;
				firstError = 1;
			}
			return NULL;
		}
		else //if not first node
		{
			//go back to previous node
			currentIndex = TOKEN_ARRAY_SIZE - 1;
			currentNode = currentNode->prev;
			return &currentNode->tokensArray[currentIndex];
		}
	}
	return NULL;
}

/*
* If the next token already exists in the storage (this happens when back_token was called before this call to next_token):
*  this function returns the next stored token.
* Else: continues to read the input file in order to identify, create and store a new token (using yylex function);
*  returns the token that was created.
*/
Token* next_token()
{
	//if the current node array is full
	if (currentIndex == TOKEN_ARRAY_SIZE - 1)
	{
		//if next node already exist, procceed to next node
		if (currentNode->next != NULL)
		{
			//initialize current index
			currentIndex = 0;
			currentNode = currentNode->next;
			if (backCounter > 0)
			{
				backCounter--;
			}
			return &currentNode->tokensArray[currentIndex];
		}
	}
	//current node array is not full, procceed this node array
	//if backCounter is greater than 0, no need to call yylex for next token since it was already called
	if (backCounter > 0)
	{
		//update the backCounter
		backCounter--;
		//move forward in the node's tokens array
		currentIndex++;
	}
	//if backCounter equals to 0, meaning that yylex wasnt called yet for the next token
	else
	{
		//call yylex to proccess the next token
		yylex();
	}
	return &currentNode->tokensArray[currentIndex];
}


char* getKindStr(int stringTypeIndex)
{
	return stringTypes[stringTypeIndex];
}