#pragma once
typedef struct Node{
	struct Node * Next;
	struct Node * Last;
	void * Element;
}Node;
typedef struct {
	Node * Top;
	Node * Bottom;
	int Size;
}DList;

DList * newDList();
void freeDList(DList * q);
void * lstGet(DList * lst, int index);
void lstAdd(DList * lst, void * e);
void lstFreeElement(DList * lst);

void qAdd(DList* q, void* e);
void * qPoll(DList* q);

void stkPush(DList * stack, void * e);
void * stkPop(DList* stack);
void * stkPeek(DList * stack);
