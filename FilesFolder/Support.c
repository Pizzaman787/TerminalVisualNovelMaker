// Programmer: ClusterTrace
// Start Date: 12-21-20
// Program: The supporting file for Base.c, so it has supporting functions

#pragma once

#include "Globals.h"
#include "DoublyLinkedList.c"

// headers for functions
void display(char* picture, char* name, char* words);
void displayer(char* picture, char* name, char* words, bool animate);
void tryNextPanel(struct node** cn, bool saveNextToFile);
void evaluate(char* s);
pid_t buttonChild(char c);
void removeEndings(char* temp);
int captureStatus(pid_t p);
int childDead(pid_t p);
void fakeSleep(unsigned int mseconds);
void saveSettings();
void loadSettings();
void printSettings();
bool readPanelsFromFile(char* path);
void saveFileStartup();
void saveFileUpdate(char* input);
void saveFileCopy(char* name);
void loadCommandsFromFile(char* name);
void reload();
void loadSaveFile(char* name);
bool scanFileForString(char* path, char* string, bool ignoreNextLine);
void choicesFileStartup();
void choicesFileUpdate(char* input);
void fileDeleteLastLine(char* filepath);
void changeStory(char* p);

// functions

// starts the reading at the first file for panels
void startPanelRead()
{
	if (!readPanelsFromFile(startingFilePath)) // if it fails to find the start file
	{
		// code below assisted from: https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
		struct stat st = {0};
		if (stat("PanelTexts", &st) == -1) // if it also cannot find the folder texts are supposed to be in
		{
			mkdir("PanelTexts", 777); // makes the folder/directory
		}
		printf("Created panel directory \"PanelTexts\" for storing texts.\n");
	}
}

// removes the endings /r and /n from a string
void removeEndings(char* temp)
{
	if (strcspn(temp, "\r") < strlen(temp))
		{
			temp[strcspn(temp, "\r")] = '\0'; // removes new line from hitting enter
		}
		if (strcspn(temp, "\n") < strlen(temp))
		{
			temp[strcspn(temp, "\n")] = '\0'; // removes new line from hitting enter
		}
}

// prints the string to file and will print a newline if the boolean is true
void writeToDebugDocString(char* temp, bool newline)
{
	FILE* fil;
	fil = fopen("DebuggingDoc.txt", "a+"); // opens to add
	if (fil == NULL) // if unable to open
	{
		fil = fopen("DebuggingDoc.txt", "w+"); // create to write
	}
	if (fil != NULL) // if was able to open
	{
		fputs(temp, fil); // write string to file
		if (newline)
		{
			fputs("\n", fil);
		}
		fclose(fil);
	}
	else
	{
		printf("Error writing to DebugDoc.txt\n");
	}
}
// version of writeToDebugDoc for a character input
void writeToDebugDocChar(char temp, bool newline)
{
	// creates a temporary string for character
	char* temps = malloc(2 * sizeof(char)); // can store two characters
	temps[0] = temp; // makes the first character the input character
	temps[1] = '\0'; // makes the second/last character the end of the string
	// uses the temp string to write to the DegubbingDoc.txt
	writeToDebugDocString(temps, newline); // calls the string version
	// free the string once it was used
	free(temps);
}

// this function waits for a button to be typed in using a child
// curretly requires enter to be pressed after the button if the button isn't enter
pid_t buttonChild(char c)
{
	pid_t pid;
	if ((pid = fork()) == 0) // child runs this
	{
		// child scans for input, so kill it if the input is no longer needed
			char button;
		while (button != c)
		{
			scanf("%c", &button);
		}
		exit(0);
	}
	return pid; // so the parent can get the pid
}

/*
 * Will return the status of the child if it is finished,
 * or it will return -1 if it ran into an error,
 * or it will return -2 if it is still running.
 * Note: cannot guarentee that the child status isn't -1 or -2 when it finished
 * as that is determined by implementation
 */
int captureStatus(pid_t p)
{
	// found help with stackoverflow.com/questions/4200373/just-check-status-process-in-c
	int status = -1;
	pid_t returned_pid = waitpid(p, &status, WNOHANG);
	if (returned_pid == 0) // the child is still running if returned_pid is 0
	{
		return -2;
	}
	else if (returned_pid == p) // returns the pid of the child if it is finished
	{
		return status; // status will be the finished status of the child
		// Note: Use WEXITSTATUS(status) to get the proper exit number
	}
	else
	{
		return -1; // -1 means it ran into an error
	}
}

// returns 1 if exited, 0 if it hasn't
int childDead(pid_t p)
{
	return WIFEXITED(captureStatus(p)); // if it is terminated
}

// found idea from https://stackoverflow.com/questions/3379139/sleep-function-in-windows-using-c
void fakeSleep(unsigned int mseconds)
{
    clock_t goal = (mseconds * (CLOCKS_PER_SEC/1000)) + clock();
    while (goal > clock())
    {
		//printf("%u : %u\n", goal, clock());
	}
}

void display(char* picture, char* name, char* words)
{
	// clear screen using the terminals clear command
	// system("clear");
	// idea from https://www.geeksforgeeks.org/clear-console-c-language/
	printf("\e[1;1H\e[2J"); // this is some regex that clears the screen like system("clear")
	// show the picture if it is wanted
	if (showPictures)
	{
		printf("%s", picture);
	}
	/* This is the version of name that adds a colon, but I thought that was limiting since the user could easily add that into their panels
	// show name:\n then the words the character says
	if (name[strlen(name) - 1] == '\n') // removes new line at the end if there is one
		name[strlen(name) - 1] = '\0'; // replaces it with a end line or null character
	if (name[strlen(name) - 1] == '\r') // removes cartition return at the end if there is one
		name[strlen(name) - 1] = '\0'; // replaces it with a cartition return or null character
	printf("%s", name);
	if (strlen(name)) // if there is a name, add a colon then a new line
	{
		printf(":\n");
	}
	*/
	// show name
	printf("%s", name);
	// show words
	printf("%s", words);
}

// newer displayer that edits the current line rather than redisplaying (Only works if there is one line, as new lines breaks it)
void displayerForOneLineWithChild(char* picture, char* name, char* words, bool animate)
{
	if (!animate)
	{
		display(picture, name, words);
	}
	else
	{
		pid_t pid; // the process ID of the child
		char current[SIZE]; // stores the current character
		int i = 0; // where in the words we are (0 is first character)

		pid = buttonChild(skipButton);

		// display picture and name with words left empty
		display(picture, name, "");

		while (i < strlen(words))
		{
			// check if the buttonChild is terminated and skip if it is
			if (childDead(pid)) // if it is terminated
			{
				display(picture, name, words);
				break;
			}
			// edit current
			strncpy(current, words, i);
			*(current + i + 1) = '\0'; // add a null character to ensure string ends (prevents unkown characters appearing)
			// update the words display
			printf("%s\r", current); // add a carrige return character to ensure string ends and allow overwrite (note: terminal won't display until \n)
			fflush(stdout); // fixes that the terminal wouldn't display until \n normally
			// add a delay timer to prevent from going too fast
			//fakeSleep(delay);
			// this is a modified fakeSleep that checks the status of skipping during the "sleep", but may be more intensive
			clock_t goal = (delay * (CLOCKS_PER_SEC/1000)) + clock();
			while (goal > clock())
			{
				if (childDead(pid))
				{
					display(picture, name, words);
					goto OutOfWhileLoops; // ensures it exits both while loops, preventing just breaking one
				}
			}
			i = i + 1;
		}
		OutOfWhileLoops: ; // this marks the spot outside of the loops
		printf("\n"); // adds a new line to ensure /r is overwritten
		// if the buttonChild isn't dead by the time the animation has finished, kill it
		if (!childDead(pid))
		{
			kill(pid, SIGTERM);
		}
	}
	//fflush(stdin); // empties stdin in case the user typed while it was animating DOESN'T WORK AS INTENDED, IT DOES NOTHING
}
// displayer that redisplayed every character update
void displayerWithChild(char* picture, char* name, char* words, bool animate)
{
	if (!animate)
	{
		display(picture, name, words);
	}
	else
	{
		pid_t pid; // the process ID of the child
		char current[SIZE]; // stores the current character
		int i = 0; // where in the words we are (0 is first character)

		pid = buttonChild(skipButton);

		// display picture and name with words left empty
		display(picture, name, "");

		while (i < strlen(words))
		{
			// check if the buttonChild is terminated and skip if it is
			if (childDead(pid)) // if it is terminated
			{
				display(picture, name, words);
				break;
			}
			// edit current
			strncpy(current, words, i);
			*(current + i + 1) = '\0'; // add a null character to ensure string ends (prevents unkown characters appearing)
			// update the words display
			display(picture, name, current);
			fflush(stdout); // fixes that the terminal wouldn't display until \n normally
			// add a delay timer to prevent from going too fast
			//fakeSleep(delay);
			// this is a modified fakeSleep that checks the status of skipping during the "sleep", but may be more intensive
			clock_t goal = (delay * (CLOCKS_PER_SEC/1000)) + clock();
			while (goal > clock())
			{
				if (childDead(pid))
				{
					display(picture, name, words);
					goto OutOfWhileLoops2; // ensures it exits both while loops, preventing just breaking one
				}
			}
			i = i + 1;
		}
		OutOfWhileLoops2: ; // this marks the spot outside of the loops
		printf("\n"); // adds a new line to ensure it is printed correctly
		// if the buttonChild isn't dead by the time the animation has finished, kill it
		if (!childDead(pid))
		{
			kill(pid, SIGTERM);
		}
	}
}

void displayer(char* picture, char* name, char* words, bool animate)
{

	if (!animate)
	{
		display(picture, name, words);
	}
	else
	{
		char current[SIZE]; // stores the current character
		int i = 0; // where in the words we are (0 is first character)
		// make a fd_set with stdin as an input
		fd_set fdsTemp;
		FD_ZERO(&fdsTemp);
		FD_SET(STDIN_FILENO, &fdsTemp);
		int retTemp; // stores return from select
		struct timeval timeoutTemp = {0, ((long int)delay * 1000)};

		// display picture and name with words left empty
		display(picture, name, "");

		while (i < strlen(words))
		{
			// edit current
			strncpy(current, words, i);
			*(current + i + 1) = '\0'; // add a null character to ensure string ends (prevents unkown characters appearing)
			retTemp = select(FD_SETSIZE, &fdsTemp, NULL, NULL, &timeoutTemp);

			// update the words display
			if (retTemp == 0) // returns with the timer
			{
				// displays with added character
				display(picture, name, current);
				printf("\n"); // adds a new line to ensure it is printed correctly
				fflush(stdout); // fixes that the terminal wouldn't display until \n normally
				// resets fdsTemp as fd_sets are altered if readied by timer
				FD_ZERO(&fdsTemp);
				FD_SET(STDIN_FILENO, &fdsTemp);
				// resests timer since altered by select
				timeoutTemp.tv_sec = 0; // no whole seconds given
				timeoutTemp.tv_usec = ((long int)delay * 1000); // sets a timer to timer's time
			}
			else // if it returns from pressing enter or error
			{
				display(picture, name, words); // diplay whole thing
				break; // stops loop
			}
			i = i + 1;
		}
	}
}

// evaluates the inputs for the built in commands for this game
void evaluate(char* s)
{
	// set the first space or new line to be null or end of string for reading purposes
	removeEndings(s);
	// commands
	// checks for choices (tries to force player to make a choice)
	if (currentNode != NULL && currentNode->panel->choices != NULL) // if node exists and has choices
	{
		bool chosen = false; // stores if they have made their choice
		// grabs choices
		struct BranchList* tempC = currentNode->panel->choices;
		// loads a runthrough on choices
		while (tempC != NULL && !chosen) // goes until out of choices or they chose one
		{
			struct StringList* tempS = tempC->choice; // gest the StringList in the BranchList
			while (tempS != NULL && !chosen) // runs through the StringList
			{
				if (strcmp(s, tempS->string) == 0) // if input matches an available choice
				{
					chosen = true;
					if (strlen(tempC->path) > 0)
					{
						choicesFileUpdate(tempC->path); // the path stores the unique name of the choice in this case and adds it to a list of choices made
					}
					saveFileUpdate(tempS->string); // saves the choice made to the save file
					// branches if it is supposed to, or if it can't it goes to the next panel if able
					tryNextPanel(&currentNode, false);
				}
				tempS = tempS->next; // sets to the next stored string
			}
			tempC = tempC->next; // sets tempB to the next one (next is NULL if there isn't a next one)
		}
		if (!chosen) // if they didn't choose an option
		{
			// redisplays current panel
			displayer(currentNode->panel->picture, currentNode->panel->name, currentNode->panel->words, animated);
			// notifies that it wasn't a valid choice
			printf("Not a choice.\n");
		}
	}
	// next panel (branches if that is what should be the next panel)
	else if (strcmp(s, nextButton) == 0)
	{
		tryNextPanel(&currentNode, true);
	}
	// previous panel
	else if (strcmp(s, previousButton) == 0)
	{
		if(canGoBack)
		{
			// checks if started yet
			if (currentNode == NULL)
			{
				// display notice
				printf("Nothing to go back to.\n");
			}
			// else sets the current to the previous one and displays if there is one
			else if (currentNode->prev != NULL)
			{
				if (currentNode->prev->panel->choices != NULL && !redos)
				{
					printf("Can't change the past (redos is false).\n");
				}
				else
				{
					currentNode = currentNode->prev;
					displayer(currentNode->panel->picture, currentNode->panel->name, currentNode->panel->words, animated);
					//saveFileDeleteLastLine(); // stores that it went back in save, by deleting the line before it
					fileDeleteLastLine(saveTempFilePath);
					fileDeleteLastLine(choicesTempFilePath);
				}
			}
			else // this means there are no more panels to go back to (at the head panel)
			{
				printf("Nothing to go back to.\n");
			}
		}
		else
			printf("Not allowed to go back.\n");
	}
	else if (strcmp(s, currentButton) == 0)
	{
		// redisplays the current panel
		if (currentNode != NULL)
		{
			displayer(currentNode->panel->picture, currentNode->panel->name, currentNode->panel->words, animated);
		}
		else
		{
			printf("No panel to display.\n");
		}
		
	}
	else if (strcmp(s, "toggle_animated_text") == 0) // swaps animated text
	{
		animated = !animated; // set text to be on or off
		if (animated)
			printf("Animated text!\n");
		else
			printf("Still, unmoving text.\n");
	}
	// add ability to edit the delay for animated text
	else if (strcmp(s, "pictures") == 0)
	{
		showPictures = true;
		printf("Visuals!\n");
	}
	else if (strcmp(s, "no_pictures") == 0)
	{
		showPictures = false;
		printf("No more pictures.\n");
	}
	else if (strcmp(s, "delay") == 0)
	{
		char temp[SIZE]; 
		long tempNum;
		char *endPointer; // just here to fulfill requirement for strtol arguments
		printf("What do you want it to be? (answer in milliseconds)\n");
		//scanf("%u", &temp);
		fgets(temp, SIZE, stdin);
		// valididty check and conversion
		tempNum = strtol(temp, &endPointer, 10); // converts string to long
		if (tempNum < 4294967295) // if within unsigned int limit
			if (tempNum > 0) // and is also positive
				delay = tempNum;
		printf("Delay: %u milliseconds\n", delay);
	}
	else if (strcmp(s, "redos?") == 0)
	{
		char temp[SIZE] = ""; 
		printf("Want to be able to go back to choices using previous?\n \"yes\" or \"no\" ?\n");
		//scanf("%u", &temp);
		fgets(temp, SIZE, stdin);
		// removes endings
		removeEndings(temp);
		// valid answer check
		if (strcmp(temp, "yes") == 0)
		{
			redos = true;
			printf("Can have redos.\n");
		}
		else if (strcmp(temp, "no") == 0)
		{
			redos = false;
			printf("No redos.\n");
		}
		else
		{
			// redos remain unchanged
			printf("Invalid answer. Redos unchanged.\n");
		}
	}
	else if (strcmp(s, "toggle_previous") == 0)
	{
		canGoBack = !canGoBack;
		if (canGoBack)
			printf("Previous command is enabled.\n");
		else
			printf("Previous command is disabled.\n");
	}
	else if (strcmp(s, "toggle_timers") == 0)
	{
		timerAllowed = !timerAllowed;
		if (timerAllowed)
			printf("Timers allowed.\n");
		else
			printf("Timers not allowed.\n");
	}
	else if (strcmp(s, "save_settings") == 0)
	{
		saveSettings();
	}
	else if (strcmp(s, "load_settings") == 0)
	{
		loadSettings();
	}
	else if (strcmp(s, "save_progress") == 0)
	{
		char temp[SIZE];
		printf("Name save file (case is saved):\n");
		fgets(temp, SIZE, stdin);
		temp[strcspn(temp, "\n")] = '\0'; // removes new line from hitting enter
		saveFileCopy(temp);
	}
	else if (strcmp(s, "load_progress") == 0)
	{
		char temp[SIZE];
		printf("Which save file?\n");
		fgets(temp, SIZE, stdin);
		removeEndings(temp); // removes new line from hitting enter
		loadSaveFile(temp);
	}
	else if (strcmp(s, "change_story") == 0)
	{
		char temp[SIZE];
		printf("Which folder is it in (within PanelTexts)?\n");
		fgets(temp, SIZE, stdin);
		removeEndings(temp); // removes new line from hitting enter
		changeStory(temp);
	}
	else if (strcmp(s, "clear") == 0)
	{
		printf("\e[1;1H\e[2J"); // this is some regex that clears the screen like system("clear")
	}
	else if (strcmp(s, "print_settings") == 0)
	{
		printSettings();
	}
	else if (strcmp(s, "commands") == 0)
	{
		displayer("Commands\n", "---------------------------------\n", startScreen, false);
	}
	else if (strcmp(s, "exit") == 0)
	{
		exit(0); // exits the game
	}
	else if(strcmp(s, "Test1") == 0)
	{
		displayer("|\t|\n|\t|\n", "Bob", "Who is there?", true);
	}
	else if(strcmp(s, "Test2") == 0)
	{
		displayer("|\t|\n|\t|\n", "Bob", "Who is there?", false);
	}
	else
	{
		// notifies of invalid command
		printf("Invalid Command.\n");
	}
}

// stores values in settings to a txt file called Settings.txt
void saveSettings() // called by save_settings in-game
{
	FILE *fp;
	fp = fopen("Settings.txt", "w+"); // creates or overwrites a file called Settings.txt
	fprintf(fp, "animated=%d\n", animated);
	fprintf(fp, "showPictures=%d\n", showPictures);
	fprintf(fp, "delay=%u\n", delay);
	fprintf(fp, "redos=%d\n", redos);
	fclose(fp);
}

// loads values from a file called Settings.txt
// assumes file is in the same format as it was saved
void loadSettings() // called by load_settings in-game
{
	FILE *fp;
	char input[SIZE]; // for storing each line 
	char *endPointer; // to fil strtol requirement
	char *pointer;
	fp = fopen("Settings.txt", "r");
	if (fp != NULL)
	{
		// gets animated
		fgets(input, SIZE, fp);
		pointer = strpbrk(input, "1234567890"); // finds where the number starts
		animated = (int) strtol(pointer, &endPointer, 10); // grabs the number in the string
		// gets showPictures
		fgets(input, SIZE, fp);
		pointer = strpbrk(input, "1234567890"); // finds where the number starts
		showPictures = (int) strtol(pointer, &endPointer, 10); // grabs the number in the string
		// gets delay
		fgets(input, SIZE, fp);
		pointer = strpbrk(input, "1234567890"); // finds where the number starts
		delay = (unsigned int) strtol(pointer, &endPointer, 10); // grabs the number in the string
		// closes file
		fclose(fp);
	}
	else
	{
		printf("No settings to load.\n");
	}
}

// shows the variables that store settings
void printSettings() // called by print_settings in-game
{
	printf("*************************************************\n");
	printf("Settings (0 = false and 1 = true):\n");
	printf("--------------------------------------\n");
	printf("animated = %d\n", animated);
	printf("showPictures = %d\n", showPictures);
	printf("delay = %u milliseconds\n", delay);
	printf("redos? = %d\n", redos);
	printf("timers allowed? = %d\n", timerAllowed);
	printf("can go backwards = %d\n", canGoBack);
}

// reads the panels from a file given as an argument and returns true (1) if it was successful and false (0) if it wasn't
bool readPanelsFromFile(char* path)
{
	/* Reference for messing with files from https://www.tutorialspoint.com/cprogramming/c_file_io.htm
		FILE *fp;
		fp = fopen("test.txt", "w+"); // opening a file for writing
		fprintf(fp, "This is testing for fprintf...\n"); // example of how to put text in a file
		fputs("This is testing for fputs...\n", fp); // another example of how to put text in a file
		fclose(fp); // used when done with the file
	*/
	FILE* texts;
	texts = fopen(path, "r"); // open for reading
	if (texts == NULL) // if the file doesn't exist check
	{
		printf("Error opening file.\n");
		return false; // no panels added
	}
	// variables for passing information
	char read[SIZE] = ""; // storing what was read
	char checker[SIZE] = ""; // used for checking what to do
	char pic[BIG_SIZE] = ""; // for pictures
	char nam[SIZE] = ""; // for name
	char wor[SIZE] = ""; // for words
	//char cho[SIZE] = ""; // for choice
	struct StringList* cho; // for choices names
	char cn[SIZE] = ""; // for choice's name
	struct StringList* bch; // for branch choice
	char bp[SIZE] = ""; // for branch path
	struct BranchList* c = NULL; // for branches
	struct BranchList* b = NULL; // for branches
	int t = 0; // for panelTime
	// booleans for what is next
	bool picNext = false;
	bool nameNext = false;
	bool wordsNext = false;
	bool timeNext = false;
	bool choicesNext = false;
	bool choicesNameNext = false;
	bool branchesNext = false;
	bool pathNext = false;
	bool finishPanel = false;
	// reads through the whole file to make the list of panels
	while (fgets(read, SIZE, texts) != NULL) // ensuring it was able to read something (reads one line at a time, until it hits the size cap, or until end of file)
	{
		strcpy(checker, read);
		// idea for line below from https://stackoverflow.com/questions/41572713/string-length-with-fgets-function-in-c
		removeEndings(checker); // removes new line from hitting enter
		// printf("read: %s\n", read); // for testing
		// printf("%d\n", strcmp(read, "*picture*\r")); // for testing that proved that read was getting something called Carriage return or \r after *picture*

		// have it check for a picture
		if (strcmp(checker, "*picture*") == 0) // uses *picture* as the line before picture signal
		{
			picNext = true;
			nameNext = false;
			wordsNext = false;
			timeNext = false;
			choicesNext = false;
			choicesNameNext = false;
			branchesNext = false;
			pathNext = false;
			finishPanel = false;
		}
		// have it check for a name
		else if (strcmp(checker, "*name*") == 0) // uses *name* as the line before name signal
		{
			picNext = false;
			nameNext = true;
			wordsNext = false;
			timeNext = false;
			choicesNext = false;
			choicesNameNext = false;
			branchesNext = false;
			pathNext = false;
			finishPanel = false;
		}
		// have it check for words
		else if (strcmp(checker, "*words*") == 0) // uses *words* as the line before words signal
		{
			picNext = false;
			nameNext = false;
			wordsNext = true;
			timeNext = false;
			choicesNext = false;
			choicesNameNext = false;
			branchesNext = false;
			pathNext = false;
			finishPanel = false;
		}
		// have it check for time
		else if (strcmp(checker, "*timer*") == 0) // uses *words* as the line before words signal
		{
			picNext = false;
			nameNext = false;
			wordsNext = false;
			timeNext = true;
			choicesNext = false;
			choicesNameNext = false;
			branchesNext = false;
			pathNext = false;
			finishPanel = false;
		}
		// have it check for the branches
		else if (strcmp(checker, "*branch*") == 0)
		{
			picNext = false;
			nameNext = false;
			wordsNext = false;
			timeNext = false;
			choicesNext = false;
			choicesNameNext = false;
			branchesNext = true;
			pathNext = false;
			finishPanel = false;
		}
		// have it check for the branches
		else if (strcmp(checker, "*choice*") == 0)
		{
			picNext = false;
			nameNext = false;
			wordsNext = false;
			timeNext = false;
			choicesNext = true;
			choicesNameNext = false;
			branchesNext = false;
			pathNext = false;
			finishPanel = false;
		}
		// have it check for the end of the panel
		else if (strcmp(checker, "*end*") == 0)
		{
			picNext = false;
			nameNext = false;
			wordsNext = false;
			timeNext = false;
			choicesNext = false;
			choicesNameNext = false;
			branchesNext = false;
			pathNext = false;
			finishPanel = true;
		}
		// if picNext is true, add it to pic
		else if (picNext)
		{
			strcat(pic, read);
			//strcpy(pic, read);
		}
		// if nameNext is true, add it to nam
		else if (nameNext)
		{
			strcat(nam, read);
			//strcpy(nam, read);
		}
		// if wordsNext is true, add it to wor
		else if (wordsNext)
		{
			strcat(wor, read);
			//strcpy(wor, read);
		}
		else if (timeNext)
		{
			t = (int)strtol(checker, NULL, 10); // converts string to int
			if (!(t < INT_MAX)) // if within int limit
				t = 0;
			if (t < 0) // and is also positive
				t = 0;
			timeNext = false; // prevents reading more than one line
		}
		else if (choicesNext)
		{
			if (strcmp(checker, "*p*") == 0)
			{
				// marks that the path is next
				choicesNameNext = true;
				choicesNext = false;
			}
			else
			{
				addStringToList(&cho, checker); // uses checker to avoid /r and /n issues
			}
		}
		else if (choicesNameNext)
		{
			strcat(cn, checker); // uses checker to avoid /r and /n issues
			choicesNameNext = false;
			// makes an addition to the branch list
			addBranchToList(&c, &cho, cn);
			// clears bp and bch
			cho = NULL;
			strcpy(cn, "");
		}
		else if (branchesNext)
		{
			if (strcmp(checker, "*p*") == 0)
			{
				// marks that the path is next
				pathNext = true;
				branchesNext = false;
			}
			else
			{
				addStringToList(&bch, checker); // uses checker to avoid /r and /n issues
			}
		}
		else if (pathNext)
		{
			strcat(bp, checker); // uses checker to avoid /r and /n issues
			pathNext = false;
			// makes an addition to the branch list
			addBranchToList(&b, &bch, bp);
			// clears bp and bch
			bch = NULL;
			strcpy(bp, "");
		}
		
		// if it supposed to finish, then make a panel and add it to the list of panels (use addNode function)
		if (finishPanel)
		{
			//wor[strcspn(wor, "\r")] = '\b'; // removes \r to prevent errors when printing THIS BREAKS SOME DISPLAYING
			addNode(pic, nam, wor, t, &c, &b);
			// clear the variables for storage once used to make a panel
			strcpy(pic, "");
			strcpy(nam, "");
			strcpy(wor, "");
			cho = NULL;	// might be unnecessary due to choicesNext check
			strcpy(cn, "");	// might be unnecessary due to choicesNext check
			bch = NULL; // might be unnecessary due to choicesNext check
			strcpy(bp, ""); // might be unnecessary due to pathNext check
			t = 0;
			c = NULL;
			b = NULL;
			// reset finishPanel
			finishPanel = false;
		}
	}
	fclose(texts); // closes the file, as it isn't used after generating panels
	return true;
}

// takes the current node address as input and runs through how it should get to the next panel (branch, go to next, say it can't)
// bool saveNextToFile is for whether it should save a nextButton input to the save file
void tryNextPanel(struct node** cn, bool saveNextToFile)
{
	bool successfulBranchOrStart = false; // for checking if it is a brancher
	// checks if started yet
	if ((*cn) == NULL)
	{
		successfulBranchOrStart = true; // starting
		// checks if head is not null
		if (head != NULL) // if true, goes to first panel and displays
		{
			*cn = head;
			displayer((*cn)->panel->picture, (*cn)->panel->name, (*cn)->panel->words, animated); // displays the first panel
			if (saveNextToFile)
			{
				saveFileUpdate(nextButton); // saves that it went next
			}
		}
		else
		{
			printf("No Panels to display\n");
		}
	}
	// checks if current Node needs to branch
	else if ((*cn)->panel->branches != NULL)
	{
		struct BranchList* tempB = (*cn)->panel->branches; // gets choice list
		char tempP[SIZE] = "";
		bool passedCheck = false; // for it has all required choices
		if (tempB == NULL) // if no branches
		{
			// printf("Failed to branch. Nothing listed to go to.\n"); // for testing
		}
		else
		{
			while (tempB != NULL) // scans through all of the choices
			{
				struct StringList* tempS = tempB->choice;
				bool thisCheck = true;
				while (tempS != NULL) // scans through all of the strings
				{
					if (!scanFileForString(choicesTempFilePath, tempS->string, true))
					{
						thisCheck = false;
					}
					tempS = tempS->next;
				}
				if (thisCheck) // if it passed this check, meaning it had all required strings for this choice
				{
					passedCheck = true;
					break; // stops checking through the choices
				}
				tempB = tempB->next; // sets tempB to the next one
			}
			if (passedCheck)
			{
				strcpy(tempP, tempB->path); // stores path
				// reset current node
				(*cn) = NULL;
				// empties current panels
				emptyLinkedList();
				// add panels from the path
				readPanelsFromFile(tempP);
				// indicates it branched
				successfulBranchOrStart = true;
			}
			if (successfulBranchOrStart)
			{
				// currentNode == NULL, so mimicing evaluate for nextButton if currentNode == NULL
				// checks if head is not null
				if (head != NULL) // if true, goes to first panel and displays
				{
					(*cn) = head;
					displayer((*cn)->panel->picture, (*cn)->panel->name, (*cn)->panel->words, animated); // displays the first panel
					if (saveNextToFile)
					{
						saveFileUpdate(nextButton); // saves that it went next
					}
				}
				else
				{
					printf("No Panels to display\n");
				}
			}
			else // failed to branch
			{
				// printf("Failed to branch. No choices matched.\n"); // for testing
			}
		}
	}
	// sets the current to the next one and displays if there is one if it didn't branch or start
	if (!successfulBranchOrStart)
	{
		if ((*cn)->next != NULL) // is there is something next
		{
			// goes to next panel
			(*cn) = (*cn)->next;
			displayer((*cn)->panel->picture, (*cn)->panel->name, (*cn)->panel->words, animated);
			if (saveNextToFile)
			{
				saveFileUpdate(nextButton); // saves that it went next
			}
		}
		else if ((*cn)->next == NULL) // this means there are no more panels
		{
			printf("End.\n");
		}
	}
	// sets the timer to whatever the panel is supposed to have if the panel exists
	if ((*cn) != NULL)
	{
			if ((*cn)->panel->panelTime > 0)
				timer = (*cn)->panel->panelTime;
	}
}

// sets up the save file
void saveFileStartup()
{
	FILE* fp = fopen(saveTempFilePath, "w+"); // creates the file fresh
	// folder check (could replace with a function to create it)
	if (fp == NULL)
	{
		printf("No directory/folder for saves (names \"Saves\")\nThis directory will be made.\n");
		mkdir("Saves", 777); // folder can read, write, and execute for all user, group, and other
		saveFileStartup(); // calls itself to ensure the file is opened
	}
	fclose(fp);
}

// adds input to the savetempfile (shouldn't include a \n)
void saveFileUpdate(char* input)
{
	FILE* fp = fopen(saveTempFilePath, "a+");
	fprintf(fp, "%s\n", input); // adds input to the SaveTempFile.txt (one per line, thus the \n)
	fclose(fp);
}

// copies the file and names the copy
// will segfault if name is too close to SIZE so it cannot add .txt (needs 4 spaces)
void saveFileCopy(char* name) // called by save_progress in-game
{
	FILE* fp;
	FILE* new;
	char input[SIZE]; // for storing each line 
	// edits name to add .txt
	char newName[SIZE];
	strcpy(newName, saveFileFolder); // ensures it saves to the folder
	strcat(newName, name);
	strcat(newName, saveFileExtension);
	printf("Saved: %s\n", newName); // notifies user
	// open to read temp file
	fp = fopen(saveTempFilePath, "r");
	// creates new file with name given to copy to or overwrites if filename already exists
	new = fopen(newName, "w+");
	// reads file and copies to other file
	while (fgets(input, SIZE, fp) != NULL) // gets line from file unless there isn't one (at end)
	{
		fprintf(new, "%s", input); // writes input to new file
	}
	// close new files
	fclose(fp);
	fclose(new);
}

// loads commands from a file (name requires extension)
void loadCommandsFromFile(char* name)
{
	FILE* fp;
	char input[SIZE]; // for storing each line
	// opens file
	fp = fopen(name, "r");
	// ensures file exists
	if (fp == NULL)
	{
		printf("File doesn't exist.\n");
	}
	else
	{
		// loads saved commands
		while (fgets(input, SIZE, fp) != NULL) // gets line from file unless there isn't one (at end)
		{
			evaluate(input); // evaluates the input from file as if it was input normally as a command
		}
		// closes file
		fclose(fp);
	}
}

// reloads the game
void reload()
{
	// refreshes game to start aka reloads
		// unload all panels
	emptyLinkedList();
		// reset current node
	currentNode = NULL;
		// read beginning panels like main does it
	readPanelsFromFile(startingFilePath);
		// resets the SaveTempFile to ensure it can make progressive saves
	saveFileStartup();
		// resets the choices made, as these will be loaded from the save
	choicesFileStartup();
}

void loadSaveFile(char* name) // called by load_progress in-game
{
	reload();
	// disables animations to ensure loaded commands are processed right
		// saves what animated was to revert it after loading
		bool temp = animated;
		// sets animated to false
		animated = false;
		// saves timerAllowed was to revert it after loading
		bool tempT = timerAllowed;
		// sets timerAllowed to false
		timerAllowed = false;
	// fixes name for the save (convenience for user)
	char newName[SIZE];
	strcpy(newName, saveFileFolder); // ensures it is from the folder
	strcat(newName, name);
	strcat(newName, saveFileExtension); // save extension
	printf("Loaded: %s\n", newName); // for user notification and testing
	// loads commands stored by save_progress command input
	loadCommandsFromFile(newName);
	// reloads variables
	animated = temp;
	timerAllowed = tempT;
}

// scans a file indicated to by the path for the string given in the second argument and returns true if it finds it, false if it doesn't
// it checks line by line, expecting the string to be on its own line
// it checks for the string directly as well, so the string should include \n if ignoreNewLine is false
// ignoreNewLine will ignore the newline character at the end of the lines in the file it checks and string character input
bool scanFileForString(char* path, char* string, bool ignoreNewLine)
{
	FILE* fp;
	char input[SIZE]; // for storing each line
	// opens file
	fp = fopen(path, "r");
	// ensures file exists
	if (fp == NULL)
	{
		printf("File doesn't exist.\n");
		return false;
	}
	else
	{
		if (ignoreNewLine)
		{
			removeEndings(string); // removes new line from hitting enter
		}
		// reads through the file
		while (fgets(input, SIZE, fp) != NULL) // gets line from file unless there isn't one (at end)
		{
			if (ignoreNewLine)
			{
				removeEndings(input); // removes new line from hitting enter
			}
			if(strcmp(input, string) == 0)
			{
				return true;
			}
		}
		// closes file
		fclose(fp);
		return false;
	}
}

// ensures the ChoicesTemp.txt file exists or clears it if it already exists
void choicesFileStartup()
{
	FILE* fp;
	fp  = fopen(choicesTempFilePath, "w+");
	fclose(fp);
}

// adds input to ChoicesTemp.txt
void choicesFileUpdate(char* input)
{
	FILE* fp;
	fp  = fopen(choicesTempFilePath, "a+");
	fprintf(fp, "%s\n", input); // adds input to ChoicesTemp.txt (one per line, thus the \n)
	fclose(fp);
}

// deletes the last line of a file
// uses the name of the file given but adds an extension of "_TEMP" for the secondary file that replaces the original
void fileDeleteLastLine(char* filepath)
{
	FILE* fp = fopen(filepath, "r"); // opens file to read
	// sets up temp file
	char temp[SIZE] = "";
	strcat(temp, filepath);
	if (strcspn(temp, ".") < strlen(temp))
		temp[strcspn(temp, ".")] = '\0'; // stops it at the file extension
	else
	{
		printf("Couldn't read file.\n");
		return;
	}
	strcat(temp, "_TEMP");
	strcat(temp, saveFileExtension);
	FILE* fpTemp = fopen(temp, "w+");
	char input[SIZE]; // for storing each line
	// ensures file exists
	if (fp == NULL)
	{
		printf("File doesn't exist.\n");
	}
	else
	{
		char temp2[SIZE] = "";
		// reads the file one time
		while (fgets(input, SIZE, fp) != NULL) // goes until it reaches the end
		{
			if (strlen(temp2) > 0) // since temp starts as nothing, this delays line addition by 1, meaning it won't add the last line
			{
				fprintf(fpTemp, "%s", temp2); // adds temp2 to the ChoiceTempFile2.txt
			}
			strcpy(temp2, input); // copies input to temp2
		}
		// closes files
		fclose(fp);
		fclose(fpTemp);
		// removes original choicetemp file
		remove(filepath);
		// renames temp file to become new choice file
		rename(temp, filepath);
	}
}

// changes the story loaded by entering the name of the folder in the PanelTexts with story
// It should have a Beginning.txt file
void changeStory(char* p)
{
	char temp[SIZE] = "";
	strcat(temp, "PanelTexts/");
	strcat(temp, p);
	strcat(temp, "/Beginning.txt");
	FILE* fp;
	fp = fopen(temp, "r"); // opens file to read
	// check if file with filepath in temp exists
	if (fp != NULL)
	{
		fclose(fp); // don't need to read it, just needed to test opening it for existence
		strcpy(startingFilePath, temp); // if it does, then change the start path to it and reload
		reload();
	}
	// else print an error and don't change the story
	else
	{
		printf("Error: Couldn't load story, story unchanged. Couldn't find folder or Beginning.txt in folder.\n");
	}
	
}