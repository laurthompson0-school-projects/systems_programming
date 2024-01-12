/*
Laurence Thompson
CS332, FA 2020

To Compile: gcc -o process_pipe process_pipe.c
To Run: ./process_pipe
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    FILE *fp1;
    char line[BUFSIZ]; //output string
    char inp[BUFSIZ]; //input string

    //Make sure usage is correct
    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Enter command: ");
    fgets(&inp[0], BUFSIZ, stdin); //Get user input in string inp

    //Loop as long as given input is not 'quit'
    while (strcmp(inp, "quit\n") != 0){
	
	/* create a pipe, fork/exec command given by inp, in "read" mode */
	if ((fp1 = popen(inp, "r")) == NULL) {
	    perror("popen");
	    exit(EXIT_FAILURE);
	}
	
	/* read stdout from child process and write to stdout*/
	while (fgets(line, BUFSIZ, fp1) != NULL) {
	    if (fputs(line, stdout) == EOF) {
		printf("Error writing to pipe\n");
		exit(EXIT_FAILURE);
	    }
	}
	
	/* wait for child process to terminate */
	if (pclose(fp1) == -1) {
	    perror("pclose");
	    exit(EXIT_FAILURE);
	}

	printf("Enter command: ");
	fgets(&inp[0], BUFSIZ, stdin); //Get user input in string inp
	
    }
    return 0;
}
