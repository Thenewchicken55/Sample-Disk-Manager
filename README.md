# Sample Disk Manager

This is a C program that runs a simple disk manager on a floppya.img. The program runs one command at a time rather than dynamically prompting the user for a request.

## Useage

The floppya.img is treated like a floppy disk with a lot of simplifications.

To compile, run
> make build

After compiling, various commands can be ran.

## Commands
To delete a file on disk
> ./filesys D *filename*

To list the files on the disk
> ./filesys L

To create a text file
> ./filesys M *filename* 

To print a text file
> ./filesys P *filename*


