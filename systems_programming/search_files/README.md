Laurence Thompson  
CS 332, FA 2020

This program prints the file hierarchy for a
given directory.

To compile:

- Use `make` command with included Makefile or
- `gcc -o file_search file_search.c` on any Linux system that supports the gcc compiler

To run:

This program can be run with or without arguments:

`search <no arguments>` - lists file hierarchy starting with current directory  
`search ../<relative path>` - lists all files in the directory, \<relative path\>, with respect to current directory
search (absolute path) - lists all files in the given directory using absolute path

    Option flags -
    -S: prints files as 'filename (size)'
    -s <size>: prints files of size greater than or equal to <size>
    -f <substring>: prints filenames containing <substring>
    -t <f> or <d>: prints only regular files for 'f' or directories for 'd'
    -e "<unix command w/ args>": runs the specified unix command for each file
    -E "<unix command w/ args>": runs the specified unix command with all files that meet criteria as arguments
