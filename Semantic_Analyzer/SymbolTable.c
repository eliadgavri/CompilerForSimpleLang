#include "SymbolTable.h"

void insertDimSizeList(Dim_Size_List* list, int size)
{
	Dim_Size_Node* tmp;

	tmp = (Dim_Size_Node*)malloc(sizeof(Dim_Size_Node));
	tmp->size = size;
	tmp->next = NULL;
	if (list->head == NULL) {
		list->head = tmp;
		return;
	}
	tmp->next = list->head;
	list->head = tmp;
	return;

}
void ctorTableNode(Table_Node* node)
{
	node->next = NULL;
	node->sizes = NULL;
	node->listParameterTypes = NULL;
	node->flag_used = 0;
}

void ctorTable(Table* table)
{
	table->father = (Table*)(malloc(sizeof(Table)));
	table->nodeHead = NULL;
}

void ctorParamNode(Param_Node* node)
{
	node->next = NULL;
	node->prev = NULL;
}

void ctorParamList(Param_List* list)
{
	list->head = NULL;
}

Table* makeTable(Table* current_table)
{
	Table* new_table = (Table*)(malloc(sizeof(Table)));
	new_table->father = current_table;
	new_table->nodeHead = NULL;
	return new_table;
}

Table* popTable(Table* current_table)
{
	return (current_table->father);
}

Table_Node* lookup(Table* current_table, char* id_name)
{
	Table_Node* ptr;
	ptr = current_table->nodeHead;
	while (ptr != NULL)
	{
		if (strcmp(ptr->name, id_name) == 0)
			return ptr;
		else
			ptr = ptr->next;
	}
	return NULL;
}

Table_Node* create_new_entry(Table* current_table)
{
	Table_Node* ptr;
	ptr = current_table->nodeHead;
	Table_Node* temp;
	if (ptr != NULL)
	{
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
	}
	else
	{
		ptr = (Table_Node*)(malloc(sizeof(Table_Node)));
		ptr->next = NULL;
		return ptr;
	}
	temp = ptr;
	ptr = (Table_Node*)(malloc(sizeof(Table_Node)));
	temp->next = ptr;
	ptr->prev = temp;
	ptr->next = NULL;
	return ptr;
}

Table_Node* find(Table* current_table, char* id_name)
{
	Table* table = (Table*)(malloc(sizeof(Table)));
	table = current_table;
	Table_Node* id_entry;
	while (table != NULL)
	{
		id_entry = lookup(table, id_name);
		if (id_entry != NULL)
			return (id_entry);
		else
			table = table->father;
	}
	return NULL;
}

Table_Node* insert(Table* current_table, char* id_name)
{
	Table_Node* entry;
	if (current_table->nodeHead == NULL)
	{
		entry = (create_new_entry(current_table));
		ctorTableNode(entry);
		int len = strlen(id_name);
		entry->name = (char*)(calloc((len), sizeof(char)));
		entry->name = id_name;
		current_table->nodeHead = entry;
		return entry;
	}
	entry = lookup(current_table, id_name);
	if (entry != NULL)
	{
		return NULL;
	}
	else
	{
		entry = (create_new_entry(current_table));
		ctorTableNode(entry);
		int len = strlen(id_name);
		entry->name = (char*)(calloc((len), sizeof(char)));
		entry->name = id_name;
		return entry;
	}
}

void setType(Table_Node* id_entry, eTOKENS id_type)
{
	if (id_entry != NULL)
	{
		id_entry->type = id_type;
	}
}

void setIdRole(Table_Node* id_entry, char* role)
{

	if (id_entry != NULL)
	{
		id_entry->role = role;
	}
}


void setIdListParameterTypes(Table_Node* id_entry, Param_List* listParameterTypes)
{

	if (id_entry != NULL)
	{
		id_entry->listParameterTypes = (Param_List*)(malloc(sizeof(Param_List)));
		id_entry->listParameterTypes = listParameterTypes;
	}

}

void setFlagUsed(Table_Node* id_entry)
{

	if (id_entry != NULL)
	{
		id_entry->flag_used = 1;

	}
}

void setIdType(Table_Node* id_entry, int num)
{

	if (id_entry != NULL)
	{
		id_entry->id_type = num;

	}

}

void setLineDeclareted(Table_Node* id_entry, int line)
{
	if (id_entry != NULL)
	{
		id_entry->lineDeclareted = line;

	}
}

eTOKENS getType(Table_Node* id_entry)
{
	return id_entry->type;
}

char* getIdRole(Table_Node* id_entry)
{
	return id_entry->role;

}

Param_List* getIdListParameterTypes(Table_Node* id_entry)
{
	return id_entry->listParameterTypes;
}

int getFlagUsed(Table_Node* id_entry)
{
	return id_entry->flag_used;
}

int getIdType(Table_Node* id_entry)
{
	return id_entry->id_type;
}

int getLineDeclareted(Table_Node* id_entry)
{
	return id_entry->lineDeclareted;
}

int compareSizes(Dim_Size_List* size, Dim_Size_List* sizeSmall)
{
	int flag = 1;
	if (size == NULL && sizeSmall == NULL) //ok
	{
		return 1;
	}

	if (size == NULL || sizeSmall == NULL) // amount dimensions not match
	{
		return 2;
	}

	Dim_Size_Node* ptr1 = size->head;
	Dim_Size_Node* ptr2 = sizeSmall->head;

	while (ptr1 != NULL && ptr2 != NULL)
	{
		if (ptr1->size <= ptr2->size) //out of bounds
		{
			flag = 0;
		}
		ptr1 = ptr1->next;
		ptr2 = ptr2->next;
	}
	if (ptr1 == NULL && ptr2 == NULL) //ok
	{
		if (flag != 0)
			flag = 1;
	}
	else // amount dimensions not match
	{
		flag = 2;
	}
	return flag;
}
int isEqualSizes(Dim_Size_List* size1, Dim_Size_List* size2)
{
	if (size1 == NULL && size2 == NULL)
	{
		return 1;
	}

	if (size1 == NULL || size2 == NULL)
	{
		return 0;
	}

	Dim_Size_Node* ptr1 = size1->head;
	Dim_Size_Node* ptr2 = size2->head;

	while (ptr1 != NULL && ptr2 != NULL)
	{
		if (ptr1->size != ptr2->size)
		{
			return 0;
		}
		ptr1 = ptr1->next;
		ptr2 = ptr2->next;
	}
	if (ptr1 == NULL && ptr2 == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int compareParamAmount(Param_List* list1, Param_List* list2)
{
	Param_Node* ptr1, * ptr2;
	if (list1 == NULL && list2 == NULL)
		return 1;
	if (list1 == NULL || list2 == NULL)
		return 0;
	ptr1 = list1->head;
	ptr2 = list2->head;

	while (ptr1 != NULL && ptr2 != NULL)
	{
		ptr1 = ptr1->next;
		ptr2 = ptr2->next;
	}
	if (ptr1 == NULL && ptr2 == NULL)
		return 1;
	else
		return 0;
}

int isEqualParamAmount(Param_List* list1, Param_List* list2)
{
	Param_Node* ptr1, * ptr2;
	if (list1 == NULL && list2 == NULL)
		return 1;
	if (list1 == NULL || list2 == NULL)
		return 0;
	ptr1 = list1->head;
	ptr2 = list2->head;

	while (ptr1 != NULL && ptr2 != NULL)
	{
		if (strcmp(getKindStr(ptr1->type), getKindStr(ptr2->type)) == 0)
		{
			if (ptr1->id_type != ptr2->id_type)
				return 0;
			if (ptr1->id_type == 1)
			{
				if (isEqualSizes(ptr1->dimSize, ptr2->dimSize) == 0)
					return 0;

			}
			ptr1 = ptr1->next;
			ptr2 = ptr2->next;
		}
		else
		{
			return 0;
		}
	}
	if (ptr1 == NULL && ptr2 == NULL)
		return 1;
	else
		return 0;
}

int isExistsParam(Param_List* paramList, Param_Node* paramNode)
{
	Param_Node* ptr;
	if (paramNode == NULL || paramList == NULL)
		return 1;
	ptr = paramList->head->next;
	while (ptr != NULL)
	{
		if (strcmp(ptr->name, paramNode->name) == 0)
			return 0; // node exsist in list
		ptr = ptr->next;
	}
	return 1;// node not exsist in list
}