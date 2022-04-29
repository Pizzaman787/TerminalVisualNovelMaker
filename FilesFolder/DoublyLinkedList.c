// Programmer: ClusterTrace
// Start Date:: 1-1-21
// Purpose: This is a partial implementation of a doubly linked list to be used as the structure for the panels

#pragma once

#include "Panel.c"

struct node
{
    struct Panel* panel;
    struct node* prev;
    struct node* next;
};

// store head
struct node* head = NULL;
// store tail
struct node* tail = NULL;
// store the size
int size = 0;

void addNode(char* p, char* n, char* w, int t, struct BranchList** c, struct BranchList** b)
{
    // make a panel
    struct Panel* temp = (struct Panel*)malloc(sizeof(struct Panel));
    //temp->picture = p;
    strcpy(temp->picture, p);
    //temp->name = n;
    strcpy(temp->name, n);
    //temp->words = w;
    strcpy(temp->words, w);
    // sets the panelTime
    if (t > 0) // only supports positive times
        temp->panelTime = t;
    else
        temp->panelTime = 0;
    // stores choices in node
    temp->choices = *c;
    // stores branches in node
    temp->branches = *b;
    
    //make a node
    struct node* nod = (struct node*)malloc(sizeof(struct node));
    nod->panel = temp;
    // if head was null, make this node the head and previous null
    if (head == NULL)
    {
        head = nod;
        nod->prev = NULL; // not necessary due to defaulting to NULL, but here just in case
    }
    // if head wasn't null, set previous and next
    else
    {
        tail->next = nod;
        nod->prev = tail;
    }
    // make this node the tail
    tail = nod;
    // sets the tail's next to be NULL
    tail->next = NULL;
    // increment size
    size = size + 1;
}

void emptyLinkedList()
{
    while (head != NULL)
    {
        struct node* temp = head; // saves where head is
        if (head->next != NULL) // if there is a next one
        {
            head = head->next; // moves head to next
        }
        else // if there is no next one
        {
            head = NULL;
            tail = NULL;
        }
        // emptyBranchList(&(temp->panel->branches)); // frees the branches first POSSIBLE LEAK! Currently causes double freeing though
        free(temp); // free where head was
        size = size - 1;
    }
}

int getListSize()
{
    return size;
}