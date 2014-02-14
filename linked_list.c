#include "linked_list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>



struct DLink {
	/* Double Link*/
	TYPE value;
	struct DLink * next;
	struct DLink * prev;
};



struct linkedList{
	/* Double Linked List with Head and Tail Sentinels  */
	int size;
	struct DLink *firstLink;
	struct DLink *lastLink;
};



void _initList (struct linkedList *lst) {
	/*
	  initList
	  param lst the linkedList
	  pre: lst is not null
	  post: lst size is 0
	*/
	lst->firstLink = malloc(sizeof(struct DLink));
	lst->lastLink = malloc(sizeof(struct DLink));
	lst->firstLink->next = lst->lastLink;
	lst->lastLink->prev = lst->firstLink;
	lst->size = 0;
}


struct linkedList *createLinkedList() {
	/*
 createLinkedList
 param: none
 pre: none
 post: firstLink and lastLink reference sentinels
	*/
	struct linkedList *newList = malloc(sizeof(struct linkedList));
	_initList(newList);
	return(newList);
}



void _addLinkBefore(struct linkedList *lst, struct DLink *l, TYPE v) {
	/*
	  param: lst the linkedList
	  param: l the  link to add before
	  param: v the value to add
	  pre: lst is not null
	  pre: l is not null
	  post: lst is not empty
	*/

	/* Adds Before the provided link, l */
	struct DLink *newLink = malloc(sizeof(struct DLink));
	newLink->value = v;
	newLink->next = l;
	newLink->prev = l->prev;
	newLink->prev->next = newLink;
	l->prev = newLink;
	lst->size++;
}


void _removeLink(struct linkedList *lst, struct DLink *l) {
	/*
	  param: lst the linkedList
	  param: l the link to be removed
	  pre: lst is not null
	  pre: l is not null
	  post: lst size is reduced by 1
	*/
	l->prev->next = l->next;
	l->next->prev = l->prev;
	free(l);
	lst->size--;
}


int isEmptyList(struct linkedList *lst) {
	/*
	  isEmptyList
	  param: lst the linkedList
	  pre: lst is not null
	  post: none
	*/
	//printf("%d\n", lst->size);
	if(lst->size == 0);
	return 1;
	return 0;
}


void freeLinkedList(struct linkedList *lst) {
	/* De-allocate all links of the list

	   param: lstpointer to the linked list
	   pre:none
	   post:All links (including the two sentinels) are de-allocated
	*/
	while(!isEmptyList(lst)) {
		/* remove the link right after the first sentinel */
		_removeLink(lst, lst->firstLink->next);
	}
	/* remove the first and last sentinels */
	free(lst->firstLink);
	free(lst->lastLink);
}


void deleteLinkedList(struct linkedList *lst) {
	/* Deallocate all the links and the linked list itself. 

	   param: vpointer to the dynamic array
	   pre:v is not null
	   post:the memory used by v->data is freed
	*/
	assert (lst != 0);
	freeLinkedList(lst);
	free(lst);
}



void _printList(struct linkedList* lst) {
	/*  Function to print list
    Pre: lst is not null
	*/
	struct DLink *current = lst->firstLink->next;
	for(int i = 0; i < lst->size; i++) {
		printf("Link %d Value: %d\n", i, current->value);
		current = current->next;
	}
}

/* ************************************************************************
   Deque Interface Functions
   ************************************************************************ */


void addFrontList(struct linkedList *lst, TYPE e) {
	/*
	  addFrontList
	  param: lst the linkedList
	  param: e the element to be added
	  pre: lst is not null
	  post: lst is not empty, increased size by 1
	*/
	struct DLink *newLink = malloc(sizeof(struct DLink));
	newLink->value = e;
	struct DLink *temp = lst->firstLink->next;
	lst->firstLink->next = newLink;
	temp->prev = newLink;
	newLink->prev = lst->firstLink;
	newLink->next = temp;
	lst->size++;
}


void addBackList(struct linkedList *lst, TYPE e) {
	/*
	  addBackList
	  param: lst the linkedList
	  param: e the element to be added
	  pre: lst is not null
	  post: lst is not empty, increased size by 1
	*/ 
	struct DLink *newLink = malloc(sizeof(struct DLink));
	newLink->value = e;
	struct DLink *temp = lst->lastLink->prev;
	lst->lastLink->prev = newLink;
	temp->next = newLink;
	newLink->prev = temp;
	newLink->next = lst->lastLink;
	lst->size++;
}


TYPE frontList (struct linkedList *lst) {
	/*
	  frontList
	  param: lst the linkedList
	  pre: lst is not null
	  pre: lst is not empty
	  post: none
	*/
	return lst->firstLink->next->value;
}


TYPE backList(struct linkedList *lst) {
	/*
	  backList
	  param: lst the linkedList
	  pre: lst is not null
	  pre: lst is not empty
	  post: lst is not empty
	*/
	return lst->lastLink->prev->value;
}




void removeFrontList(struct linkedList *lst) {
	/*
	  removeFrontList
	  param: lst the linkedList
	  pre:lst is not null
	  pre: lst is not empty
	  post: size is reduced by 1
	*/
	struct DLink *temp = lst->firstLink->next;
	lst->firstLink->next = temp->next;
	temp->next->prev = temp->prev;
	free(temp);
	lst->size--;
}


void removeBackList(struct linkedList *lst) {
	/*
	  removeBackList
	  param: lst the linkedList
	  pre: lst is not null
	  pre:lst is not empty
	  post: size reduced by 1
	*/
	struct DLink *temp = lst->lastLink->prev;
	lst->lastLink->prev = temp->prev;
	temp->prev->next = temp->next;
	free(temp);
	lst->size--;
}


/* ************************************************************************
   Stack Interface Functions
   ************************************************************************ */


void addList(struct linkedList *lst, TYPE v){
	/* 
	   Add an item to the bag
	   param: lstpointer to the bag
	   param: vvalue to be added
	   pre:lst is not null
	   post:a link storing val is added to the bag
	*/
	struct DLink *newLink = malloc(sizeof(struct DLink));
	newLink->value = v;
	if(isEmptyList(lst)) {
		lst->size += 1;
		lst->firstLink->next = newLink;
		lst->lastLink->prev = newLink;
		newLink->next = lst->lastLink;
		newLink->prev = lst->firstLink;
	}
	else {
		lst->size += 1;
		struct DLink *temp = lst->firstLink->next;
		lst->firstLink->next = newLink;
		temp->prev = newLink;
		newLink->prev = lst->firstLink;
		newLink->next = temp;
	}
}


int containsList (struct linkedList *lst, TYPE e) {
	/*Returns boolean (encoded as an int) demonstrating whether or not
	  the specified value is in the collection
	  true = 1
	  false = 0

	  param:lstpointer to the bag
	  param:ethe value to look for in the bag
	  pre:lst is not null
	  pre:lst is not empty
	  post:no changes to the bag
	*/
	struct DLink *current;
	current = lst->firstLink->next;

	while(current != lst->lastLink) {
		if(EQ(current->value, e))
			return 1;
		current = current->next;
	}
	return 0;
}


void removeList (struct linkedList *lst, TYPE e) {
	/*Removes the first occurrence of the specified value from the collection
	  if it occurs

	  param:lstpointer to the bag
	  param:ethe value to be removed from the bag
	  pre:lst is not null
	  pre:lst is not empty
	  post:e has been removed
	  post:size of the bag is reduced by 1
	*/
	struct DLink *current;
	assert(!isEmptyList(lst));
	current = lst->firstLink->next;

	while(current != lst->lastLink) {
		if(EQ(current->value, e)) {
			_removeLink(lst, current);
			lst->size--;
			return;
		}
		current = current->next;

	}
}
