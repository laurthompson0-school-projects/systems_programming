Laura Thompson  
CS332, FA 2020

**To Compile:** `make`  
**To Run:** `./job_scheduler <#of jobs executed at once>`

This program is a simple job scheduler that will take one of three commands:

`submit <command> <args>` - Creates a new job and sends it to a waiting queue.  
`showjobs` - Shows jobs that are either running or
waiting to be run.  
`submithistory` - Shows jobs that have been completed.

Once in the waiting queue, the program checks to see how many jobs are running
if it is less than the number of specified as an input argument, the program
will start to run the job. Once completed, the struct representing that job
will be added to an array of completed jobs and displayed in submithistory.
