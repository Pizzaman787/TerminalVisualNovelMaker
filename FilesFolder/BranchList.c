// Programmer: ClusterTrace
// Start Date: 1-29-2021
// Program: Structure for a branch list

#include "Globals.h" 
#include "StringList.c"
// includes Globals.h as it is the first file in the heirarchy that needs it (all files that need it after will gett it from paassed down inclusions)

// size definition check
/*
#ifndef SIZE
#define SIZE 1024
#endif
*/

struct BranchList
{
    /* CHANGE CHOICES AND BRANCHING TO TAKE MULTIPLE INPUTS FOR THEIR CHECKS
    * This means that multiple choices would output the same thing without needing to list them individually (checks if anything in them is typed)
    * This means that branching can branch but requiring multiple choices to have been made (checks for if all are there)
    */
	struct StringList* choice;
    char path[SIZE];
	struct BranchList* next;
    struct BranchList* head;
};

// takes a pointer to the head node, string for a choice, and a string for the path
void addBranchToList(struct BranchList** h, struct StringList** s, char* p)
{
    //make a node
    struct BranchList* nod = (struct BranchList*)malloc(sizeof(struct BranchList));
    nod->choice = *s; // sets the choices to the list given
    strcpy(nod->path, p); // copies string to node
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
        struct BranchList* tail;
        tail = (*h)->head; // starts at the head
        while (tail->next != NULL) // moves down the list until tail becomes the tail
        {
            tail = tail->next;
        }
        tail->next = nod; // add the generated node to the tail, making it the new end
    }
}

// should be given the head of a ChoiceList and it will make it NULL if it isn't already
void emptyBranchList(struct BranchList** h)
{
    while (*h != NULL)
    {
        struct BranchList* temp = *h; // saves where head is
        if ((*h)->next != NULL) // if there is a next one
        {
            *h = (*h)->next; // moves head to next
        }
        free(temp); // free where head was
    }
}