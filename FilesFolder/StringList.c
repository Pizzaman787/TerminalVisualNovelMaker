// ClusterTrace
// 2-17-21
// this is a linked list for strings

#pragma once

#include "Globals.h"

struct StringList
{
	char string[SIZE];
	struct StringList* next;
    struct StringList* head;
};

// takes a pointer to the head node, string for a choice, and a string for the path
void addStringToList(struct StringList** h, char* s)
{
    //make a node
    struct StringList* nod = (struct StringList*)malloc(sizeof(struct StringList));
    strcpy(nod->string, s); // copies string to node
    // if h was null, make this node the head
    if (*h == NULL)
    {
        *h = nod;
        (*h)->head = nod; // it is the head
        nod->next = NULL;
    }
    // if head wasn't null, find the tail
    else
    {
        struct StringList* tail;
        tail = (*h)->head; // starts at the head
        while (tail->next != NULL) // moves down the list until tail becomes the tail
        {
            tail = tail->next;
        }
        tail->next = nod; // add the generated node to the tail, making it the new end
    }
}

// should be given the head of a ChoiceList and it will make it NULL if it isn't already
void emptyStringList(struct StringList** h)
{
    while (*h != NULL)
    {
        struct StringList* temp = *h; // saves where head is
        if ((*h)->next != NULL) // if there is a next one
        {
            *h = (*h)->next; // moves head to next
        }
        free(temp); // free where head was
    }
}