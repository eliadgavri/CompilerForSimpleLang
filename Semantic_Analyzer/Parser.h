#pragma once
#include "SymbolTable.h"
#ifndef PARSERS_H
#define PARSERS_H

#define INITIAL_ARRAY_SIZE 3

//enum of variables of the grammar
typedef enum eVARS
{
	PROG,
	GLOBAL_VARS,
	GLOBAL_VARS_TAG,
	VAR_DEC,
	VAR_DEC_TAG,
	TYPE,
	DIM_SIZES,
	DIM_SIZES_TAG,
	FUNC_PREDEFS,
	FUNC_PREDEFS_TAG,
	FUNC_PROTOTYPE,
	FUNC_FULL_DEFS,
	FUNC_FULL_DEFS_TAG,
	FUNC_WITH_BODY,
	RETURNED_TYPE,
	PARAMS,
	PARAM_LIST,
	PARAM_LIST_TAG,
	PARAM,
	PARAM_TAG,
	COMP_STMT,
	VAR_DEC_LIST,
	STMT_LIST,
	STMT_LIST_TAG,
	STMT,
	STMT_TAG,
	IF_STMT,
	ARGS,
	ARG_LIST,
	ARG_LIST_TAG,
	RETURN_STMT,
	RETURN_STMT_TAG,
	VAR_TAG,
	EXPR_LIST,
	EXPR_LIST_TAG,
	CONDITION,
	EXPR,
	EXPR_TAG,
	TERM,
	TERM_TAG,
	FACTOR,
	FACTOR_TAG,
	LAST_VAR
}eVARS;

//parse functions for each variable
void parse_PROG();
void parse_GLOBAL_VARS();
void parse_GLOBAL_VARS_TAG();
void parse_VAR_DEC();
void parse_VAR_DEC_TAG();
void parse_TYPE();
Dim_Size_List* parse_DIM_SIZES();
Dim_Size_List* parse_DIM_SIZES_TAG();
void parse_FUNC_PREDEFS();
void parse_FUNC_PREDEFS_TAG();
void parse_FUNC_PROTOTYPE();
void parse_FUNC_FULL_DEFS();
void parse_FUNC_FULL_DEFS_TAG();
void parse_FUNC_WITH_BODY();
void parse_RETURNED_TYPE();
Param_List* parse_PARAMS();
Param_List* parse_PARAM_LIST();
Param_List* parse_PARAM_LIST_TAG();
Param_Node* parse_PARAM();
Dim_Size_List* parse_PARAM_TAG();
void parse_COMP_STMT();
void parse_VAR_DEC_LIST();
void parse_STMT_LIST();
void parse_STMT_LIST_TAG();
void parse_STMT();
void parse_STMT_TAG();
void parse_IF_STMT();
Param_List* parse_ARGS();
Param_List* parse_ARG_LIST();
Param_List* parse_ARG_LIST_TAG();
void parse_RETURN_STMT();
Param_Node* parse_RETURN_STMT_TAG();
Param_Node* parse_VAR_TAG();
Dim_Size_List* parse_EXPR_LIST();
Dim_Size_List* parse_EXPR_LIST_TAG();
void parse_CONDITION();
Param_Node* parse_EXPR();
Param_Node* parse_EXPR_TAG();
Param_Node* parse_TERM();
Param_Node* parse_TERM_TAG();
Param_Node* parse_FACTOR();
Param_Node* parse_FACTOR_TAG();

void InitializeParserGrammer();

#define VARS_NUM LAST_VAR

//struct for holding array of follow or first tokens
typedef struct tokenStruct {
	int* tokenArr;
	int realSize; //the real number of tokens in the array
	int arraySize; //current defined size
}tokenStruct;

//struct for each variable, holds its first, follow and nullable attributes
typedef struct varAttributes {
	tokenStruct first;
	tokenStruct follow;
	int nullable;
}varAttributes;

//table of all variables and their attributes
varAttributes varsArray[VARS_NUM];

Token* curr_token;
int currVarIndex;

//match the expected kind with the actual kind
//return true if match, false otherwise
int matchToken(eTOKENS kind);

//functions for handling varsArray
void addFirst(eVARS eVar, eTOKENS eToken);
void addFollow(eVARS eVar, eTOKENS eToken);
void resizeArray(tokenStruct* tokenArray);
void freeArrays();

//functions for handling errors
void printUnexpectedToken();
void RecoveryAfterError();
int isContainCurrentToken();


#endif