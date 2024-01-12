Laurence Thompson  
CS 332, FA 2020

**To Compile:** `gcc -Wall -o thread_sum thread_sum.c -lpthread`  
**To Run:** `./thread_sum <# of elements> <# of threads>`

This program uses the POSIX threads library. The function takes two arguments. The first is the number of elements to include in an array, [1, 2, ..., N]. The second is the number of threads to create. Each thread created takes arguments in the form of a struct and a function that computes the partial sum of elements in the array. Each thread takes a turn locking the resources necessary to partially compute the sum of elements in the array such that the final output will be the total sum of the array.
