Laura Thompson  
CS332, FA 2020

**To Compile:** `gcc -o process_pipe process_pipe.c`
**To Run:** `./process_pipe`

This program prompts the user to enter a command such as "ls -l"
then takes the provided string and uses the `popen()` command to create a separate process and pipes the
output of the given command/process to stdout. The program continues to prompt
the user until the user enters "quit".
