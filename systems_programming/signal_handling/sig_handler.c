/*
Laura Thompson
CS 332, FA 2020
Lab 09, 10/28/2020

To Compile: gcc -Wall thompson_lab09.c
To Run: ./a.out <command> [args]
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

static void sig_parent(int signo)
{
    switch(signo){
    case SIGINT: //catch C-c signal and prevent default behavior
	printf(" Parent handled SIGINT signal %d\n", signo);
	break;
    case SIGTSTP: //catch C-z signal and prevent default behavior
	printf(" Parent handled SIGSTP signal %d\n", signo);
	printf("Child process is stopped\n");
	printf("Use C-\\ to terminate parent process with SIGQUIT\n");
	break;
    }
}

static void sig_child(int signo)
{
    //catch C-z signal and prevent default behavior
    printf("Child suspended by SIGSTP signal %d\n", signo);
    printf("Use C-\\ to terminate parent process with SIGQUIT\n");
    raise(SIGSTOP);
}

int main(int argc, char **argv) {
    pid_t pid;
    int status;

    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        exit(-1);
    }

    pid = fork(); //create child process
    
    if (pid == 0) { /* this is child process */
        execvp(argv[1], &argv[1]); //Execute command line arguments
	if (signal(SIGTSTP, sig_child) == SIG_ERR){
	    printf("Can't catch SIGINT\n");
	}
        printf("If you see this statement then execl failed ;-(\n");
	perror("execvp");
	exit(-1);
	
    } else if (pid > 0) { /* this is the parent process */
	
        printf("Wait for the child process to terminate\n");

	//Catch C-c and C-z so parent process is not terminated
	//or suspended when child process is
	if (signal(SIGINT, sig_parent) == SIG_ERR){
	    printf("Can't catch SIGINT\n");
	}
	if (signal(SIGTSTP, sig_parent) == SIG_ERR){
	    printf("Can't catch SIGSTP\n");
	}

	//Program will get stuck at this point if child process
	//is suspended with C-z
        wait(&status); /* wait for the child process to terminate */
	
        if (WIFEXITED(status)) { /* child process terminated normally */
	    //This will print if child is allowed to execute to completion
            printf("Child process exited with status = %d\n", WEXITSTATUS(status));
        } else { /* child process did not terminate normally */
	    //This will print if C-c signal is used
            printf("Child process did not terminate normally!\n");
	    printf("Use C-\\ to terminate parent process with SIGQUIT\n");
	
	    for( ; ; )
		pause();
        }

	//Tell user that the parent process will continue to execute
	//unless C-\\ is pressed because this signal is not handled
	
    } else { /* we have an error */
        perror("fork"); /* use perror to print the system error message */
        exit(EXIT_FAILURE);
    }
    
    printf("[%ld]: Exiting program .....\n", (long)getpid());

    return 0;
}
