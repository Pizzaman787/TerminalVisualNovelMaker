linux : Base.c
	gcc -Wall Base.c -o Game 
#gcc -Wall Base.c -o Game -lcurses #Use if curses are needed

windows : Base.c
	gcc -H Base.c -o Game.exe

clean :
	#uses -f so it deletes what it can without stopping for errors in removal
	rm -f Game
	rm -f Game.exe
	rm -f Saves/* 
	# ^ deletes all saves
	rm -f ChoicesTemp.txt
	rm -f Settings.txt
	rm -f DebuggingDoc.txt

#Note: Found a way to check if a file exists from https://stackoverflow.com/questions/5553352/how-do-i-check-if-file-exists-in-makefile-so-i-can-delete-it
#ifneq ("$(wildcard $(PATH_TO_FILE))","")
#   FILE_EXISTS = 1
#else
#   FILE_EXISTS = 0
#endif
