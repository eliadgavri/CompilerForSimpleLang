#pragma once
#include "Token.h"
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

typedef struct Dim_Size_Node {
	int size;
	struct Dim_Size_Node* next;
}Dim_Size_Node;


typedef struct Dim_Size_List {
	struct Dim_Size_Node* head;
}Dim_Size_List;

typedef struct Param_Node
{
	char* name;
	eTOKENS type;
	float value;
	struct Dim_Size_List* dimSize;
	int id_type;
	struct Param_Node* next;
	struct Param_Node* prev;
}Param_Node;


typedef struct Param_List
{
	struct Param_Node* head;
}Param_List;


typedef struct Table_Node
{
	struct Table_Node* next;
	struct Table_Node* prev;
	char* name;
	char* role;
	eTOKENS type;
	Dim_Size_List* sizes;
	struct Param_List* listParameterTypes;
	int flag_used;
	int id_type; // 0=variable, 1=array, 2=function
	int lineDeclareted;
	float value;
}Table_Node;


typedef struct Table
{
	struct Table* father;
	struct Table_Node* nodeHead;
	int id_type; // 0=not function, 2=function
}Table;

Table* makeTable(Table* current_table);
Table* popTable(Table* current_table);
struct Table_Node* lookup(Table* current_table, char* id_name);
struct Table_Node* create_new_entry(Table* current_table);
struct Table_Node* find(Table* current_table, char* id_name);
struct Table_Node* insert(Table* current_table, char* id_name);
void setType(Table_Node* id_entry, eTOKENS id_type);
void setIdRole(Table_Node* id_entry, char* role);
void setIdListParameterTypes(Table_Node* id_entry, Param_List* listParameterTypes);
void setIdType(Table_Node* id_entry, int num);
void setFlagUsed(Table_Node* id_entry);
void setLineDeclareted(Table_Node* id_entry, int line);
eTOKENS getType(Table_Node* id_entry);
char* getIdRole(Table_Node* id_entry);
int getFlagUsed(Table_Node* id_entry);
int getIdType(Table_Node* id_entry);
int getLineDeclareted(Table_Node* id_entry);
Param_List* getIdListParameterTypes(Table_Node* id_entry);
void insertDimSizeList(Dim_Size_List* list, int size);
void ctorTableNode(Table_Node* node);
void ctorTable(Table* table);
void ctorParamNode(Param_Node* node);
void ctorParamList(Param_List* list);
int compareSizes(Dim_Size_List* size, Dim_Size_List* sizeSmall);
int isEqualSizes(Dim_Size_List* size1, Dim_Size_List* size2);
int compareParamAmount(Param_List* list1, Param_List* list2);
int isEqualParamAmount(Param_List* list1, Param_List* list2);
int isExistsParam(Param_List* paramList, Param_Node* paramNode);

#endif