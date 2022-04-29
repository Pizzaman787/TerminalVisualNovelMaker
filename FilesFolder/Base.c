// Programmer: ClusterTrace
// Start Date: 12-14-20
// Program: Base script for game (Has main function)

// includes
#include "Support.c"

// SOME SIGNAL NOTES
// can restore a signal with signal(SIGNAL, SIG_DFL);
// can disable a signal with signal(SIGNAL, SIG_IGN);

// Used these to set up select
// https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html
// https://stackoverflow.com/questions/6681492/using-select-and-fgets-to-access-information-from-a-serial-port

int main(int argc, char* argv[])
{
	// creates save file
	saveFileStartup();
	// sets up a file to store choices
	choicesFileStartup();
	// reads file to get starting panels
	startPanelRead();
	// make a fd_set with stdin as an input
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	int ret; // stores return from select
	struct timeval timeout;
	struct timeval* timeoutPointer = NULL;
	// string for input of fake terminal
	char input[SIZE];

	// display a start screen
	displayer("-Game coded by ClusterTrace-\n-Story may be made by someone else; give all story credit to the author-\n", "Base Commands (commands are case sensitive)\n", "n - goes next\t p - goes previous\t c - redisplays current\n----------------------------------------\nFor more commands type \"commands\"\nRemember to press enter after each command.\n", false);
	while(1)
	{
		printf(">"); // indicator
		fflush(stdout);
		// sets up timer if there is one and is allowed
		if (timer < 0) // if not on a timer
		{
			timeoutPointer = NULL; // disables the timer for select
			//printf("Not on timer.\n"); // for testing
		}
		else // if on a timer (timer >= 0)
		{
			if (timerAllowed) // if allowed to have a timer
			{
				timeout.tv_sec = 0; // no whole seconds given
				timeout.tv_usec = ((long int)timer * 1000); // sets a timer to timer's time
				timeoutPointer = &timeout; // assigns the pointer, readies timer for select
			}
			timer = -1; // disables timer check
			//printf("On timer.\n"); // for testing
		}

		// waits for input or timer
		ret = select(FD_SETSIZE, &fds, NULL, NULL, timeoutPointer);

		if (ret == 1) // ret is 1 if it is ready to read from
		{
			fgets(input, sizeof(input), stdin);
			// evaluate
			evaluate(input);	
			// clear buffer
			fflush(stdin);
		}
		else if (ret == 0) // ready by timer
		{
			// resets fds as fd_sets are altered if readied by timer
			FD_ZERO(&fds);
			FD_SET(STDIN_FILENO, &fds);
			// actis as if the user pressed enter
			evaluate(nextButton);
			strcpy(input, "\0"); // empties input
		}
		else
		{
			printf("Selection error.\n"); // error if it doesn't match my coded returned ifs
		}

		if (feof(stdin)) // if read all of the input of a file or if EOF is active (can be called by pressing Ctrl + D when stdin is empty)
		{
			printf("End of file.\n"); // for testing
			//rewind(stdin); // resets stdin (goes back to the beginning as to prevent the end of the program)
			//fgets(input, sizeof(input), stdin);
			//fflush(stdin);
		}
		//fflush(stdout);
	}
}