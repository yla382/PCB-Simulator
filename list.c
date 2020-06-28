#include <stdio.h>
#include "list.h"


#define MAX_LIST_NUM 10
#define MAX_ITEM_NUM 50

//ITEMS and LISTS
ITEM items[MAX_ITEM_NUM];
LIST lists[MAX_LIST_NUM];

//FREE ITEMS AND FREE LISTS
ITEM *free_items[MAX_ITEM_NUM];
LIST *free_lists[MAX_LIST_NUM];

//Number of availbe items and lists
int free_items_size;
int free_lists_size;



//Initiallize items, lists, free items, free lists
void init() {
	for(int i = 0; i < MAX_LIST_NUM; i++) {
		LIST list = {NULL, NULL, NULL, 0, 0};
		lists[i] = list;
		free_lists[i] = &(lists[i]);
	}

	for(int i = 0; i < MAX_ITEM_NUM; i++) {
		ITEM item = {NULL, NULL, NULL};
		items[i] = item;
		free_items[i] = &(items[i]);
	}

	free_lists_size = MAX_LIST_NUM;
	free_items_size = MAX_ITEM_NUM;
}

LIST *ListCreate() {
	if(free_lists_size == 0) {
		return NULL;
	} else {
		LIST *list = free_lists[free_lists_size - 1];
		free_lists[free_lists_size - 1] = NULL;
		free_lists_size--;
		return list;
	}
}

int ListCount(LIST *list) {
	return list->size;
}

void *ListFirst(LIST *list) {
	if(list->size == 0) {
		return NULL;
	} else {
		list->current_item = list->first_item;
		return list->current_item->data;
	}
}

void *ListLast(LIST *list) {
	if(list->size == 0) {
		return NULL;
	} else {
		list->current_item = list->last_item;
		return list->current_item->data;
	}
}

void *ListNext(LIST *list) {
	if(list->size == 0 || list->current_item == NULL) {
		return NULL;
	} else if(list->current_item == list->last_item) {
		list->current_item = NULL;
		list->beforeAfter = 1;
		return NULL;
	} else {
		list->current_item = list->current_item->next_item;
		return list->current_item->data;
	}
}

void *ListPrev(LIST *list) {
	if(list->size == 0 || list->current_item == NULL) {
		return NULL;
	} else if(list->current_item == list->first_item) {
		list->current_item = NULL;
		list->beforeAfter = -1;
		return NULL;
	} else {
		list->current_item = list->current_item->previous_item;
		return list->current_item->data;
	}
}

void *ListCurr(LIST *list) {
	if(list == NULL || list->size == 0 || list->current_item == NULL) {
		return NULL;
	} else {
		return list->current_item->data;
	}
}

int ListAdd(LIST *list, void *item) {
	if(list == NULL || free_items_size == 0) {
		return -1;
	} else {
		if(list->size == 0) {
			list->first_item = free_items[free_items_size - 1];
			list->last_item = free_items[free_items_size - 1];
			list->current_item = free_items[free_items_size - 1];
		} else {
			if(list->current_item == NULL) {
				if(list->beforeAfter == -1) {
					list->current_item = free_items[free_items_size - 1];
					list->current_item->next_item = list->first_item;
					list->first_item->previous_item = list->current_item;
					list->first_item = list->current_item;
				} else {
					if(list->beforeAfter == 1) {
						list->current_item = free_items[free_items_size - 1];
						list->current_item->previous_item = list->last_item;
						list->last_item->next_item = list->current_item;
						list->last_item = list->current_item;
					}
				}
			} else {
				if(list->current_item == list->last_item) {
					list->current_item = free_items[free_items_size - 1];
					list->current_item->previous_item = list->last_item;
					list->last_item->next_item = list->current_item;
					list->last_item = list->current_item;
				} else {
					free_items[free_items_size - 1]->previous_item = list->current_item;
					free_items[free_items_size - 1]->next_item = list->current_item->next_item;
					list->current_item->next_item->previous_item = free_items[free_items_size - 1];
					list->current_item->next_item = free_items[free_items_size - 1];
					list->current_item = list->current_item->next_item;
				}
			}
		}
		(list->size)++;
		free_items[free_items_size - 1]->data = item;
		free_items[free_items_size - 1] = NULL;
		free_items_size--;
		return 0;
	}
}

int ListInsert(LIST *list, void *item) {
	if(list == NULL || free_items_size == 0) {
		return -1;
	} else {
		if(list->size == 0) {
			list->first_item = free_items[free_items_size - 1];
			list->last_item = free_items[free_items_size - 1];
			list->current_item = free_items[free_items_size - 1];
		} else {
			if(list->current_item == NULL) {
				if(list->beforeAfter == -1) {
					list->current_item = free_items[free_items_size - 1];
					list->current_item->next_item = list->first_item;
					list->first_item->previous_item = list->current_item;
					list->first_item = list->current_item;
				} else {
					if(list->beforeAfter == 1) {
						list->current_item = free_items[free_items_size - 1];
						list->current_item->previous_item = list->last_item;
						list->last_item->next_item = list->current_item;
						list->last_item = list->current_item;
					}
				}
			} else {
				if(list->current_item == list->first_item) {
					list->current_item = free_items[free_items_size - 1];
					list->current_item->next_item = list->first_item;
					list->first_item->previous_item = list->current_item;
					list->first_item = list->current_item;
				} else {
					free_items[free_items_size - 1]->next_item = list->current_item;
					free_items[free_items_size - 1]->previous_item = list->current_item->previous_item;
					list->current_item->previous_item->next_item = free_items[free_items_size - 1];
					list->current_item->previous_item = free_items[free_items_size - 1];
					list->current_item = list->current_item->previous_item;
				}
			}
		}
		(list->size)++;
		free_items[free_items_size - 1]->data = item;
		free_items[free_items_size - 1] = NULL;
		free_items_size--;
		return 0;
	}
}

int ListAppend(LIST *list, void *item) {
	if(list == NULL || free_items_size == 0) {
		return -1;
	} else {
		if(list->size == 0) {
			list->first_item = free_items[free_items_size - 1];
			list->last_item = free_items[free_items_size - 1];
			list->current_item = free_items[free_items_size - 1];
		} else {
			list->current_item = free_items[free_items_size - 1];
			list->current_item->previous_item = list->last_item;
			list->last_item->next_item = list->current_item;
			list->last_item = list->current_item;
		}
		(list->size)++;
		free_items[free_items_size - 1]->data = item;
		free_items[free_items_size - 1] = NULL;
		free_items_size--;
		return 0;
	}
}

int ListPrepend(LIST *list, void *item) {
	if(list == NULL || free_items_size == 0) {
		return -1;
	} else {
		if(list->size == 0) {
			list->first_item = free_items[free_items_size - 1];
			list->last_item = free_items[free_items_size - 1];
			list->current_item = free_items[free_items_size - 1];
		} else {
			list->current_item = free_items[free_items_size - 1];
			list->current_item->next_item = list->first_item;
			list->first_item->previous_item = list->current_item;
			list->first_item = list->current_item;
		}
		(list->size)++;
		free_items[free_items_size - 1]->data = item;
		free_items[free_items_size - 1] = NULL;
		free_items_size--;
		return 0;
	}
}

void *ListRemove(LIST *list) {
	if(list == NULL || list->size == 0 || list->current_item == NULL) {
		return NULL;
	} else {
		void *item = list->current_item->data;
		if(list->size == 1) { //case one: list only has one item
			free_items[free_items_size] = list->current_item;
			free_items[free_items_size]->data = NULL;
			free_items[free_items_size]->next_item = NULL;
			free_items[free_items_size]->previous_item = NULL;
			list->current_item = NULL;
			list->first_item = NULL;
			list->last_item = NULL;
		} else {
			if(list->current_item == list->last_item) { //case two: current item == last_item
				list->last_item = list->current_item->previous_item;
				list->last_item->next_item = NULL;
				free_items[free_items_size] = list->current_item;
				free_items[free_items_size]->data = NULL;
				free_items[free_items_size]->next_item = NULL;
				free_items[free_items_size]->previous_item = NULL;
				list->current_item = NULL;
				list->beforeAfter = 1;	
			} else { //case three: current item is in the middle of the list
				list->current_item->previous_item->next_item = list->current_item->next_item;
				list->current_item->next_item->previous_item = list->current_item->previous_item;
				free_items[free_items_size] = list->current_item;
				list->current_item = list->current_item->next_item;
				free_items[free_items_size]->data = NULL;
				free_items[free_items_size]->next_item = NULL;
				free_items[free_items_size]->previous_item = NULL;
			}
		}
		(list->size)--;
		free_items_size++;
		return item;
	}
}

void ListConcat(LIST *list1, LIST **list2) {
	if(list1 == NULL || (*list2) == NULL) {
		return;
	} else {
		if(list1->size == 0 && (*list2)->size == 0) { //checked
			//just free the list2
			free_lists[free_lists_size] = *list2;
			free_lists[free_lists_size]->beforeAfter = 0;
		} else if(list1->size != 0 && (*list2)->size == 0) { //checked
			//just free the list2
			free_lists[free_lists_size] = *list2;
			free_lists[free_lists_size]->beforeAfter = 0;
		} else if(list1->size == 0 && (*list2)->size != 0) {  //checked
			list1->first_item = (*list2)->first_item;
			list1->last_item = (*list2)->last_item;
			list1->size = (*list2)->size;
			//free list2
			free_lists[free_lists_size] = *list2;
			free_lists[free_lists_size]->current_item = NULL;
			free_lists[free_lists_size]->first_item = NULL;
			free_lists[free_lists_size]->last_item = NULL;
			free_lists[free_lists_size]->size = 0;
			free_lists[free_lists_size]->beforeAfter = 0;
		} else {
			list1->last_item->next_item = (*list2)->first_item;
			(*list2)->first_item->previous_item = list1->last_item->next_item;
			list1->last_item = (*list2)->last_item;
			list1->size = list1->size + (*list2)->size;
			//free list 2
			free_lists[free_lists_size] = *list2;
			free_lists[free_lists_size]->current_item = NULL;
			free_lists[free_lists_size]->first_item = NULL;
			free_lists[free_lists_size]->last_item = NULL;
			free_lists[free_lists_size]->size = 0;
			free_lists[free_lists_size]->beforeAfter = 0;
		}
		free_lists_size++;
		*list2 = NULL;
	}
}

// void itemFree(void *item) {
// 	printf("ITEM RELEASED TO HELL\n");
// }

void ListFree(LIST *list, void(*itemFree)(void*)) {  // :)
	if(list == NULL || list->size == 0) {
		return;
	} else {
		if(list->size == 0) {
			free_lists[free_lists_size] = list;
			free_lists[free_lists_size]->current_item = NULL;
			free_lists[free_lists_size]->first_item = NULL;
			free_lists[free_lists_size]->last_item = NULL;
			free_lists[free_lists_size]->size = 0;
			free_lists[free_lists_size]->beforeAfter = 0;
			free_lists_size++;
		} else {
			ITEM *item = list->first_item;
			for(int i = 0; i < list->size; i++) {
				void *data = item->data;
				free_items[free_items_size] = item;
				item = item->next_item;
				free_items[free_items_size]->data = NULL;
				free_items[free_items_size]->next_item = NULL;
				free_items[free_items_size ]->previous_item = NULL;
				(*itemFree)(data);
				free_items_size++;
			}
			free_lists[free_lists_size] = list;
			free_lists[free_lists_size]->current_item = NULL;
			free_lists[free_lists_size]->first_item = NULL;
			free_lists[free_lists_size]->last_item = NULL;
			free_lists[free_lists_size]->size = 0;
			free_lists[free_lists_size]->beforeAfter = 0;
			free_lists_size++;
		}
	}
}

void *ListTrim(LIST *list) {
	if(list == NULL || list->size == 0) {
		return NULL;
	} else {
		void *item = list->last_item->data;
		//printf("%d\n", *(int *)item->data);
		ListLast(list);
		ListRemove(list);
		ListLast(list);
		list->current_item = list->last_item;
		printf("%d\n", *(int *)list->last_item->data);
		return item;
	}
}

void *ListSearch(LIST *list, int (*comparator)(void*, void*), void *comparisonArg) {
	if(list == NULL || list->size == 0 || list->current_item == NULL) {
		return NULL;
	} else {
		ITEM *item = list->current_item;
		while(item != NULL) {
			int test = comparator(item->data, comparisonArg);
			if(test == 1) {
				list->current_item = item;
				return list->current_item->data;
			} else {
				item = item->next_item;
			}
		}
		list->current_item = NULL;
		list->beforeAfter = 1;
		return NULL;
	}
}


void display(LIST *list) {
	if(list == NULL || list->size == 0) {
		printf("LIST null or empty\n");
	} else {
		ITEM *item = list->first_item;
		for(int i = 0; i < list->size; i++) {
			printf("%d ", *(int *)item->data);
			item = item->next_item;
		}
		printf("\n");
	}
}
