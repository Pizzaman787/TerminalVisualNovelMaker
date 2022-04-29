// ClusterTrace
// 2-14-21
// This is for commands in progress or not working


// https://stackoverflow.com/questions/1516122/how-to-capture-controld-signal
/** Select to check if stdin has pending input */
int pending_input(void) {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}
/** Input terminal mode; save old, setup new */
void setup_terminal(struct termios org_opts) {
  struct termios new_opts;
  tcgetattr(STDIN_FILENO, &org_opts);
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ISIG | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
}
/** Shutdown terminal mode */
void reset_terminal(struct termios org_opts) {
  tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
}
/** Return next input or -1 if none */
int next_input(void) {
  if (!pending_input())
    return -1;
  int rtn = fgetc(stdin);
  printf("Found: %d\n", rtn);
  return(rtn);
}
// Note on above code: It might break terminal displaying after it is used for code running after it

pid_t inputChild(struct termios org_opts)
{
	pid_t pid;
	if ((pid = fork()) == 0) // child runs this
	{
		setup_terminal(org_opts);

		printf("Press Q to quit...\n");
		for (;;) {
			int key = next_input();
			if (key != -1) {
			if ((key == 113) || (key == 81)) {
				printf("\nNormal exit\n");
				break;
			}
			}
		}

		reset_terminal(org_opts);
		exit(0);
	}
	else
	{
		int tempStatus;
		wait(&tempStatus); // has the parent wait
		printf("Child is done.\n");
		exit(0); // ends parent
	}
	return pid; // so the parent can get the pid
}


/* This allows me to alter stdin to instead be a file that it reads, so it no longer takes input from the terminal, but also to return it to terminal input
// messing around with stdin
	int stdinRevert = dup(STDIN_FILENO);
	FILE* stdinTemp;
	stdinTemp = fopen("STDIN_TEMP.txt", "w+"); // creates the file
	fputs("n\n", stdinTemp);
	fclose(stdinTemp); // close as I don't need to access it with this right now, but I might want to later
	// changes stdin to use a file I can read
	freopen("STDIN_TEMP.txt", "r", stdin); // redirects stdin (it now reads from the file STDIN_TEMP.txt)
	int tempInt;
	scanf("%d", &tempInt);
	writeToDebugDocChar((char)tempInt, true);
	dup2(stdinRevert, STDIN_FILENO); // reverts back to the original use of stdin
	//fclose(stdin); // closes stdin, leaving it open
	//int fdTemp = open("STDIN_TEMP.txt", O_RDONLY);
	//dup(fdTemp); // since stdin was closed, fdTemp should become the new stdin when it dupes
	// read(fd, string, string size);
*/

// tries to check for any stdin input by removing the buffer, but it appears to still buffer even when disabled
pid_t inputChild()
{
	pid_t pid;
	if ((pid = fork()) == 0) // child runs this
	{
		// child scans for input
		char buff[2];
		char button;
		//setvbuf(stdin, NULL, _IONBF, NULL); // disables buffering for stdin
		if (!(setvbuf(stdin, buff, _IOLBF, sizeof(char)))) // maxes size to 1 character as to force it to flush with one character inputed
		{
			// waits until something is pressed that isn't a NULL character, so just about anything
			//while (!(strlen(button) > 0))
			{
				//fflush(stdin);
				button = fgetc(stdin); // checks for input with the newly set buffer settings
				writeToDebugDocChar(button, true); // for testing
			}
			setvbuf(stdin, NULL, _IOLBF, SIZE); // enables line buffering
		} 
		else
		{
			printf("Error changing buffer.\n");
		}
		exit(0);
	}
	return pid; // so the parent can get the pid
}

// tries to check for any stdin input by removing the buffer, but it appears to still buffer even when disabled
pid_t inputChildString()
{
	pid_t pid;
	if ((pid = fork()) == 0) // child runs this
	{
		// child scans for input
		char button[SIZE];
		//setvbuf(stdin, NULL, _IONBF, NULL); // disables buffering for stdin
		if (!(setvbuf(stdin, NULL, _IONBF, NULL)))
		{
			// waits until something is pressed that isn't a NULL character, so just about anything
			while (!(strlen(button) > 0))
			{
				//fflush(stdin);
				fgets(button, sizeof(button), stdin); // checks for input with the newly set buffer settings
				writeToDebugDocString(button, true); // for testing
			}
			setvbuf(stdin, NULL, _IOLBF, SIZE); // enables line buffering
		} 
		else
		{
			printf("Error changing buffer.\n");
		}
		exit(0);
	}
	return pid; // so the parent can get the pid
}

// Tries to check if stdin is empty
pid_t inputChild2()
{
	pid_t pid;
	if ((pid = fork()) == 0) // child runs this
	{
		fflush(stdin);
		// readies stuff for selector
		fd_set fdTemp;
		FD_ZERO(&fdTemp);
		FD_SET(STDIN_FILENO, &fdTemp);
		struct timeval timeTemp = {0, 0}; // a timer of zero
		// child scans for input
		//select(FD_SETSIZE, &fds, NULL, NULL, &timeTemp); // checks for input immediately (-1 if nothing to read or 1 if somethin is in stdin)
		while(select(FD_SETSIZE, &fdTemp, NULL, NULL,  &timeTemp) <= 0) // checks if stdin is readable and continues if not
		{
			// do nothing
		}
		exit(0);
	}
	return pid; // so the parent can get the pid
}

// tried using ncurses to check for input, but it would break my displayed text and didn't even return right
pid_t inputChild()
{
	pid_t pid;
	if ((pid = fork()) == 0) // child runs this
	{
		// sets up the curses https://stackoverflow.com/questions/4241366/getch-returns-1
		initscr(); // initializes curses
		//cbreak(); // makes it non-line buffered
		//noecho(); // echo shouldn't be active if cbreak() is enabled
		// call keypad(stdscr, TRUE); if ncurses needs to translate special keys
		// child scans for input
		char button = '\0';
		while (button == '\0')
			button = getch(); 
		writeToDebugDocChar(button, true); // for testing/debugging
		exit(0);
	}
	return pid; // so the parent can get the pid
}