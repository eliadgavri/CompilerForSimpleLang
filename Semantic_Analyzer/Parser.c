#include "Parser.h"
#include "SymbolTable.h"

Table* curr_table = NULL;
Table_Node* id_entry = NULL;
Table_Node* table_entry = NULL;
eTOKENS curr_type;
eTOKENS returned_type;
int func_flag = 0;
int return_flag = 1;

//add follow by eVar id
void addFollow(eVARS eVar, eTOKENS eToken) {
	int index = varsArray[eVar].follow.realSize++;
	varsArray[eVar].follow.tokenArr[index] = eToken; //add the eToken to follow array
	resizeArray(&varsArray[eVar].follow);
}

//add first by eVar id
void addFirst(eVARS eVar, eTOKENS eToken) {
	int index = varsArray[eVar].first.realSize++;
	varsArray[eVar].first.tokenArr[index] = eToken; //add the eToken to first array
	resizeArray(&varsArray[eVar].first);
}

//resize tokens first or follow array when the array is full
void resizeArray(tokenStruct* tArray) {
	if (tArray->realSize == tArray->arraySize) {
		tArray->arraySize *= INITIAL_ARRAY_SIZE;
		tArray->tokenArr = (int*)realloc(tArray->tokenArr, sizeof(int) * (tArray->arraySize));
	}
}

//free first and follow tokens array
void freeArrays()
{
	int i;
	for (i = 0; i < VARS_NUM; i++)
	{
		free(varsArray[i].first.tokenArr);
		free(varsArray[i].follow.tokenArr);
	}
}

//match the expected kind with the actual kind
int matchToken(eTOKENS kind)
{
	curr_token = next_token();

	//if there is no match
	if (curr_token->kind != kind)
	{
		char* actualStr = getKindStr(curr_token->kind);
		char* expectedStr = getKindStr(kind);
		fprintf(yyoutSyntactic, "Expected token of type '%s' at line : %d, Actual token of type '%s', lexeme: '%s'\n", expectedStr, curr_token->lineNumber, actualStr, curr_token->lexeme);
		//call error recovery
		RecoveryAfterError();
		return 0;
	}
	return 1;
}

//find the variable which its follow contains the current token
//return true if found, false otherwise
int isContainCurrentToken()
{
	int i;
	for (i = 0; i < varsArray[currVarIndex].follow.realSize; i++)
	{
		if (varsArray[currVarIndex].follow.tokenArr[i] == curr_token->kind)
		{
			return 1;
		}
	}
	return 0;
}

//print unexpected token
void printUnexpectedToken()
{
	char* kind;
	int i;
	//in order to avoid multiple printings of identitcal tokens we manage an array 
	//holding the already printed tokens
	int printedTokens[TOKEN_EOF + 1] = { 0 };
	int kindInd;

	fprintf(yyoutSyntactic, "Expected token of type");
	//print variable first tokens
	for (i = 0; i < varsArray[currVarIndex].first.realSize; i++)
	{
		kindInd = varsArray[currVarIndex].first.tokenArr[i];
		kind = getKindStr(kindInd);
		if (i != 0)
		{
			fprintf(yyoutSyntactic, "/");
		}
		fprintf(yyoutSyntactic, " %s ", kind);
		printedTokens[kindInd] = 1;
	}
	//if the variable is nullable print the variable's follow tokens
	if (varsArray[currVarIndex].nullable)
	{
		for (i = 0; i < varsArray[currVarIndex].follow.realSize; i++)
		{
			kindInd = varsArray[currVarIndex].follow.tokenArr[i];
			if (printedTokens[kindInd] == 0)
			{
				kind = getKindStr(kindInd);
				if (i != (varsArray[currVarIndex].follow.realSize))
				{
					fprintf(yyoutSyntactic, "/");
				}
				fprintf(yyoutSyntactic, " %s ", kind);
			}
		}
	}
	fprintf(yyoutSyntactic, " at line %d, Actual token: %s, lexeme: '%s'\n", curr_token->lineNumber, getKindStr(curr_token->kind), curr_token->lexeme);
}

//handle error recovery
void RecoveryAfterError()
{
	//if error occured check if current token is contained in the current variable follow
	//if not, move to next token.
	while (!isContainCurrentToken() && (curr_token->kind != TOKEN_EOF))
	{
		curr_token = next_token();
	}
	//if the variable follow which contains the current token was not found
	if (curr_token->kind == TOKEN_EOF)
	{
		exit(0);
	}
	//the variable which its follow contains the current token was found
	//go back one token to find the deriving variable
	else
	{
		curr_token = back_token();
	}
}

void InitializeParserGrammer() {
	int i;
	for (i = 0; i < VARS_NUM; i++)
	{
		varsArray[i].first.realSize = 0;
		varsArray[i].follow.realSize = 0;
		varsArray[i].nullable = 0;
		varsArray[i].first.arraySize = INITIAL_ARRAY_SIZE;
		varsArray[i].follow.arraySize = INITIAL_ARRAY_SIZE;
		//allocate area for first tokens of a variable
		varsArray[i].first.tokenArr = (int*)malloc(sizeof(int) * INITIAL_ARRAY_SIZE);
		//allocate area for follow tokens of a variable
		varsArray[i].follow.tokenArr = (int*)malloc(sizeof(int) * INITIAL_ARRAY_SIZE);
	}

	//Variables attributes according to grammar
	varsArray[PROG].nullable = 0;
	addFirst(PROG, TOKEN_FLOAT);
	addFirst(PROG, TOKEN_INT);
	addFollow(PROG, TOKEN_EOF);

	varsArray[GLOBAL_VARS].nullable = 0;
	addFirst(GLOBAL_VARS, TOKEN_FLOAT);
	addFirst(GLOBAL_VARS, TOKEN_INT);
	addFollow(GLOBAL_VARS, TOKEN_FLOAT);
	addFollow(GLOBAL_VARS, TOKEN_INT);
	addFollow(GLOBAL_VARS, TOKEN_VOID);

	varsArray[GLOBAL_VARS_TAG].nullable = 1;
	addFirst(GLOBAL_VARS_TAG, TOKEN_FLOAT);
	addFirst(GLOBAL_VARS_TAG, TOKEN_INT);
	addFollow(GLOBAL_VARS_TAG, TOKEN_FLOAT);
	addFollow(GLOBAL_VARS_TAG, TOKEN_INT);
	addFollow(GLOBAL_VARS_TAG, TOKEN_VOID);

	varsArray[VAR_DEC].nullable = 0;
	addFirst(VAR_DEC, TOKEN_FLOAT);
	addFirst(VAR_DEC, TOKEN_INT);
	addFollow(VAR_DEC, TOKEN_FLOAT);
	addFollow(VAR_DEC, TOKEN_IDENTIFIER);
	addFollow(VAR_DEC, TOKEN_IF);
	addFollow(VAR_DEC, TOKEN_INT);
	addFollow(VAR_DEC, TOKEN_RETURN);
	addFollow(VAR_DEC, TOKEN_VOID);
	addFollow(VAR_DEC, TOKEN_LEFT_CURLY_BRACES);

	varsArray[VAR_DEC_TAG].nullable = 0;
	addFirst(VAR_DEC_TAG, TOKEN_SEMI_COLON);
	addFirst(VAR_DEC_TAG, TOKEN_LEFT_BRACKET);
	addFollow(VAR_DEC_TAG, TOKEN_FLOAT);
	addFollow(VAR_DEC_TAG, TOKEN_IDENTIFIER);
	addFollow(VAR_DEC_TAG, TOKEN_IF);
	addFollow(VAR_DEC_TAG, TOKEN_INT);
	addFollow(VAR_DEC_TAG, TOKEN_RETURN);
	addFollow(VAR_DEC_TAG, TOKEN_VOID);
	addFollow(VAR_DEC_TAG, TOKEN_LEFT_CURLY_BRACES);

	varsArray[TYPE].nullable = 0;
	addFirst(TYPE, TOKEN_FLOAT);
	addFirst(TYPE, TOKEN_INT);
	addFollow(TYPE, TOKEN_IDENTIFIER);

	varsArray[DIM_SIZES].nullable = 0;
	addFirst(DIM_SIZES, TOKEN_INT_NUM);
	addFollow(DIM_SIZES, TOKEN_RIGHT_BRACKET);

	varsArray[DIM_SIZES_TAG].nullable = 1;
	addFirst(DIM_SIZES_TAG, TOKEN_COMMA);
	addFollow(DIM_SIZES_TAG, TOKEN_RIGHT_BRACKET);

	varsArray[FUNC_PREDEFS].nullable = 0;
	addFirst(FUNC_PREDEFS, TOKEN_FLOAT);
	addFirst(FUNC_PREDEFS, TOKEN_INT);
	addFirst(FUNC_PREDEFS, TOKEN_VOID);
	addFollow(FUNC_PREDEFS, TOKEN_FLOAT);
	addFollow(FUNC_PREDEFS, TOKEN_INT);
	addFollow(FUNC_PREDEFS, TOKEN_VOID);

	varsArray[FUNC_PREDEFS_TAG].nullable = 1;
	addFirst(FUNC_PREDEFS_TAG, TOKEN_FLOAT);
	addFirst(FUNC_PREDEFS_TAG, TOKEN_INT);
	addFirst(FUNC_PREDEFS_TAG, TOKEN_VOID);
	addFollow(FUNC_PREDEFS_TAG, TOKEN_FLOAT);
	addFollow(FUNC_PREDEFS_TAG, TOKEN_INT);
	addFollow(FUNC_PREDEFS_TAG, TOKEN_VOID);

	varsArray[FUNC_PROTOTYPE].nullable = 0;
	addFirst(FUNC_PROTOTYPE, TOKEN_FLOAT);
	addFirst(FUNC_PROTOTYPE, TOKEN_INT);
	addFirst(FUNC_PROTOTYPE, TOKEN_VOID);
	addFollow(FUNC_PROTOTYPE, TOKEN_SEMI_COLON);
	addFollow(FUNC_PROTOTYPE, TOKEN_LEFT_CURLY_BRACES);

	varsArray[FUNC_FULL_DEFS].nullable = 0;
	addFirst(FUNC_FULL_DEFS, TOKEN_FLOAT);
	addFirst(FUNC_FULL_DEFS, TOKEN_INT);
	addFirst(FUNC_FULL_DEFS, TOKEN_VOID);
	addFollow(FUNC_FULL_DEFS, TOKEN_EOF);

	varsArray[FUNC_FULL_DEFS_TAG].nullable = 1;
	addFirst(FUNC_FULL_DEFS_TAG, TOKEN_FLOAT);
	addFirst(FUNC_FULL_DEFS_TAG, TOKEN_INT);
	addFirst(FUNC_FULL_DEFS_TAG, TOKEN_VOID);
	addFollow(FUNC_FULL_DEFS_TAG, TOKEN_EOF);

	varsArray[FUNC_WITH_BODY].nullable = 0;
	addFirst(FUNC_WITH_BODY, TOKEN_FLOAT);
	addFirst(FUNC_WITH_BODY, TOKEN_INT);
	addFirst(FUNC_WITH_BODY, TOKEN_VOID);
	addFollow(FUNC_WITH_BODY, TOKEN_EOF);
	addFollow(FUNC_WITH_BODY, TOKEN_FLOAT);
	addFollow(FUNC_WITH_BODY, TOKEN_INT);
	addFollow(FUNC_WITH_BODY, TOKEN_VOID);

	varsArray[RETURNED_TYPE].nullable = 0;
	addFirst(RETURNED_TYPE, TOKEN_FLOAT);
	addFirst(RETURNED_TYPE, TOKEN_INT);
	addFirst(RETURNED_TYPE, TOKEN_VOID);
	addFollow(RETURNED_TYPE, TOKEN_IDENTIFIER);

	varsArray[PARAMS].nullable = 1;
	addFirst(PARAMS, TOKEN_FLOAT);
	addFirst(PARAMS, TOKEN_INT);
	addFollow(PARAMS, TOKEN_RIGHT_PARENTHESES);

	varsArray[PARAM_LIST].nullable = 0;
	addFirst(PARAM_LIST, TOKEN_FLOAT);
	addFirst(PARAM_LIST, TOKEN_INT);
	addFollow(PARAM_LIST, TOKEN_RIGHT_PARENTHESES);

	varsArray[PARAM_LIST_TAG].nullable = 1;
	addFirst(PARAM_LIST_TAG, TOKEN_COMMA);
	addFollow(PARAM_LIST_TAG, TOKEN_RIGHT_PARENTHESES);

	varsArray[PARAM].nullable = 0;
	addFirst(PARAM, TOKEN_FLOAT);
	addFirst(PARAM, TOKEN_INT);
	addFollow(PARAM, TOKEN_RIGHT_PARENTHESES);
	addFollow(PARAM, TOKEN_COMMA);

	varsArray[PARAM_TAG].nullable = 1;
	addFirst(PARAM_TAG, TOKEN_LEFT_BRACKET);
	addFollow(PARAM_TAG, TOKEN_RIGHT_PARENTHESES);
	addFollow(PARAM_TAG, TOKEN_COMMA);

	varsArray[COMP_STMT].nullable = 0;
	addFirst(COMP_STMT, TOKEN_LEFT_CURLY_BRACES);
	addFollow(COMP_STMT, TOKEN_EOF);
	addFollow(COMP_STMT, TOKEN_SEMI_COLON);
	addFollow(COMP_STMT, TOKEN_FLOAT);
	addFollow(COMP_STMT, TOKEN_INT);
	addFollow(COMP_STMT, TOKEN_VOID);
	addFollow(COMP_STMT, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[VAR_DEC_LIST].nullable = 1;
	addFirst(VAR_DEC_LIST, TOKEN_FLOAT);
	addFirst(VAR_DEC_LIST, TOKEN_INT);
	addFollow(VAR_DEC_LIST, TOKEN_IDENTIFIER);
	addFollow(VAR_DEC_LIST, TOKEN_IF);
	addFollow(VAR_DEC_LIST, TOKEN_RETURN);
	addFollow(VAR_DEC_LIST, TOKEN_LEFT_CURLY_BRACES);

	varsArray[STMT_LIST].nullable = 0;
	addFirst(STMT_LIST, TOKEN_IDENTIFIER);
	addFirst(STMT_LIST, TOKEN_IF);
	addFirst(STMT_LIST, TOKEN_RETURN);
	addFirst(STMT_LIST, TOKEN_LEFT_CURLY_BRACES);
	addFollow(STMT_LIST, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[STMT_LIST_TAG].nullable = 1;
	addFirst(STMT_LIST_TAG, TOKEN_SEMI_COLON);
	addFollow(STMT_LIST_TAG, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[STMT].nullable = 0;
	addFirst(STMT, TOKEN_IDENTIFIER);
	addFirst(STMT, TOKEN_IF);
	addFirst(STMT, TOKEN_RETURN);
	addFirst(STMT, TOKEN_LEFT_CURLY_BRACES);
	addFollow(STMT, TOKEN_SEMI_COLON);
	addFollow(STMT, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[STMT_TAG].nullable = 0;
	addFirst(STMT_TAG, TOKEN_LEFT_PARENTHESES);
	addFirst(STMT_TAG, TOKEN_ASSIGNMENT);
	addFirst(STMT_TAG, TOKEN_LEFT_BRACKET);
	addFollow(STMT_TAG, TOKEN_SEMI_COLON);
	addFollow(STMT_TAG, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[IF_STMT].nullable = 0;
	addFirst(IF_STMT, TOKEN_IF);
	addFollow(IF_STMT, TOKEN_SEMI_COLON);
	addFollow(IF_STMT, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[ARGS].nullable = 1;
	addFirst(ARGS, TOKEN_LEFT_PARENTHESES);
	addFirst(ARGS, TOKEN_FLOAT_NUM);
	addFirst(ARGS, TOKEN_IDENTIFIER);
	addFirst(ARGS, TOKEN_INT_NUM);
	addFollow(ARGS, TOKEN_RIGHT_PARENTHESES);

	varsArray[ARG_LIST].nullable = 0;
	addFirst(ARG_LIST, TOKEN_LEFT_PARENTHESES);
	addFirst(ARG_LIST, TOKEN_FLOAT_NUM);
	addFirst(ARG_LIST, TOKEN_IDENTIFIER);
	addFirst(ARG_LIST, TOKEN_INT_NUM);
	addFollow(ARG_LIST, TOKEN_RIGHT_PARENTHESES);

	varsArray[ARG_LIST_TAG].nullable = 1;
	addFirst(ARG_LIST_TAG, TOKEN_COMMA);
	addFollow(ARG_LIST_TAG, TOKEN_RIGHT_PARENTHESES);

	varsArray[RETURN_STMT].nullable = 0;
	addFirst(RETURN_STMT, TOKEN_RETURN);
	addFollow(RETURN_STMT, TOKEN_SEMI_COLON);
	addFollow(RETURN_STMT, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[RETURN_STMT_TAG].nullable = 1;
	addFirst(RETURN_STMT_TAG, TOKEN_LEFT_PARENTHESES);
	addFirst(RETURN_STMT_TAG, TOKEN_FLOAT_NUM);
	addFirst(RETURN_STMT_TAG, TOKEN_IDENTIFIER);
	addFirst(RETURN_STMT_TAG, TOKEN_INT_NUM);
	addFollow(RETURN_STMT_TAG, TOKEN_SEMI_COLON);
	addFollow(RETURN_STMT_TAG, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[VAR_TAG].nullable = 1;
	addFirst(VAR_TAG, TOKEN_LEFT_BRACKET);
	addFollow(VAR_TAG, TOKEN_RIGHT_PARENTHESES);
	addFollow(VAR_TAG, TOKEN_MULTIPLACTION);
	addFollow(VAR_TAG, TOKEN_PLUS);
	addFollow(VAR_TAG, TOKEN_COMMA);
	addFollow(VAR_TAG, TOKEN_SEMI_COLON);
	addFollow(VAR_TAG, TOKEN_ASSIGNMENT);
	addFollow(VAR_TAG, TOKEN_RIGHT_BRACKET);
	addFollow(VAR_TAG, TOKEN_BELOW);
	addFollow(VAR_TAG, TOKEN_BELOW_EQUAL);
	addFollow(VAR_TAG, TOKEN_EQUAL);
	addFollow(VAR_TAG, TOKEN_ABOVE_EQUAL);
	addFollow(VAR_TAG, TOKEN_ABOVE);
	addFollow(VAR_TAG, TOKEN_NOT_EQUAL);
	addFollow(VAR_TAG, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[EXPR_LIST].nullable = 0;
	addFirst(EXPR_LIST, TOKEN_LEFT_PARENTHESES);
	addFirst(EXPR_LIST, TOKEN_FLOAT_NUM);
	addFirst(EXPR_LIST, TOKEN_IDENTIFIER);
	addFirst(EXPR_LIST, TOKEN_INT_NUM);
	addFollow(EXPR_LIST, TOKEN_RIGHT_BRACKET);

	varsArray[EXPR_LIST_TAG].nullable = 1;
	addFirst(EXPR_LIST_TAG, TOKEN_COMMA);
	addFollow(EXPR_LIST_TAG, TOKEN_RIGHT_BRACKET);

	varsArray[CONDITION].nullable = 0;
	addFirst(CONDITION, TOKEN_LEFT_PARENTHESES);
	addFirst(CONDITION, TOKEN_FLOAT_NUM);
	addFirst(CONDITION, TOKEN_IDENTIFIER);
	addFirst(CONDITION, TOKEN_INT_NUM);
	addFollow(CONDITION, TOKEN_RIGHT_PARENTHESES);

	varsArray[EXPR].nullable = 0;
	addFirst(EXPR, TOKEN_LEFT_PARENTHESES);
	addFirst(EXPR, TOKEN_FLOAT_NUM);
	addFirst(EXPR, TOKEN_IDENTIFIER);
	addFirst(EXPR, TOKEN_INT_NUM);
	addFollow(EXPR, TOKEN_RIGHT_PARENTHESES);
	addFollow(EXPR, TOKEN_COMMA);
	addFollow(EXPR, TOKEN_SEMI_COLON);
	addFollow(EXPR, TOKEN_RIGHT_BRACKET);
	addFollow(EXPR, TOKEN_BELOW);
	addFollow(EXPR, TOKEN_BELOW_EQUAL);
	addFollow(EXPR, TOKEN_EQUAL);
	addFollow(EXPR, TOKEN_ABOVE_EQUAL);
	addFollow(EXPR, TOKEN_ABOVE);
	addFollow(EXPR, TOKEN_NOT_EQUAL);
	addFollow(EXPR, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[EXPR_TAG].nullable = 1;
	addFirst(EXPR_TAG, TOKEN_PLUS);
	addFollow(EXPR_TAG, TOKEN_RIGHT_PARENTHESES);
	addFollow(EXPR_TAG, TOKEN_COMMA);
	addFollow(EXPR_TAG, TOKEN_SEMI_COLON);
	addFollow(EXPR_TAG, TOKEN_RIGHT_BRACKET);
	addFollow(EXPR_TAG, TOKEN_BELOW);
	addFollow(EXPR_TAG, TOKEN_BELOW_EQUAL);
	addFollow(EXPR_TAG, TOKEN_EQUAL);
	addFollow(EXPR_TAG, TOKEN_ABOVE_EQUAL);
	addFollow(EXPR_TAG, TOKEN_ABOVE);
	addFollow(EXPR_TAG, TOKEN_NOT_EQUAL);
	addFollow(EXPR_TAG, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[TERM].nullable = 0;
	addFirst(TERM, TOKEN_LEFT_PARENTHESES);
	addFirst(TERM, TOKEN_FLOAT_NUM);
	addFirst(TERM, TOKEN_IDENTIFIER);
	addFirst(TERM, TOKEN_INT_NUM);
	addFollow(TERM, TOKEN_RIGHT_PARENTHESES);
	addFollow(TERM, TOKEN_PLUS);
	addFollow(TERM, TOKEN_COMMA);
	addFollow(TERM, TOKEN_SEMI_COLON);
	addFollow(TERM, TOKEN_RIGHT_BRACKET);
	addFollow(TERM, TOKEN_BELOW);
	addFollow(TERM, TOKEN_BELOW_EQUAL);
	addFollow(TERM, TOKEN_EQUAL);
	addFollow(TERM, TOKEN_ABOVE_EQUAL);
	addFollow(TERM, TOKEN_ABOVE);
	addFollow(TERM, TOKEN_NOT_EQUAL);
	addFollow(TERM, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[TERM_TAG].nullable = 1;
	addFirst(TERM_TAG, TOKEN_MULTIPLACTION);
	addFollow(TERM_TAG, TOKEN_RIGHT_PARENTHESES);
	addFollow(TERM_TAG, TOKEN_PLUS);
	addFollow(TERM_TAG, TOKEN_COMMA);
	addFollow(TERM_TAG, TOKEN_SEMI_COLON);
	addFollow(TERM_TAG, TOKEN_RIGHT_BRACKET);
	addFollow(TERM_TAG, TOKEN_BELOW);
	addFollow(TERM_TAG, TOKEN_BELOW_EQUAL);
	addFollow(TERM_TAG, TOKEN_EQUAL);
	addFollow(TERM_TAG, TOKEN_ABOVE_EQUAL);
	addFollow(TERM_TAG, TOKEN_ABOVE);
	addFollow(TERM_TAG, TOKEN_NOT_EQUAL);
	addFollow(TERM_TAG, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[FACTOR].nullable = 0;
	addFirst(FACTOR, TOKEN_LEFT_PARENTHESES);
	addFirst(FACTOR, TOKEN_FLOAT_NUM);
	addFirst(FACTOR, TOKEN_IDENTIFIER);
	addFirst(FACTOR, TOKEN_INT_NUM);
	addFollow(FACTOR, TOKEN_RIGHT_PARENTHESES);
	addFollow(FACTOR, TOKEN_MULTIPLACTION);
	addFollow(FACTOR, TOKEN_PLUS);
	addFollow(FACTOR, TOKEN_COMMA);
	addFollow(FACTOR, TOKEN_SEMI_COLON);
	addFollow(FACTOR, TOKEN_RIGHT_BRACKET);
	addFollow(FACTOR, TOKEN_BELOW);
	addFollow(FACTOR, TOKEN_BELOW_EQUAL);
	addFollow(FACTOR, TOKEN_EQUAL);
	addFollow(FACTOR, TOKEN_ABOVE_EQUAL);
	addFollow(FACTOR, TOKEN_ABOVE);
	addFollow(FACTOR, TOKEN_NOT_EQUAL);
	addFollow(FACTOR, TOKEN_RIGHT_CURLY_BRACES);

	varsArray[FACTOR_TAG].nullable = 1;
	addFirst(FACTOR_TAG, TOKEN_LEFT_PARENTHESES);
	addFirst(FACTOR_TAG, TOKEN_LEFT_BRACKET);
	addFollow(FACTOR_TAG, TOKEN_RIGHT_PARENTHESES);
	addFollow(FACTOR_TAG, TOKEN_MULTIPLACTION);
	addFollow(FACTOR_TAG, TOKEN_PLUS);
	addFollow(FACTOR_TAG, TOKEN_COMMA);
	addFollow(FACTOR_TAG, TOKEN_SEMI_COLON);
	addFollow(FACTOR_TAG, TOKEN_RIGHT_BRACKET);
	addFollow(FACTOR_TAG, TOKEN_BELOW);
	addFollow(FACTOR_TAG, TOKEN_BELOW_EQUAL);
	addFollow(FACTOR_TAG, TOKEN_EQUAL);
	addFollow(FACTOR_TAG, TOKEN_ABOVE_EQUAL);
	addFollow(FACTOR_TAG, TOKEN_ABOVE);
	addFollow(FACTOR_TAG, TOKEN_NOT_EQUAL);
	addFollow(FACTOR_TAG, TOKEN_RIGHT_CURLY_BRACES);
}

//parse functions for all the variables,
//each function implements the grammar of its variable

//first parse function
void parse_PROG()
{
	fprintf(yyoutSyntactic, "Rule (PROG -> GLOBAL_VARS FUNC_PREDEFS FUNC_FULL_DEFS)\n");
	currVarIndex = PROG;
	curr_table = makeTable(curr_table);
	func_flag = 0;
	int return_flag = 1;
	parse_GLOBAL_VARS();
	parse_FUNC_PREDEFS();
	parse_FUNC_FULL_DEFS();
	Table_Node* ptr = (Table_Node*)(malloc(sizeof(Table_Node)));
	ptr = curr_table->nodeHead;
	while (ptr != NULL)
	{
		if (ptr->flag_used == 0)
		{
			if (ptr->id_type == 0)
				fprintf(yyoutSemantic, "Note: The global variable %s has been declared at line %d but never got used \n", ptr->name, ptr->lineDeclareted);
			else if (ptr->id_type == 1)
				fprintf(yyoutSemantic, "Note: The global array %s has been declared at line %d but never got used \n", ptr->name, ptr->lineDeclareted);
			else if (ptr->id_type == 2)
				fprintf(yyoutSemantic, "Note: The function %s has been declared at line %d but never got implemented \n", ptr->name, ptr->lineDeclareted);
		}
		ptr = ptr->next;
	}
	curr_table = popTable(curr_table);
	matchToken(TOKEN_EOF);
}

void parse_GLOBAL_VARS()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (GLOBAL_VARS -> VAR_DEC GLOBAL_VARS')\n");
	currVarIndex = GLOBAL_VARS;
	parse_VAR_DEC();
	parse_GLOBAL_VARS_TAG();
	currVarIndex = prevVarIndex;
}

void parse_GLOBAL_VARS_TAG()
{
	Token temp_token;
	int prevVarIndex = currVarIndex;
	int k;
	currVarIndex = GLOBAL_VARS_TAG;
	// Perform lookahead by taking 3 steps forward and see what's next
	for (k = 0; k < 3; k++) {
		curr_token = next_token();
	}

	if(curr_token->kind == TOKEN_LEFT_PARENTHESES)
	{
		for (k = 0; k < 3; k++) {
			curr_token = back_token();
		}
		fprintf(yyoutSyntactic, "Rule (GLOBAL_VARS' -> EPSILON)\n");
	}
	else
	{
		for (k = 0; k < 2; k++) {
			curr_token = back_token();
		}
		switch (curr_token->kind)
		{
		case TOKEN_FLOAT:
		case TOKEN_INT:
			fprintf(yyoutSyntactic, "Rule (GLOBAL_VARS' -> VAR_DEC GLOBAL_VARS')\n");
			curr_token = back_token();
			parse_VAR_DEC();
			parse_GLOBAL_VARS_TAG();
			break;
		case(TOKEN_VOID):
			fprintf(yyoutSyntactic, "Rule (GLOBAL_VARS' -> EPSILON)\n");
			curr_token = back_token();
			break;
		default:
			printUnexpectedToken();
			RecoveryAfterError();
		}
	}
	currVarIndex = prevVarIndex;
}

void parse_VAR_DEC()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (VAR_DEC  ->  TYPE  id   VAR_DEC')\n");
	currVarIndex = VAR_DEC;
	parse_TYPE();
	if (matchToken(TOKEN_IDENTIFIER))
	{
		if ((id_entry = insert(curr_table, curr_token->lexeme)) != NULL)
		{
			setType(id_entry, curr_type);
			setLineDeclareted(id_entry, curr_token->lineNumber);
			id_entry->flag_used = 0;
		}
		else
			fprintf(yyoutSemantic, "Error: Duplicated declaration within same scope of %s at line %d \n", curr_token->lexeme, curr_token->lineNumber);
	}
	parse_VAR_DEC_TAG();
	currVarIndex = prevVarIndex;
}

void parse_VAR_DEC_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = GLOBAL_VARS_TAG;
	curr_token = next_token();
	Dim_Size_List* listSize;
	switch (curr_token->kind)
	{
	case TOKEN_SEMI_COLON:
	{
		if (id_entry != NULL)
		{
			setIdType(id_entry, 0);
		}
		fprintf(yyoutSyntactic, "Rule (VAR_DEC' -> ;)\n");
		break;
	}
	case TOKEN_LEFT_BRACKET:
	{
		fprintf(yyoutSyntactic, "Rule (VAR_DEC' -> [ DIM_SIZES ] ;)\n");
		if (id_entry != NULL)
		{
			setIdType(id_entry, 1);
			listSize = parse_DIM_SIZES();
			id_entry->sizes = listSize;
		}
		else
		{
			listSize = parse_DIM_SIZES();
		}
		matchToken(TOKEN_RIGHT_BRACKET);
		matchToken(TOKEN_SEMI_COLON);
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

void parse_TYPE()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = TYPE;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_INT:
	{
		fprintf(yyoutSyntactic, "Rule (TYPE -> int)\n");
		curr_type = curr_token->kind;
		break;
	}
	case TOKEN_FLOAT:
	{
		fprintf(yyoutSyntactic, "Rule (TYPE -> float)\n");
		curr_type = curr_token->kind;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

Dim_Size_List* parse_DIM_SIZES()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = DIM_SIZES;
	Dim_Size_List* listSize = (Dim_Size_List*)(malloc(sizeof(Dim_Size_List)));
	Dim_Size_List* nodeSize = (Dim_Size_List*)(malloc(sizeof(Dim_Size_List)));
	curr_token = next_token();
	if (TOKEN_INT_NUM == curr_token->kind)
	{
		listSize->head = (Dim_Size_Node*)(malloc(sizeof(Dim_Size_Node)));
		listSize->head->size = atoi(curr_token->lexeme);
		fprintf(yyoutSyntactic, "Rule (DIM_SIZES -> int_num DIM_SIZES')\n");
		nodeSize = parse_DIM_SIZES_TAG();
		if (nodeSize != NULL)
		{
			listSize->head->next = (Dim_Size_Node*)(malloc(sizeof(Dim_Size_Node)));
			listSize->head->next = nodeSize->head;
		}
		else
		{
			listSize->head->next = NULL;
		}
	}
	else
	{
		fprintf(yyoutSemantic, "Error: Array dimensions must be a natural number at line %d \n", curr_token->lineNumber);
		printUnexpectedToken();
		RecoveryAfterError();
	}
	currVarIndex = prevVarIndex;
	return listSize;
}

Dim_Size_List* parse_DIM_SIZES_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = DIM_SIZES_TAG;
	Dim_Size_List* listSize = (Dim_Size_List*)(malloc(sizeof(Dim_Size_List)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_COMMA:
	{
		fprintf(yyoutSyntactic, "Rule (DIM_SIZES' -> , DIM_SIZES)\n");
		listSize = parse_DIM_SIZES();
		break;
	}
	case TOKEN_RIGHT_BRACKET:
	{
		fprintf(yyoutSyntactic, "Rule (DIM_SIZES' -> EPSILON)\n");
		curr_token = back_token();
		listSize = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return listSize;
}

void parse_FUNC_PREDEFS()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (FUNC_PREDEFS -> FUNC_PROTOTYPE ; FUNC_PREDEFS')\n");
	currVarIndex = FUNC_PREDEFS;
	parse_FUNC_PROTOTYPE();
	matchToken(TOKEN_SEMI_COLON);
	parse_FUNC_PREDEFS_TAG();
	currVarIndex = prevVarIndex;
}

void parse_FUNC_PREDEFS_TAG()
{
	int prevVarIndex = currVarIndex;
	int countNextTokens = 0;
	curr_token = next_token();
	currVarIndex = FUNC_PREDEFS_TAG;
	back_token();
	switch (curr_token->kind)
	{
	case TOKEN_INT:
	case TOKEN_FLOAT:
	case TOKEN_VOID:
		// Perform lookahead with unlimited steps forward in order to see what's next
		while (curr_token->kind != TOKEN_SEMI_COLON && curr_token->kind != TOKEN_LEFT_CURLY_BRACES && curr_token->kind != TOKEN_EOF)
		{
			countNextTokens++;
			curr_token = next_token();
		}

		for (int i = 0; i < countNextTokens; i++)
		{
			back_token();
		}
		if (curr_token->kind == TOKEN_SEMI_COLON)
		{
			fprintf(yyoutSyntactic, "Rule (FUNC_PREDEFS' -> FUNC_PROTOTYPE ; FUNC_PREDEFS')\n");
			parse_FUNC_PROTOTYPE();
			matchToken(TOKEN_SEMI_COLON);
			parse_FUNC_PREDEFS_TAG();
			break;
		}
		else if (curr_token->kind == TOKEN_LEFT_CURLY_BRACES)
		{
			fprintf(yyoutSyntactic, "Rule (FUNC_PREDEFS' -> EPSILON)\n");
			break;
		}
		break;
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

void parse_FUNC_PROTOTYPE()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (FUNC_PROTOTYPE -> RETURNED_TYPE id ( PARAMS ))\n");
	currVarIndex = FUNC_PROTOTYPE;
	Param_List* param_head = (Param_List*)(malloc(sizeof(Param_List)));
	parse_RETURNED_TYPE();
	if (matchToken(TOKEN_IDENTIFIER))
	{
		if (func_flag) //in case: full function
		{
			id_entry = lookup(curr_table, curr_token->lexeme);

			if (id_entry == NULL) //if no declered to specific function
			{
				id_entry = insert(curr_table, curr_token->lexeme);
				matchToken(TOKEN_LEFT_PARENTHESES);
				setType(id_entry, curr_type);
				param_head = parse_PARAMS();
				setIdType(id_entry, 2);
				setLineDeclareted(id_entry, curr_token->lineNumber);
				setIdListParameterTypes(id_entry, param_head);
				id_entry->flag_used = 1;
			}
			else if (id_entry->id_type != 2) //if is not function (and no var/arry)
			{
				fprintf(yyoutSemantic, "Error: The function %s at line %d has not been declared \n", id_entry->name, curr_token->lineNumber);
				matchToken(TOKEN_LEFT_PARENTHESES);
				param_head = parse_PARAMS();
			}
			else
			{
				if (strcmp(getKindStr(curr_type), getKindStr(id_entry->type)) != 0) //if return the same type
				{
					fprintf(yyoutSemantic, "Error: The function %s at line %d has different return value and therefore it's not declared \n", id_entry->name, curr_token->lineNumber);
					id_entry->type = curr_type; //update the return value (if different from decleration)
				}
				matchToken(TOKEN_LEFT_PARENTHESES);
				param_head = parse_PARAMS();
				if (!(isEqualParamAmount(param_head, id_entry->listParameterTypes)))
				{
					fprintf(yyoutSemantic, "Error: The function %s at line %d has different parameter list and therefore it's not declared \n", id_entry->name, curr_token->lineNumber);
					setIdListParameterTypes(id_entry, param_head); //update the parameters (if different from decleration)
				}
				id_entry->flag_used = 1; // the function declered + implemented
				setLineDeclareted(id_entry, curr_token->lineNumber);
			}
		}
		else //in case: function decleration
		{
			if ((id_entry = insert(curr_table, curr_token->lexeme)) != NULL)
			{

				matchToken(TOKEN_LEFT_PARENTHESES);
				setType(id_entry, curr_type);
				param_head = parse_PARAMS();
				setIdType(id_entry, 2);
				setLineDeclareted(id_entry, curr_token->lineNumber);
				setIdListParameterTypes(id_entry, param_head);
				id_entry->flag_used = 0;
			}
			else
			{
				fprintf(yyoutSemantic, "Error: Duplicated declaration within same scope of %s at line %d \n", curr_token->lexeme, curr_token->lineNumber);
				matchToken(TOKEN_LEFT_PARENTHESES);
				param_head = parse_PARAMS();
			}
		}
	}
	matchToken(TOKEN_RIGHT_PARENTHESES);
	currVarIndex = prevVarIndex;
}

void parse_FUNC_FULL_DEFS()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (FUNC_FULL_DEFS -> FUNC_WITH_BODY FUNC_FULL_DEFS')\n");
	currVarIndex = FUNC_FULL_DEFS;
	func_flag = 1;
	parse_FUNC_WITH_BODY();
	parse_FUNC_FULL_DEFS_TAG();
	currVarIndex = prevVarIndex;
}

void parse_FUNC_FULL_DEFS_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = FUNC_FULL_DEFS_TAG;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_FLOAT:
	case TOKEN_INT:
	case TOKEN_VOID:
	{
		fprintf(yyoutSyntactic, "Rule (FUNC_FULL_DEFS' -> FUNC_FULL_DEFS)\n");
		curr_token = back_token();
		parse_FUNC_FULL_DEFS();
		break;
	}
	case TOKEN_EOF:
	{
		fprintf(yyoutSyntactic, "Rule (FUNC_FULL_DEFS' -> EPSILON)\n");
		curr_token = back_token();
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

void parse_FUNC_WITH_BODY()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (FUNC_WITH_BODY -> FUNC_PROTOTYPE COMP_STMT)\n");
	currVarIndex = FUNC_FULL_DEFS;
	parse_FUNC_PROTOTYPE();
	parse_COMP_STMT();
	currVarIndex = prevVarIndex;
}

void parse_RETURNED_TYPE()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = RETURNED_TYPE;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_FLOAT:
	case TOKEN_INT:
	{
		fprintf(yyoutSyntactic, "Rule (RETURNED_TYPE -> TYPE)\n");
		curr_token = back_token();
		parse_TYPE();
		break;
	}
	case TOKEN_VOID:
	{
		fprintf(yyoutSyntactic, "Rule (RETURNED_TYPE -> void)\n");
		curr_type = TOKEN_VOID;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

Param_List* parse_PARAMS()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = PARAMS;
	Param_List* param_head;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_FLOAT:
	case TOKEN_INT:
	{
		fprintf(yyoutSyntactic, "Rule (PARAMS -> PARAM_LIST)\n");
		curr_token = back_token();
		param_head = parse_PARAM_LIST();
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	{
		fprintf(yyoutSyntactic, "Rule (PARAMS -> EPSILON)\n");
		curr_token = back_token();
		param_head = NULL;
		break;
	}
	default:
	{
		param_head = NULL;
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return param_head;
}

Param_List* parse_PARAM_LIST()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (PARAM_LIST -> PARAM PARAM_LIST')\n");
	currVarIndex = PARAM_LIST;
	Param_List* param_head = (Param_List*)(malloc(sizeof(Param_List)));
	param_head->head = parse_PARAM();
	param_head->head->next = NULL;
	param_head->head->prev = NULL;
	param_head->head->next = parse_PARAM_LIST_TAG()->head;
	if (isExistsParam(param_head, param_head->head) == 0) //node exsists in list
		fprintf(yyoutSemantic, "Error: Duplicated parameter name in function at line %d \n", curr_token->lineNumber);
	currVarIndex = prevVarIndex;
	return param_head;
}

Param_List* parse_PARAM_LIST_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = PARAM_LIST_TAG;
	Param_List* param = (Param_List*)(malloc(sizeof(Param_List)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_COMMA:
	{
		fprintf(yyoutSyntactic, "Rule (PARAM_LIST' -> , PARAM PARAM_LIST')\n");
		param->head = parse_PARAM();
		param->head->next = NULL;
		param->head->next = parse_PARAM_LIST_TAG()->head;
		if (isExistsParam(param, param->head) == 0) //node exsists in list
		{
			fprintf(yyoutSemantic, "Error: Duplicated parameter name in function at line %d \n", curr_token->lineNumber);
		}
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	{
		fprintf(yyoutSyntactic, "Rule (PARAM_LIST' -> EPSILON)\n");
		curr_token = back_token();
		param->head = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return param;
}

Param_Node* parse_PARAM()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (PARAM -> TYPE id PARAM')\n");
	currVarIndex = PARAM;
	Param_Node* param_node = (Param_Node*)(malloc(sizeof(Param_Node)));
	Dim_Size_List* listSize;
	parse_TYPE();
	if (matchToken(TOKEN_IDENTIFIER))
	{
		curr_token = back_token();
		param_node->type = curr_token->kind;
		curr_token = next_token();
		param_node->name = curr_token->lexeme;
	}
	listSize = parse_PARAM_TAG();
	param_node->dimSize = listSize;
	if (listSize == NULL)
		param_node->id_type = 0;
	else
		param_node->id_type = 1;
	currVarIndex = prevVarIndex;
	return param_node;
}

Dim_Size_List* parse_PARAM_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = PARAM_TAG;
	Dim_Size_List* listSize = (Dim_Size_List*)(malloc(sizeof(Dim_Size_List)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_LEFT_BRACKET:
	{
		fprintf(yyoutSyntactic, "Rule (PARAM' -> [ DIM_SIZES ])\n");
		listSize = parse_DIM_SIZES();
		matchToken(TOKEN_RIGHT_BRACKET);
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	case TOKEN_COMMA:
	{
		fprintf(yyoutSyntactic, "Rule (PARAM' -> EPSILON)\n");
		curr_token = back_token();
		listSize = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return listSize;
}

void parse_COMP_STMT()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = COMP_STMT;
	Table_Node* ptr = (Table_Node*)(malloc(sizeof(Table_Node)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_LEFT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (COMP_STMT -> { VAR_DEC_LIST STMT_LIST })\n");
		curr_table = makeTable(curr_table);
		if (id_entry != NULL)
		{
			if (id_entry->id_type == 2)
			{
				curr_table->id_type = 2;
				returned_type = id_entry->type;
				if (strcmp(getKindStr(returned_type), getKindStr(TOKEN_VOID)) != 0)
					return_flag = 0;
				int line = id_entry->lineDeclareted;
				if (id_entry->listParameterTypes != NULL)
				{
					Param_Node* listParamter = id_entry->listParameterTypes->head;
					while (listParamter != NULL)
					{
						if ((id_entry = insert(curr_table, listParamter->name)) != NULL)
						{
							setType(id_entry, listParamter->type);
							if (listParamter->id_type == 0)
								setIdType(id_entry, 0);
							else
							{
								setIdType(id_entry, 1);
								id_entry->sizes = listParamter->dimSize;
							}
							id_entry->lineDeclareted = line;
						}
						listParamter = listParamter->next;
					}
				}
			}
			else
			{
				curr_table->id_type = 0;
			}
		}
		else
		{
			curr_table->id_type = 0;
		}
		parse_VAR_DEC_LIST();
		parse_STMT_LIST();

		matchToken(TOKEN_RIGHT_CURLY_BRACES);

		if (curr_table->id_type == 2)
		{
			if (return_flag == 0)
			{
				fprintf(yyoutSemantic, "Error: The function must return a value at line %d\n", curr_token->lineNumber);
			}
		}
		return_flag = 1;
		ptr = curr_table->nodeHead;
		while (ptr != NULL)
		{
			if (ptr->flag_used == 0)
			{
				if (ptr->id_type == 0)
					fprintf(yyoutSemantic, "Note: The variable %s has been declared at line %d but never got used \n", ptr->name, ptr->lineDeclareted);
				else if (ptr->id_type == 1)
					fprintf(yyoutSemantic, "Note: The array %s has been declared at line %d but never got used \n", ptr->name, ptr->lineDeclareted);
			}
			ptr = ptr->next;
		}
		curr_table = popTable(curr_table);
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

void parse_VAR_DEC_LIST()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = VAR_DEC_LIST;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_FLOAT:
	case TOKEN_INT:
	{
		fprintf(yyoutSyntactic, "Rule (VAR_DEC_LIST -> VAR_DEC VAR_DEC_LIST)\n");
		curr_token = back_token();
		parse_VAR_DEC();
		parse_VAR_DEC_LIST();
		break;
	}
	case TOKEN_IDENTIFIER:
	case TOKEN_IF:
	case TOKEN_RETURN:
	case TOKEN_LEFT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (VAR_DEC_LIST -> EPSILON)\n");
		curr_token = back_token();
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

void parse_STMT_LIST()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (STMT_LIST -> STMT STMT_LIST')\n");
	currVarIndex = STMT_LIST;
	parse_STMT();
	parse_STMT_LIST_TAG();
	currVarIndex = prevVarIndex;
}

void parse_STMT_LIST_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = STMT_LIST_TAG;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_SEMI_COLON:
	{
		fprintf(yyoutSyntactic, "Rule (STMT_LIST' -> ; STMT STMT_LIST')\n");
		parse_STMT();
		parse_STMT_LIST_TAG();
		break;
	}
	case TOKEN_RIGHT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (STMT_LIST' -> EPSILON)\n");
		curr_token = back_token();
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

void parse_STMT()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = STMT;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_IDENTIFIER:
	{
		fprintf(yyoutSyntactic, "Rule (STMT -> id STMT')\n");
		id_entry = find(curr_table, curr_token->lexeme);
		if (id_entry != NULL)
		{
			setFlagUsed(id_entry);
		}
		else
		{
			fprintf(yyoutSemantic, "Error: The identifier %s at line %d has not been declared \n", curr_token->lexeme, curr_token->lineNumber);
		}
		parse_STMT_TAG();
		break;
	}
	case TOKEN_LEFT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (STMT-> COMP_STMT)\n");
		curr_token = back_token();
		parse_COMP_STMT();
		break;
	}
	case TOKEN_IF:
	{
		fprintf(yyoutSyntactic, "Rule (STMT -> IF_STMT)\n");
		curr_token = back_token();
		parse_IF_STMT();
		break;
	}
	case TOKEN_RETURN:
	{
		fprintf(yyoutSyntactic, "Rule (STMT -> RETURN_STMT)\n");
		curr_token = back_token();
		parse_RETURN_STMT();
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

void parse_STMT_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = STMT_TAG;
	Param_Node *param1, *param2;
	param1 = (Param_Node*)(malloc(sizeof(Param_Node)));
	param2 = (Param_Node*)(malloc(sizeof(Param_Node)));
	Param_List* head = (Param_List*)(malloc(sizeof(Param_List)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_ASSIGNMENT:
	case TOKEN_LEFT_BRACKET:
	{
		fprintf(yyoutSyntactic, "Rule (STMT' -> VAR' = EXPR)\n");
		curr_token = back_token();
		param1 = parse_VAR_TAG();
		matchToken(TOKEN_ASSIGNMENT);
		param2 = parse_EXPR();
		if (param1 == NULL || param2 == NULL)
			fprintf(yyoutSemantic, "Error: illegal assignment at line %d \n", curr_token->lineNumber);
		else
		{
			if ((strcmp(getKindStr(param1->type), getKindStr(TOKEN_INT)) == 0) && (strcmp(getKindStr(param2->type), getKindStr(TOKEN_FLOAT)) == 0))

			{
				fprintf(yyoutSemantic, "Error: illegal assignment by trying to put float into int variable at line %d \n", curr_token->lineNumber);
			}
			else
			{
				param1->value = param2->value;
				id_entry = find(curr_table, param1->name);
				if (id_entry != NULL)
				{
					id_entry->value = param1->value;
				}
			}
		}
		break;
	}
	case TOKEN_LEFT_PARENTHESES:
	{
		fprintf(yyoutSyntactic, "Rule (STMT'-> ( ARGS ))\n");
		head = parse_ARGS();
		matchToken(TOKEN_RIGHT_PARENTHESES);
		if (id_entry != NULL)
		{
			if (id_entry->id_type != 2) //function
				fprintf(yyoutSemantic, "Error: illegal assignment into variable or array %s at line %d \n", id_entry->name, curr_token->lineNumber);
			else
			{
				if (id_entry->flag_used == 0)
				{
					fprintf(yyoutSemantic, "Error: The function %s at line %d has no implementation \n", id_entry->name, curr_token->lineNumber);
				}
				else if (compareParamAmount(head, id_entry->listParameterTypes) == 0)
				{
					fprintf(yyoutSemantic, "Error: Wrong amount of parameters was sent to function %s at line %d \n", id_entry->name, curr_token->lineNumber);
				}
				else if (isEqualParamAmount(head, id_entry->listParameterTypes) == 0)
				{
					fprintf(yyoutSemantic, "Error: The function %s at line %d has at least one unmatch parameter type \n", id_entry->name, curr_token->lineNumber);
				}
			}
		}
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

void parse_IF_STMT()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (IF_STMT -> if ( CONDITION ) STMT)\n");
	currVarIndex = IF_STMT;
	matchToken(TOKEN_IF);
	matchToken(TOKEN_LEFT_PARENTHESES);
	parse_CONDITION();
	matchToken(TOKEN_RIGHT_PARENTHESES);
	parse_STMT();
	currVarIndex = prevVarIndex;
}

Param_List* parse_ARGS()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = ARGS;
	Param_List* head = (Param_List*)(malloc(sizeof(Param_List)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_LEFT_PARENTHESES:
	case TOKEN_FLOAT_NUM:
	case TOKEN_IDENTIFIER:
	case TOKEN_INT_NUM:
	{
		fprintf(yyoutSyntactic, "Rule (ARGS -> ARG_LIST)\n");
		curr_token = back_token();
		head = parse_ARG_LIST();
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	{
		fprintf(yyoutSyntactic, "Rule (ARGS -> EPSILON)\n");
		curr_token = back_token();
		head = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return head;
}

Param_List* parse_ARG_LIST()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (ARG_LIST -> EXPR ARG_LIST')\n");
	currVarIndex = ARG_LIST;
	Param_List* head = (Param_List*)(malloc(sizeof(Param_List)));
	head->head = parse_EXPR();
	if (head->head != NULL)
	{
		head->head->next = NULL;
		head->head->prev = NULL;
		head->head->next = parse_ARG_LIST_TAG()->head;
	}
	else
	{
		parse_ARG_LIST_TAG();
		if (head->head != NULL)
			head->head->next = NULL;
	}
	currVarIndex = prevVarIndex;
	return head;
}

Param_List* parse_ARG_LIST_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = ARG_LIST_TAG;
	Param_List* head = (Param_List*)(malloc(sizeof(Param_List)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_COMMA:
	{
		fprintf(yyoutSyntactic, "Rule (ARG_LIST' -> , EXPR ARG_LIST')\n");
		head->head = parse_EXPR();
		if (head->head != NULL)
		{
			head->head->next = NULL;
			head->head->next = parse_ARG_LIST_TAG()->head;
		}
		else
		{
			parse_ARG_LIST_TAG();
			if (head->head != NULL)
				head->head->next = NULL;
		}
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	{
		fprintf(yyoutSyntactic, "Rule (ARG_LIST' -> EPSILON)\n");
		curr_token = back_token();
		head->head = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return head;
}

void parse_RETURN_STMT()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = RETURN_STMT;
	Param_Node* param = (Param_Node*)(malloc(sizeof(Param_Node)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_RETURN:
	{
		fprintf(yyoutSyntactic, "Rule (RETURN_STMT -> return RETURN_STMT')\n");
		return_flag = 1;
		param = parse_RETURN_STMT_TAG();
		if (param == NULL)
		{
			if ((strcmp(getKindStr(returned_type), getKindStr(TOKEN_VOID)) == 0))//O.K.
			{
			}
			else if (((strcmp(getKindStr(returned_type), getKindStr(TOKEN_INT)) == 0) || (strcmp(getKindStr(returned_type), getKindStr(TOKEN_FLOAT)) == 0)))
			{
				fprintf(yyoutSemantic, "Error: A value must be returned at line %d \n", curr_token->lineNumber);
			}
		}
		else
		{
			if (strcmp(getKindStr(param->type), getKindStr(returned_type)) == 0)//O.K.
			{
			}
			else if (((strcmp(getKindStr(param->type), getKindStr(TOKEN_INT)) == 0)) && (strcmp(getKindStr(returned_type), getKindStr(TOKEN_FLOAT)) == 0))
			{
				fprintf(yyoutSemantic, "Error: A float value must be returned at line %d \n", curr_token->lineNumber);
			}
			else if (((strcmp(getKindStr(param->type), getKindStr(TOKEN_FLOAT)) == 0)) && (strcmp(getKindStr(returned_type), getKindStr(TOKEN_INT)) == 0))
			{
				fprintf(yyoutSemantic, "Error: An int value must be returned at line %d \n", curr_token->lineNumber);
			}
			else if (((strcmp(getKindStr(param->type), getKindStr(TOKEN_FLOAT)) == 0) || (strcmp(getKindStr(param->type), getKindStr(TOKEN_INT)) == 0)) && (strcmp(getKindStr(returned_type), getKindStr(TOKEN_VOID)) == 0))
			{
				fprintf(yyoutSemantic, "Error: Cannot return a value at line %d \n", curr_token->lineNumber);
			}
		}
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
}

Param_Node* parse_RETURN_STMT_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = RETURN_STMT_TAG;
	curr_token = next_token();
	Param_Node* param = (Param_Node*)(malloc(sizeof(Param_Node)));
	switch (curr_token->kind)
	{
	case TOKEN_LEFT_PARENTHESES:
	case TOKEN_FLOAT_NUM:
	case TOKEN_IDENTIFIER:
	case TOKEN_INT_NUM:
	{
		fprintf(yyoutSyntactic, "Rule (RETURN_STMT' -> EXPR)\n");
		curr_token = back_token();
		param = parse_EXPR();
		break;
	}
	case TOKEN_SEMI_COLON:
	case TOKEN_RIGHT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (RETURN_STMT' -> EPSILON)\n");
		curr_token = back_token();
		param = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return param;
}

Param_Node* parse_VAR_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = VAR_TAG;
	Param_Node* param = (Param_Node*)(malloc(sizeof(Param_Node)));
	Dim_Size_List* size;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_LEFT_BRACKET:
	{
		fprintf(yyoutSyntactic, "Rule (VAR' -> [ EXPR_LIST ])\n");
		size = parse_EXPR_LIST();
		if (id_entry != NULL)
		{
			if (id_entry->id_type != 1) //if not array
			{
				fprintf(yyoutSemantic, "Error: illegal assignment into variable or function %s at line %d \n", id_entry->name, curr_token->lineNumber);
				param = NULL;
			}
			else
			{
				if (compareSizes(id_entry->sizes, size) == 0)
				{
					fprintf(yyoutSemantic, "Error: Out of bounds assignment to the array %s at line %d \n", id_entry->name, curr_token->lineNumber);
					param = NULL;
				}
				else if (compareSizes(id_entry->sizes, size) == 2)
				{
					fprintf(yyoutSemantic, "Error: Wrong amount of dimensions of the array %s at line %d \n", id_entry->name, curr_token->lineNumber);
					param = NULL;
				}
				else
				{
					param->type = id_entry->type;
					param->name = id_entry->name;
				}
			}
		}
		else
		{
			param = NULL;
		}
		matchToken(TOKEN_RIGHT_BRACKET);
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	case TOKEN_MULTIPLACTION:
	case TOKEN_PLUS:
	case TOKEN_COMMA:
	case TOKEN_SEMI_COLON:
	case TOKEN_ASSIGNMENT:
	case TOKEN_RIGHT_BRACKET:
	case TOKEN_BELOW:
	case TOKEN_BELOW_EQUAL:
	case TOKEN_EQUAL:
	case TOKEN_ABOVE_EQUAL:
	case TOKEN_ABOVE:
	case TOKEN_NOT_EQUAL:
	case TOKEN_RIGHT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (VAR' -> EPSILON)\n");
		if (id_entry != NULL)
		{
			if (id_entry->id_type != 0) //if not variable
			{
				fprintf(yyoutSemantic, "Error: illegal assignment into array or function %s at line %d \n", id_entry->name, curr_token->lineNumber);
				param = NULL;
			}
			else
			{
				param->name = (char*)(calloc(strlen(id_entry->name), sizeof(char)));
				param->name = id_entry->name;
				param->type = id_entry->type;
				param->value = id_entry->value;
			}
		}
		else
		{
			param = NULL;
		}
		curr_token = back_token();
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return param;
}

Dim_Size_List* parse_EXPR_LIST()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (EXPR_LIST -> EXPR EXPR_LIST')\n");
	currVarIndex = EXPR_LIST;
	Param_Node* param = (Param_Node*)(malloc(sizeof(Param_Node)));
	Dim_Size_List* sizes = (Dim_Size_List*)(malloc(sizeof(Dim_Size_List)));
	sizes->head = NULL;
	param = parse_EXPR();
	if (param != NULL)
	{
		if (strcmp(getKindStr(param->type), getKindStr(TOKEN_FLOAT)) == 0)
		{
			if (id_entry != NULL)
			{
				fprintf(yyoutSemantic, "Error: The array %s size must be a natural number at line %d \n", id_entry->name, curr_token->lineNumber);
			}
			else
			{
				fprintf(yyoutSemantic, "Error: The array size must be a natural number at line %d \n", curr_token->lineNumber);
			}
			sizes = parse_EXPR_LIST_TAG();
		}
		else
		{
			insertDimSizeList(sizes, (int)(param->value));
			sizes->head->next = parse_EXPR_LIST_TAG()->head;
		}
	}
	else
	{
		parse_EXPR_LIST_TAG();
		sizes = NULL;
	}
	currVarIndex = prevVarIndex;
	return sizes;
}

Dim_Size_List* parse_EXPR_LIST_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = EXPR_LIST_TAG;
	Param_Node* param = (Param_Node*)(malloc(sizeof(Param_Node)));
	Dim_Size_List* sizes = (Dim_Size_List*)(malloc(sizeof(Dim_Size_List)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_COMMA:
	{
		fprintf(yyoutSyntactic, "Rule (EXPR_LIST' -> , EXPR EXPR_LIST')\n");
		param = parse_EXPR();
		if (strcmp(getKindStr(param->type), getKindStr(TOKEN_FLOAT)) == 0)
		{
			if (id_entry != NULL)
			{
				fprintf(yyoutSemantic, "Error: The array %s size must be a natural number at line %d \n", id_entry->name, curr_token->lineNumber);
			}
			else
			{
				fprintf(yyoutSemantic, "Error: The array size must be a natural number at line %d \n", curr_token->lineNumber);
			}
			sizes = parse_EXPR_LIST_TAG();
		}
		else
		{
			insertDimSizeList(sizes, (int)(param->value));
			sizes->head->next = parse_EXPR_LIST_TAG()->head;
		}
		return sizes;
		break;
	}
	case TOKEN_RIGHT_BRACKET:
	{
		fprintf(yyoutSyntactic, "Rule (EXPR_LIST' -> EPSILON)\n");
		curr_token = back_token();
		sizes->head = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return sizes;
}

void parse_CONDITION()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (CONDITION -> EXPR rel_op EXPR)\n");
	currVarIndex = CONDITION;
	parse_EXPR();
	curr_token = next_token();
	if (curr_token->kind == TOKEN_BELOW || curr_token->kind == TOKEN_BELOW_EQUAL || curr_token->kind == TOKEN_EQUAL || curr_token->kind == TOKEN_ABOVE_EQUAL
		|| curr_token->kind == TOKEN_ABOVE || curr_token->kind == TOKEN_NOT_EQUAL)
	{
		parse_EXPR();
	}
	else
	{
		char* actualStr = getKindStr(curr_token->kind);
		char* expectedStr = "TOKEN_BELOW / TOKEN_BELOW_EQUAL / TOKEN_EQUAL / TOKEN_ABOVE_EQUAL / TOKEN_ABOVE / TOKEN_NOT_EQUAL";
		fprintf(yyoutSyntactic, "Expected token of type %s at line: %d, Actual token %s, lexeme: '%s'\n", expectedStr, curr_token->lineNumber, actualStr, curr_token->lexeme);
		//call error recovery
		RecoveryAfterError();
	}
	currVarIndex = prevVarIndex;
}

Param_Node* parse_EXPR()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (EXPR -> TERM EXPR')\n");
	currVarIndex = EXPR;
	Param_Node *param1, * param2;
	param1 = (Param_Node*)(malloc(sizeof(Param_Node)));
	param2 = (Param_Node*)(malloc(sizeof(Param_Node)));
	param1 = parse_TERM();
	param2 = parse_EXPR_TAG();
	if (param1 != NULL)
	{
		if (param2 != NULL)
		{
			if (strcmp(getKindStr(param2->type), getKindStr(TOKEN_FLOAT)) == 0)
			{
				param1->type = TOKEN_FLOAT;
			}
			param1->value += param2->value;
		}
	}
	currVarIndex = prevVarIndex;
	return param1;
}

Param_Node* parse_EXPR_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = EXPR_TAG;
	Param_Node *param1, *param2;
	param1 = (Param_Node*)(malloc(sizeof(Param_Node)));
	param2 = (Param_Node*)(malloc(sizeof(Param_Node)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_PLUS:
	{
		fprintf(yyoutSyntactic, "Rule (EXPR' -> + TERM EXPR')\n");
		param1 = parse_TERM();
		param2 = parse_EXPR_TAG();
		if (param1 != NULL)
		{
			if (param2 != NULL)
			{
				if (strcmp(getKindStr(param2->type), getKindStr(TOKEN_FLOAT)) == 0)
				{
					param1->type = TOKEN_FLOAT;
				}
				param1->value += param2->value;
			}
		}
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	case TOKEN_COMMA:
	case TOKEN_SEMI_COLON:
	case TOKEN_RIGHT_BRACKET:
	case TOKEN_BELOW:
	case TOKEN_BELOW_EQUAL:
	case TOKEN_EQUAL:
	case TOKEN_ABOVE_EQUAL:
	case TOKEN_ABOVE:
	case TOKEN_NOT_EQUAL:
	case TOKEN_RIGHT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (EXPR' -> EPSILON)\n");
		curr_token = back_token();
		param1 = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return param1;
}

Param_Node* parse_TERM()
{
	int prevVarIndex = currVarIndex;
	fprintf(yyoutSyntactic, "Rule (TERM -> FACTOR TERM')\n");
	currVarIndex = TERM;
	Param_Node *param1, *param2;
	param1 = (Param_Node*)(malloc(sizeof(Param_Node)));
	param2 = (Param_Node*)(malloc(sizeof(Param_Node)));
	param1 = parse_FACTOR();
	param2 = parse_TERM_TAG();
	if (param1 != NULL)
	{
		if (param2 != NULL)
		{
			if (strcmp(getKindStr(param2->type), getKindStr(TOKEN_FLOAT)) == 0)
			{
				param1->type = TOKEN_FLOAT;
			}
			param1->value *= param2->value;
		}
	}
	currVarIndex = prevVarIndex;
	return param1;
}

Param_Node* parse_TERM_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = TERM_TAG;
	Param_Node *param1, *param2;
	param1 = (Param_Node*)(malloc(sizeof(Param_Node)));
	param2 = (Param_Node*)(malloc(sizeof(Param_Node)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_MULTIPLACTION:
	{
		fprintf(yyoutSyntactic, "Rule (TERM' -> * FACTOR TERM')\n");
		param1 = parse_FACTOR();
		param2 = parse_TERM_TAG();
		if (param1 != NULL)
		{
			if (param2 != NULL)
			{
				if (strcmp(getKindStr(param2->type), getKindStr(TOKEN_FLOAT)) == 0)
				{
					param1->type = TOKEN_FLOAT;
				}
				param1->value *= param2->value;
			}
		}
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	case TOKEN_PLUS:
	case TOKEN_COMMA:
	case TOKEN_SEMI_COLON:
	case TOKEN_RIGHT_BRACKET:
	case TOKEN_BELOW:
	case TOKEN_BELOW_EQUAL:
	case TOKEN_EQUAL:
	case TOKEN_ABOVE_EQUAL:
	case TOKEN_ABOVE:
	case TOKEN_NOT_EQUAL:
	case TOKEN_RIGHT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (TERM' -> EPSILON)\n");
		curr_token = back_token();
		param1 = NULL;
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return param1;
}

Param_Node* parse_FACTOR()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = FACTOR;
	Param_Node* param = (Param_Node*)(malloc(sizeof(Param_Node)));
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_IDENTIFIER:
	{
		fprintf(yyoutSyntactic, "Rule (FACTOR -> id FACTOR')\n");
		id_entry = find(curr_table, curr_token->lexeme);
		if (id_entry == NULL)
		{
			fprintf(yyoutSemantic, "Error: The identifier %s at line %d has not been declared \n", curr_token->lexeme, curr_token->lineNumber);
			param = NULL;
			parse_FACTOR_TAG();
		}
		else
		{
			setFlagUsed(id_entry);
			param = parse_FACTOR_TAG();

		}
		break;
	}
	case TOKEN_INT_NUM:
	{
		fprintf(yyoutSyntactic, "Rule (FACTOR -> int_num)\n");
		param->type = TOKEN_INT;
		param->value = atof(curr_token->lexeme);
		break;
	}
	case TOKEN_FLOAT_NUM:
	{
		fprintf(yyoutSyntactic, "Rule (FACTOR -> float_num)\n");
		param->type = TOKEN_FLOAT;
		param->value = atof(curr_token->lexeme);
		break;
	}
	case TOKEN_LEFT_PARENTHESES:
	{
		fprintf(yyoutSyntactic, "Rule (FACTOR -> ( EXPR ))\n");
		param = parse_EXPR();
		matchToken(TOKEN_RIGHT_PARENTHESES);
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	if (param != NULL)
		param->id_type = 0; //var not array
	currVarIndex = prevVarIndex;
	return param;
}

Param_Node* parse_FACTOR_TAG()
{
	int prevVarIndex = currVarIndex;
	currVarIndex = FACTOR_TAG;
	Param_Node* param = (Param_Node*)(malloc(sizeof(Param_Node)));
	Table_Node* entry;
	Param_List* paramList;
	Dim_Size_List* size;
	curr_token = next_token();
	switch (curr_token->kind)
	{
	case TOKEN_LEFT_PARENTHESES:
	{
		fprintf(yyoutSyntactic, "Rule (FACTOR' -> ( ARGS ))\n");
		if (id_entry != NULL)
		{
			entry = id_entry;
			if (id_entry->id_type != 2) //function
			{
				fprintf(yyoutSemantic, "Error: illegal assignment into variable or array %s at line %d \n", id_entry->name, curr_token->lineNumber);
				parse_ARGS();
			}
			else
			{
				param->type = id_entry->type;
				paramList = parse_ARGS();
				if (isEqualParamAmount(entry->listParameterTypes, paramList) == 0)
					fprintf(yyoutSemantic, "Error: The call to function %s at line %d is illegal \n", entry->name, curr_token->lineNumber);
			}
		}
		else
		{
			parse_ARGS();
		}
		matchToken(TOKEN_RIGHT_PARENTHESES);
		break;
	}
	case TOKEN_LEFT_BRACKET:
	{
		fprintf(yyoutSyntactic, "Rule (FACTOR' -> [ EXPR_LIST ])\n");
		size = parse_EXPR_LIST();
		if (id_entry != NULL)
		{
			if (id_entry->id_type != 1) //if not array
			{
				fprintf(yyoutSemantic, "Error: illegal assignment into variable or function %s at line %d \n", id_entry->name, curr_token->lineNumber);
				param = NULL;
			}
			else
			{
				if (compareSizes(id_entry->sizes, size) == 0)
				{
					fprintf(yyoutSemantic, "Error: Out of bounds assignment to the array %s at line %d \n", id_entry->name, curr_token->lineNumber);
					param = NULL;
				}
				else if (compareSizes(id_entry->sizes, size) == 2)
				{
					fprintf(yyoutSemantic, "Error: Wrong amount of dimensions of the array %s at line %d \n", id_entry->name, curr_token->lineNumber);
					param = NULL;
				}
				else
				{
					param->type = id_entry->type;
					param->name = id_entry->name;
				}
			}
		}
		else
		{
			param = NULL;
		}
		matchToken(TOKEN_RIGHT_BRACKET);
		break;
	}
	case TOKEN_RIGHT_PARENTHESES:
	case TOKEN_MULTIPLACTION:
	case TOKEN_PLUS:
	case TOKEN_COMMA:
	case TOKEN_SEMI_COLON:
	case TOKEN_RIGHT_BRACKET:
	case TOKEN_BELOW:
	case TOKEN_BELOW_EQUAL:
	case TOKEN_EQUAL:
	case TOKEN_ABOVE_EQUAL:
	case TOKEN_ABOVE:
	case TOKEN_NOT_EQUAL:
	case TOKEN_RIGHT_CURLY_BRACES:
	{
		fprintf(yyoutSyntactic, "Rule (FACTOR' -> EPSILON)\n");
		if (id_entry != NULL)
		{
			if (id_entry->id_type != 0) //if not variable
			{
				fprintf(yyoutSemantic, "Error: illegal assignment into array or function %s at line %d \n", id_entry->name, curr_token->lineNumber);
				param = NULL;
			}
			else
			{
				param->name = (char*)(calloc(strlen(id_entry->name), sizeof(char)));
				param->name = id_entry->name;
				param->type = id_entry->type;
				param->value = id_entry->value;
			}
		}
		else
		{
			param = NULL;
		}
		curr_token = back_token();
		break;
	}
	default:
	{
		printUnexpectedToken();
		RecoveryAfterError();
	}
	}
	currVarIndex = prevVarIndex;
	return param;
}