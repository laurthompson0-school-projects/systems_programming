/*
Laurence Thompson
CS 332, FA 2020
*/

#include <stdio.h> 
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFSIZE 1000

typedef void Myfunc(const char *pathname, const char *dirname, const struct dirent *dirent, const int iter);

static Myfunc print_Dir;
static void get_Dir(char *dirname, const int iter, Myfunc *func);

//strings to hold arguments given after option flags
char *fval, *tval, *eval, eval2[100], *Eval, Eval2[100], cmdE[100];
int snum, S = 0, s = 0, f = 0, t = 0, e = 0, E = 0, len, E_count = 0;//Option flags
char **e_args; //pointer array for parsed arguments of -e
char E_args[50][100]; //string array for parsed arguments of -E

int main (int argc, char **argv) 
{
    //Read in option flags
    char *sval;
    int n;
    while ((n = getopt(argc, argv, "Ss:f:t:e:E:")) != -1)
    {
	switch (n)
	{
	case 'S':
	    S = 1; //If option flag -S is present, set S to 1 (true)
	    break;
	    
	case 's':
	    s = 1; //If option flag -s is present, set s to 1 (true)
	    sval = optarg; //set sval equal to argument after -s
	    snum = atoi(sval); //convert sval to integer
	    //Check that sval is a positive integer
	    if ((snum <= 0) && (strcmp(sval, "0") != 0))
	    {
		printf("-s accepts positive integer byte sizes.\n");
		exit(-1);
	    }
	    break;
	    
	case 'f':
	    f = 1; //If option flag -f is present, set f to 1 (true)
	    fval = optarg; //set fval equal to the argument after -f
	    break;
	    
	case 't': 
	    t = 1; //If option flag -t is present, set t to 1 (true)
	    tval = optarg; //set tval equal to the argement after -t
	    //Check that tval is either "f" or "d"
	    if ((strcmp(tval, "f") != 0) && (strcmp(tval, "d") != 0))
	    {
		printf("-t requires arguments 'f'(file) or 'd'(directory).\n");
		printf("%s", tval);
		exit(-1);
	    }
	    break;
	    
	case 'e':
	    e = 1; //If option flag -e is present, set e to 1
	    eval = optarg; //set eval equal to the argument after -e
	    
	    //Parse to get number of arguments for len
	    strcpy(eval2, eval);
	    char *delimE = " \t\n\f\r\v"; //delimiters for strtok()
	    char *tokenE; //to get values from first use of strtok()
	    char *token2E; //to get values from second use of strtok()
	    
	    //Tokenize line and increment i each time, excluding first time
	    //Last token will be null so length (i)
	    int j = 1;
	    tokenE = strtok(eval, delimE);
	    while (tokenE != NULL)
	    {
		
		tokenE = strtok(NULL, delimE);
		j++;
	    }
	    len = (const int) j;
	    break;
	    
	case 'E':
	    E = 1; //If option flag -E is present, set E to 1
	    Eval = optarg; //set eval equal to the argument after -E
	    
	    //Parse arguments and set as first 'len' elements of E_args
	    strcpy(Eval2, Eval);
	    char *delim = " \t\n\f\r\v"; //delimiters for strtok()
	    char *token; //to get values from first use of strtok()
	    char *token2; //to get values from second use of strtok()
	    
	    //Tokenize line and increment i each time, excluding first time
	    //Last token will be null so length (i)
	    int i = 0;
	    char Eval2[100];
	    strcpy(Eval2, Eval); //Create string to be intact after strtok
	    token = strtok(Eval, delim);
	    while (token != NULL)
	    {
		
		token = strtok(NULL, delim);
		i++;
	    }
	    len = (const int) i;
	    
	    //Once len is known, parse elements again and add to E_args
	    int k = 0;  
	    token2 = strtok(Eval2, delim); //Tokenize line for a second time
	    
	    strcpy(cmdE, token2);
	    strcpy(E_args[k++], token2);
	    
	    //Fill args with the subsequent words and then an additional null
	    while (token2 != NULL)
	    {
		token2 = strtok(NULL, delim);
		if (token2 != NULL)
		    strcpy(E_args[k++], token2);
	    }
	    break;
	    
	case '?':
	    //Check if an invalid option character/flag was entered
	    if (optopt == 's' || optopt == 'f' || optopt == 't' || optopt == 'e' || optopt == 'E')
	    {
		printf("Option -%c requires an argument.\n", optopt);
		exit(-1);
	    }
	    else
	    {
		printf("Unknown option character -%c. \n", optopt);
	    }
	    break;
	    
	default:
	    //Return an error if not an expected case
	    printf("There was a problem reading input options.\n");
	    exit(-1);
	    break;
	}	
    }
    
    //If no arguments are given, call get_Dir on current directory
    if(optind > (argc - 1))
	get_Dir("./", 0, print_Dir);
    //Otherwise call get_Dir on the path given
    else
	get_Dir(argv[optind], 0, print_Dir);
    
    if (E == 1) //In case of E, execvp only executes once, after rest of program completes
    {
	int total = len + E_count + 1; //total elements in E_args
	int l;

	//print message that -E is executing
	printf("\n Now executing -E '");
	for (l = 0; l < len; l++)
	{
	    if(l = len - 1)
		printf("%s'\n", E_args[l]);
	    else
		printf("%s ", E_args[l]);
	}
	//E_args had a buffer length, copy its elements to an array of exact length
	char *E_args2[total];
	for (l = 0; l < total - 1; l++)
	{
	    E_args2[l] = E_args[l];
	}
	E_args2[total - 1] = NULL;
	
	pid_t pid; //initialize variables for forking process
	int status;
	pid = fork();
	
	//Child process executes commands written from file
	if (pid == 0) 
	{
	    execvp(cmdE, E_args2);
	    printf("If you see this statement then execvp failed ;-(\n");
	    perror("execvp");
	    exit(-1);  
	}
	//Parent process
	else if (pid > 0)
	{
	    wait(&status); //Wait for child process
	    
	    if (!WIFEXITED(status))
	    { //Child process does not exit normally
		printf("Child process did not terminate normally!\n");
	    }    
	}
	else
	{
	    perror("fork");
	    exit(EXIT_FAILURE);
	}
    }
    
    return 0;
}

static void get_Dir(char *dirname, const int iter, Myfunc *func)
{
    //Function that prints information about a file based on specified options
    
    //Initialize path, dirent struct, and DIR variables
    char path[BUFFSIZE];
    struct dirent *dirent; 
    DIR *parentDir;
    int i;
    
    //Open directory specified by path
    parentDir = opendir(dirname); 
    if (parentDir == NULL) 
    { 
	printf ("Error opening directory '%s'\n", dirname); 
	exit (-1);
    }
    
    //Read contents of directory
    while((dirent = readdir(parentDir)) != NULL)
    {
	//Exclude "." and ".." directories which are self-paths/loops
	if (strcmp((*dirent).d_name, ".") != 0 && strcmp((*dirent).d_name, "..") != 0)
	{
	    
	    //Generate complete pathname for a given file
	    strcpy(path, dirname);
	    strcat(path, "/");
	    strcat(path, (*dirent).d_name);
	    
	    //Use pathname and directory to print Directory Hierarchy
	    func(path, dirname, dirent, iter);
	    
	    //If file is a directory, recursively call listDir
	    //Increase iter to get appropriate number of tabs
	    if ((*dirent).d_type == DT_DIR)
	    {
		get_Dir(path, iter + 1, func);
	    }
	}
    } 
    closedir (parentDir);
}

static void print_Dir(const char *pathname, const char *dirname, const struct dirent *dirent, const int iter)
{
    //Print Directory Hierarchy based on option flags
    
    struct stat statbuf;
    int i;
    if ((i = lstat(pathname, &statbuf)) == -1)
    {
	perror("Error creating lstat struct");
	exit(-1);
    }
    
    //Print a file if -s not present or if -s is present and file size greater or equal to snum
    if ((s == 0) || ((s == 1) && snum <= statbuf.st_size))
    {
	//Print a file if -f not present or if -f is present and filename or directory name contains fval
	if((f == 0) || (f == 1 && ((strstr((*dirent).d_name, fval) != NULL) || (strstr(dirname, fval) != NULL) )))
	{
	    //Print a file if -t not present or if -t is present and
	    //either argument f is given and it is a regular file OR
	    //argument d is given and it is a directory
	    if((t == 0) || ((t == 1) && (((strcmp(tval, "f") == 0) && (*dirent).d_type == DT_REG) ||
					 (strcmp(tval, "d") == 0) && (*dirent).d_type == DT_DIR)))
	    {
		//Print as many tabs as the depth of recursion to make hierarchy clear
		for (i = 0; i < iter; i++)
		{
		    printf("\t");
		}
		
		//if -S is present print the file size as well
		if (S == 1)
		{
		    printf("%s (%ld)\n", dirent->d_name, statbuf.st_size);
		}
		//Otherwise just print the file name
		else
		{
		    printf("%s\n", (*dirent).d_name);
		}

		//If -E is present, increase E_count and append path to end of E_args
		if (E == 1)
		{
		    strcpy(E_args[len + E_count++], pathname);
		}
		
		//File that has been filtered for all other flags will now execute -e command
		if ((e == 1) && (*dirent).d_type != DT_DIR)
		{
		    char eval3[100];
		    strcpy(eval3, eval2);
		    
		    char *delim = " \t\n\f\r\v"; //delimiters for strtok()
		    char *token; //to get values from first use of strtok()
		    char *token2; //to get values from second use of strtok()
		    
		    char *args[len+1];
		    int j = 0; 
		    
		    token2 = strtok(eval2, delim); //Tokenize line for a second time
		    
		    args[j++] = token2;
		    char *cmd = token2; //first argument for execvp
		    
					//Fill args with the subsequent words and then an additional null
		    while (token2 != NULL)
		    {
			token2 = strtok(NULL, delim);
			args[j++] = token2;
		    }
		    
		    char temp2[100]; //put pathname in second to last position in array
		    strcpy(temp2, pathname);
		    args[len-1] = temp2;
		    args[len] = NULL;
		    
		    strcpy(eval2, eval3);
		    
		    pid_t pid; //initialize variables for forking process
		    int status;
		    pid = fork();
		    
		    //Child process executes commands written from file
		    if (pid == 0) 
		    {
			execvp(cmd, args);
			printf("If you see this statement then execvp failed ;-(\n");
			perror("execvp");
			exit(-1);  
		    }
		    //Parent process
		    else if (pid > 0)
		    {
			wait(&status); //Wait for child process
			
			if (!WIFEXITED(status))
			{ //Child process does not exit normally
			    printf("Child process did not terminate normally!\n");
			}    
		    }
		    else
		    {
			perror("fork");
			exit(EXIT_FAILURE);
		    }
		}
	    }
	}
    }
}
