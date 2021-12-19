Laura Thompson  
CS 332, FA 2020

**To Compile:** `gcc -Wall -o sig_handler sig_handler.c`  
**To Run:** `./sig_handler <command> [arguments]`

This program takes a command and its arguments, such as "ls -l" and creates a process to execute the command.
If the command executes without any keyboard signal interruptions, it will proceed as normal and have
the child process perform the given command and then terminate.
However, if the user interrups the process before it can be completed by either terminating it (ctrl-c) or
pausing it (ctrl-z) the child process will either terminate or pause accordingly, but
the parent process will continue to run and wait until it receives a quit signal (ctrl-\ ) from the user.
