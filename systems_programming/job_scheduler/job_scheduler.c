/*
Laura Thompson
CS332, FA 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void itoa(int inp, char out[]) {
    //Helper function to convert integer to string
    //INCLUDED FROM CS330 LAB 3 FA 2020
    int i = -1; int temp = 0;
    int negative = 0;
    //This process outputs the reverse string 1234 -> "4321"
    while(inp != 0)
    {
	i++;
	if (inp < 0)//account for if negative
	{
	    inp = -inp;
	    negative = 1;
	}
	temp = inp%10; //modulo of 10 will be the 1's place
	out[i] = temp + '0';//set next character equal to 1's place value
	inp = (inp-temp)/10;
    }
    if (negative == 1){
	i++;
	out[i] = '-';
    }
	
    //This process reverses the string
    int j = 0;
    int k = i;
    while (j < k)
    {
	//In place reversal from lecture
	*(out + k) =*(out + j) ^ *(out + k);
	*(out + j) = *(out + j) ^ *(out + k);
	*(out + k) =*(out + j) ^ *(out + k);
	j++;
	k--;
    }
    out[i+1] = '\0'; //add null terminator to end
}

void createarray(char *buf, char **array) {
    //Helper function to tokenize a string and add a null character to end
    //Modified from CS 332 Lab 10
    int i, count, len;
    len = strlen(buf);
    buf[len] = '\0'; //terminate string
    //Loop through string and terminate string/add word to array when ' ' encountered
    for (i = 0, array[0] = &buf[0], count = 1; i < len; i++) {
	if (buf[i] == ' ') {
	    buf[i] = '\0';
	    array[count++] = &buf[i+1];
	}
    }
    //Add null character to end of array
    array[count] = (char *)NULL;
}

int tokenize(char inp[], char toks[][BUFSIZ]){
    //Helper function that tokenizes string
    //Does not add null character at end
    //Works by copying rather than pointers, different from 'createarray' function
    char temp[BUFSIZ];
    char *tok;
    int i = 0;
    char *delim = " \t\n\f\r\v";
    strcpy(temp, inp); //creates copy string so original string is not modified
    //Copy each resulting token to array
    tok = strtok(temp, delim);
    while (tok != NULL){
	strcpy(toks[i], tok);
	tok = strtok(NULL, delim);
	i++;
    }

    return i; //returns length of array
}

void detokenize(char str[], char toks[][BUFSIZ], int len){
    //Removes initial command in toks[0]
    //and converts rest of tokens back into a single string
    //Used to get the command string
    int i;
    for (i = 1; i < len; i++){
	if (i > 1)
	    strcat(str, " ");
	strcat(str, toks[i]);
    }
}


int main(int argc, char **argv) {
    
    //Make sure usage is correct
    if (argc != 2) {
        printf("Usage: %s <# of jobs executing>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int numjobs = atoi(argv[1]);
    if (numjobs > 8){
	printf("# of jobs executing cannot exceed number of cores (8)\n");
	exit(EXIT_FAILURE);
    }
    
    char inp[BUFSIZ]; //input string
    char toks[1000][BUFSIZ]; //initialized version of tokenized string
    int i, j, jobcount = 0, numcomplete = 0, numrun = 0, numwait = 0;
    queue *waiting = queue_init(1000); //queue for waiting jobs
    queue *running = queue_init(numjobs); //queue for executing jobs (this is just to keep track of them)
    job* completed = malloc(1000 * sizeof(*completed)); //array of completed jobs
    char str[BUFSIZ] = ""; //string used to make command string
    



    //User instructions
    printf("\nType 'quit' to exit the program, otherwise enter a command\n");
    printf("Commands: submit <program> <arguments>, showjobs, submithistory\n");
    printf("\nEnter command>  ");
    
    fgets(&inp[0], BUFSIZ, stdin); //Get user input in string inp

    //Loop as long as given input is not 'quit'
    while (strcmp(inp, "quit\n") != 0){

	i = tokenize(inp, toks); //break input into tokens to parse

	//Case where command is "showjobs"
	if (strcmp(toks[0], "showjobs") == 0){
	    //Print header then use dispay function on running and waiting jobs
	    printf("jobid   command             status\n");
	    queue_display(running);
	    queue_display(waiting);
	}

	//Case where command is "submithistory"
	else if (strcmp(toks[0], "submithistory") == 0){
	    //Print header then loop and print each value for all completed jobs
	    printf("jobid   command        starttime            endtime                  status\n");
	    for (j = 0; j < numcomplete; j++){
		
		//Remove newline character from time display
	        char temp1[200];
		char temp2[200];
		strcpy(temp1,ctime(&(completed[j].starttime)));
		strcpy(temp2, ctime(&(completed[j].endtime)));
		int n = strlen(temp1);
		temp1[n-1] = '\0';
		n = strlen(temp2);
		temp2[n-1] = '\0';
		
		printf("%d      %s   %s   %s   %s\n", completed[j].jobid, completed[j].command, temp1, temp2,  completed[j].status);
	    }
	}

	//Case where command is "submit"
	else if (strcmp(toks[0], "submit") == 0){

	    //Create and initialize new job
	    job newjob;
	    
	    strcpy(str, "");
	    detokenize(str, toks, i); //get command arguments as a string
	    
	    newjob.jobid = jobcount + 1; //set jobid to one more than previous
	    strcpy(newjob.command, str); //set job commands to arguments
	    strcpy(newjob.status,"waiting"); //set status to waiting
	    
	    //Try inserting new job into waiting queue
	    if ((numwait = queue_insert(waiting, newjob)) != -1){
		jobcount++;
		printf("job %d added to the queue\n", jobcount);
	    }
	    else{
		printf("Queue is full, job %d not added to queue\n", jobcount + 1);
	    }

	    
	}

	//case where command entered is not any of the three specified
	else{
	    printf("%s is not a valid command: submit, showjobs, submithistory\n", toks[0]);
	}


	//Check if something can be added to running queue
	if (numrun < numjobs){
	    job runnext = queue_delete(waiting);
	    //check that there is something waiting
	    if (runnext.jobid > 0){
		numrun++; //increment number of running tasks
		strcpy(runnext.status, "running"); //change status to running
		runnext.starttime = time(NULL); //start timing
		runnext.endtime = 0.0;//If endtime prints as Jan 1, 1970, error
		queue_insert(running, runnext); //add to running queue
	    }
	}

	//If not submitting a new job, take next running job and run it
	if (strcmp(toks[0], "submit") != 0){
	    job runnow = queue_getnext(running);
	    //make sure there is something in running queue
	    if (runnow.jobid > 0){

		int status;
		pid_t pid = fork(); //create new process

		//Child process
		if (pid == 0){

		    //tokenize command 
		    char *args[BUFSIZ]; 
		    strcpy(str, runnow.command);
		    createarray(str, args);
		    
		    int fdout, fderr;//Initialize FD's for output and error redirection
		    char fout[BUFSIZ];//Initialize strings for file names
		    char ferr[BUFSIZ];
		    int jobid = runnow.jobid;//Get pid of child
		    itoa(jobid, fout); //Convert child pid to string
		    itoa(jobid, ferr);
		    strcat(fout, ".out"); //append file extension to file name strings
		    strcat(ferr, ".err");
		    if ((fdout = open(fout, O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1) {
			printf("Error opening file stdin.txt for input\n");
			exit(-1);
		    }
		    //Open/create file to write standard error to
		    if ((fderr = open(ferr, O_CREAT | O_TRUNC | O_WRONLY, 0755)) == -1) {
			printf("Error opening file stdout.txt for output\n");
			exit(-1);
		    }
		    //Redirect stdout and stderr to fdout and fderr respectively
		    dup2(fdout, 1);
		    dup2(fderr, 2);

		    //execute new process
		    execvp(args[0], args);
		    perror("exec");
		    exit(-1);
		    
		}

		//Parent process
		else if (pid > 0) {
		    
		    wait(&status); //wait for the child process to terminate
		    runnow.endtime = time(NULL); //get end time

		    if (WIFEXITED(status)) { //child process exited normally
			strcpy(runnow.status, "Success");
		    } else { //child process did not exit normally
		        strcpy(runnow.status, "Failure");
		    }
		    
		    numrun--; //decrement number running
		    completed[numcomplete] = runnow; //add to completed
		    queue_delete(running); //remove from running queue
		    numcomplete++; //increment number of completed
		    
		    
		}
		//Error with fork
		else { 
		    perror("fork");
		    exit(EXIT_FAILURE);
		}	
	    }
	}

	//Take user input and loop again
	printf("Enter command> ");
	fgets(&inp[0], BUFSIZ, stdin); //Get user input in string inp
	
    }
    
    return 0;
}
