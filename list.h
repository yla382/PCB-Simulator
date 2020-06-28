#ifndef LIST_H
#define LIST_H

typedef struct ITEM {
	void *data;
	struct ITEM *next_item;
	struct ITEM *previous_item;
} ITEM;

typedef struct LIST {
	struct ITEM *current_item;
	struct ITEM *first_item;
	struct ITEM *last_item;
	int size;
	int beforeAfter;
} LIST;


void init();
LIST *ListCreate();
int ListCount(LIST *list);
void *ListFirst(LIST *list);
void *ListLast(LIST *list);
void *ListNext(LIST *list);
void *ListPrev(LIST *list);
void *ListCurr(LIST *list);
int ListAdd(LIST *list, void *item);
int ListInsert(LIST *list, void *item);
int ListAppend(LIST *list, void *item);
int ListPrepend(LIST *list, void *item);
void *ListRemove(LIST *list);
void ListConcat(LIST *list1, LIST **list2);
void ListFree(LIST *list, void(*itemFree)(void*));
void *ListTrim(LIST *list);
void *ListSearch(LIST *list, int (*comparator)(void*, void*), void *comparisonArg);
void display(LIST *list);

#endif
