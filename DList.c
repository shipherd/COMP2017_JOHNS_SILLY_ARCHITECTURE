#include"DList.h"
#include<stdlib.h>
#include<memory.h>
void * lstGet(DList * lst,int index) {

	Node * tmp = lst->Top->Next;
	for (int k = 0; k < lst->Size; k++) {
		if (index == k)return tmp->Element;
		else tmp = tmp->Next;
	}

	return NULL;
}
void lstAdd(DList * lst, void * e) {
	Node * n = malloc(sizeof(Node));
	n->Element = e;
	Node * bottom = lst->Bottom;
	Node * lbottom = bottom->Last;
	n->Last = lbottom;
	n->Next = bottom;
	lbottom->Next = n;
	bottom->Last = n;
	lst->Size++;
}
Node * newNode(void * e) {
	Node * tmp = malloc(sizeof(Node));
	tmp->Element = e;
	return tmp;
}
DList * newDList() {
	DList * q = (DList*)malloc(sizeof(DList));
	q->Size = 0;
	q->Top = newNode(NULL);
	q->Bottom = newNode(NULL);
	memset(q->Top,0,sizeof(Node));
	memset(q->Bottom, 0, sizeof(Node));
	q->Top->Next = q->Bottom;
	q->Bottom->Last = q->Top;
	
	return q;
}
void freeDList(DList * l) {
	while (l->Size!=0) {
		qPoll(l);
	}
	free(l->Bottom);
	free(l->Top);
	free(l);
}
void qAdd(DList* q,void* e) {
	Node * n = (Node*)malloc(sizeof(Node));
	n->Element = e;
	n->Last = q->Top;
	n->Next = q->Top->Next;
	q->Top->Next->Last = n;
	q->Top->Next = n;
	q->Size++;
}
void* qPoll(DList* q) {
	if (q->Size == 0)return NULL;
	Node* last = q->Bottom->Last;
	last->Last->Next = q->Bottom;
	q->Bottom->Last = last->Last;
	void * ret = last->Element;
	free(last);
	q->Size--;
	return ret;
}
void lstFreeElement(DList * lst) {
	Node * tmp = lst->Top->Next;
	while (tmp->Next!=NULL) {
		free(tmp->Element);
	}
}
void stkPush(DList * stack, void * e) {
	/*Node * head = stack->Top;
	Node * hNext = head->Next;
	Node * newNode = (Node*)malloc(sizeof(Node));
	newNode->Element = e;
	head->Next = newNode;
	newNode->Last = head;
	hNext->Last = newNode;
	newNode->Next = head;
	stack->Size++;*/
	qAdd(stack,e);
}

void * stkPop(DList* stack) {
	Node * head = stack->Top;
	Node * hNext = head->Next;
	Node * hnextNext = hNext->Next;

	void * ret = hNext->Element;
	head->Next = hnextNext;
	hnextNext->Last = head;

	free(hNext);
	stack->Size--;
	return ret;
}
void * stkPeek(DList * stack) {
	return stack->Top->Next->Element;
}
void* lRemove(DList* l) {/*NOT IMPLEMENTED*/
	return NULL;
}