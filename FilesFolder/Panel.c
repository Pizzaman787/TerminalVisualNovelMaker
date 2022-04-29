// Programmer: ClusterTrace
// Start Date: 12-21-20
// Program: Structure for Panels

#pragma once

#include "Globals.h"
#include "BranchList.c"

// size definition check
/*
#ifndef SIZE
#define SIZE 1024
#endif

#ifndef BIG_SIZE
#define BIG_SIZE 1024*1024
#endif
*/

struct Panel
{
	/*
	char *picture;
	char *name;
	char *words;
	*/
	//char picture[SIZE];
	char picture[BIG_SIZE];
	char name[SIZE];
	char words[SIZE];
	int panelTime;
	struct BranchList* choices; // stores choices for this panel
	struct BranchList* branches; // stores branches for this panel
};