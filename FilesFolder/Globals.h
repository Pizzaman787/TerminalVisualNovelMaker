// Programmer: ClusterTrace
// Start Date: 12-21-20
// Program: Stores the inclusions, definitions, and globals

// operating system definement (only uncomment out one of them, so comment out the other)
// if Linux, uncomment line below
//#define LINUX
// if Windows, uncomment line below
//#define WINDOWS

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h> // used for mkdir
#include <stdlib.h>
#include <string.h>
#include <limits.h>
//#include <termios.h>
//#include <fcntl.h> // allows use of the open() function
//#include <curses.h> // for use of getch()
//#include "DoublyLinkedList.c" // for the panels

// includes the sleep command for windows
/*
#ifdef WINDOWS
#include <windows.h>
#endif
*/

// Macros
#define SIZE 1024 
#define BIG_SIZE 1024*1024 // this is for things like pictures

// Globals
// file paths variables
char startingFilePath[SIZE] = "PanelTexts/Virus/Beginning.txt"; // file read at beginning for panels
char* choicesTempFilePath = "ChoicesTemp.txt"; // stores choices done
char* saveFileFolder = "Saves/"; // should edit this and saveTempFilePath's beginning to be the same
char* saveFileExtension = ".txt"; // should edit this and saveTempFilePath's end to be the same for convention purposes
char* saveTempFilePath = "Saves/SaveTempFile.txt";
// useful storage
char* startScreen = "n - next panel\n"
"p - previous panel\n"
"c - redisplay current panel\n"
"toggle_animated_text - toggles animated text\n"
"pressing enter (during animated text) - skips animation\n"
"pictures - shows pictures (the defualt)\n"
"no_pictures - doesn't show pictures\n"
"delay - to input a new delay for animated text (default 16 milliseconds)\n"
"redos? - prompts for if you want redos (changing decisions)\n"
"toggle_previous - enables or disables the previous command"
"toggle_timers - toggles whether panels can have a timer\n"
"save_progress - saves progress to a file\n"
"load_progress - loads progress from a file\n"
"change_story - change what story is loaded\n"
"save_settings - saves settings to a txt file (Settings.txt)\n"
"load_settings - loads settings from txt file (Settings.txt)\n"
"print_settings - prints current settings\n"
"clear - clears the screen\n"
"commands - shows this command list\n"
"exit - exits the game\n";
struct node* currentNode = NULL;
int timer = -1; // stores the status of the timer (-1 if off, anything at or above 0 is for activating and is the input in milliseconds for the timeGoal) and timer is disabled when next or previous is evaluated
// settings variables
bool animated = false; // the default for whether to animate text or not
bool showPictures = true; // the default for whether to show pictures
unsigned int delay = 16; // the default delay for animated text (in milliseconds or 1/1000s of a second)
bool redos = false; // stores whether the user can redo already made choices (prevents or allows going back to panels with choices)
bool timerAllowed = true;
bool canGoBack = true; // allows use of previous command
char skipButton = '\n';
char* nextButton = "n";
char* previousButton = "p";
char* currentButton = "c";