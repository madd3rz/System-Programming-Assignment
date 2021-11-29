#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>

int block = 0;
int position = 0;
char *fromPath = NULL;
char *toPath = NULL;
int isBlockGiven = 0;
int isPositionGiven = 0;
int isPrinting = 0;
int help = 0;

int getToFromFilePath( char *argv[], int *optind) {
    // This function get the source file path and destination file path and store in fromPath and toPath pointers.
    int idx = *optind;
    fromPath = argv[idx];
    toPath = argv[idx + 1];

    if(access(fromPath,F_OK) == 0 && access(toPath,F_OK) == 0){ // check if the file exists in the directory specified
        fprintf(stdout, "Origin File Path:%s\n", fromPath);
        if(toPath){ fprintf(stdout, "Destination Filepath:%s\n", toPath); }
    }
}

int getFileLength(int fd){
    // This function returns the length of the file in bytes
    int fileLength = lseek( fd, 0, SEEK_END );  // get file length using lseek()
    if ( fileLength < 0 ){
        exit(1);
    }
    lseek(fd, 0, SEEK_SET); // return to beginning position of file after seek to EOF above

    return fileLength;
}

int openFromFile(char *path){
    // This function returns the file descriptor of the source file
    int fd = open(path,O_RDONLY); // open the source file with O_RDONLY permission 
    if(fd < 0){
		perror("Error open file!");
		exit(1); // exit program if errpr opening source file 
	}
    return fd;
}

int openToFile(char *path){
    // This function returns the file descriptor of the destination file
    // open the destination file with read write permission
    // append new data to EOF, and create a new file if not exists
    // Read and write permission bit for file owner, 
    // and set read permission for group and other users.

    int fd = open(path, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fd < 0){
		perror("Error!");
		exit(1); //exit program if error opening file
	}
    return fd;
}

void printFile(){
    // This function is for printing content of source file to the screen
    if(fromPath == NULL){
        fprintf(stderr,"Error: No file specified.\n");
        exit(1); //exit program if not found source file
    }
    
    int fd = openFromFile(fromPath);
    int fileLength = getFileLength(fd);
    char *buf[fileLength]; // buffer to hold the content of source file

    fprintf(stdout, "\nContent of source file: \n");
    read(fd, buf, fileLength);  // read content from source file fd
    write(1, buf, fileLength);  // write to stdout
    close(fd);                  // close fd
    fprintf(stdout, "--- EOF ---\n");
}

void copyBlock(){
    // This function copy the content from source to destination file with user specified block size
    if(fromPath == NULL || toPath == NULL){
        fprintf(stderr,"Error: Please specify source and destination file path.\n");
        exit(1); //exit program if not found file
    }

    int fd1 = openFromFile(fromPath);
    int fd2 = openToFile(toPath);
    int fileLength = getFileLength(fd1);
    char buf[block]; // buffer to hold the content of teh source file

    // Assign the file length as the block size if user input block size exceeded the file size
    if(block > fileLength){
        fprintf(stdout, "Block size entered: %d exceeds source file length: %d\n", block, fileLength);
        fprintf(stdout, "Using the file size: %d as the block size\n", fileLength);
        block = fileLength;
    }
    else{
        fprintf(stdout,"Block size entered: %d\n", block);
    }

    read(fd1, buf, block);  // read content from source file fd1
    buf[fileLength] = '\0'; // NULL terminator in read buffer
    write(fd2, buf, block); // write content to destination file fd2
    close(fd1);             // close fd1
    close(fd2);             // close fd2
}

void copyPositional(){
    // This function copy the content from source file to destination file with user indicating where to copy from the source file
    if(fromPath == NULL || toPath == NULL){
        fprintf(stderr,"Error: Please specify source and destination file path.\n");
        exit(1); //exit program if not found file
    }

    int fd1 = openFromFile(fromPath);
    int fd2 = openToFile(toPath);
    int fileLength = getFileLength(fd1);

    // lseek to the position user input in command
    lseek(fd1, position, SEEK_SET);
    fprintf(stdout,"Position entered: %d\n", position);

    char *buf[fileLength];                 // buffer to hold the content of source file
    read(fd1, buf, fileLength-position);   // read the content of the source file into buffer
    write(fd2, buf, fileLength-position);  // write the buffer to the destination file
    close(fd1);                            // close fd1
    close(fd2);                            // close fd2
}

void copyBlockandPosition(){
    // This function copy the content from source file to destination file with user indicating both block size and position
    if(fromPath == NULL || toPath == NULL){
        fprintf(stderr,"Error: Please specify source and destination file path.\n");
        exit(1); //exit program if not found file
    }

    int fd1 = openFromFile(fromPath);
    int fd2 = openToFile(toPath);
    int fileLength = getFileLength(fd1);
    char buf[block];        // buffer to hold the content of the source file

    // lseek to the position user input in switch option argument
    lseek(fd1, position, SEEK_SET);
    fprintf(stdout,"Position entered: %d\n", position);

    // Assign (file size - position) as block size if block size input exceeds (source file size - position)
    if(block > fileLength-position){
        fprintf(stdout, "Block size entered: %d exceeds source file length: %d\n", block, fileLength);
        fprintf(stdout, "Using the size: %d as the block size\n", fileLength-position);
        block = fileLength-position;
    }
    else{
        fprintf(stdout,"Block size entered: %d\n", block);
    }

    read(fd1, buf, block);  // read content from source file fd1
    write(fd2, buf, block); // write content to destination file fd2
    close(fd1);             // close fd1
    close(fd2);             // close fd2
}

void runTask(){
    // This function runs the task based on the specified options
    if(isPrinting){
        printFile();
    }
    if(isBlockGiven && isPositionGiven){
        copyBlockandPosition(); // if block size and position options are given
    }

    else if(isBlockGiven){
        copyBlock();            // if only block size options are given
    }

    else if(isPositionGiven){
        copyPositional();       // if only position options are given
    }
    else{
        exit(1);
    }
}

void usage (FILE *fp, char *argv[])
{
    // help message for this program
    fprintf(fp, "Usage: %s [options] [argument] <source file path> [destination file path]\n",argv[0]);
    fprintf(fp, "  -h, --help\t\t"
                 "Print this help and exit.\n");
    fprintf(fp, "  -b, --block-size\t"
                 "Indicate the block size to copy from the source file.\n");
    fprintf(fp, "  -p, --position\t"
                 "Indicate the position to copy from the source file.\n");
    fprintf(fp, "  -o, --output\t\t"
                 "Output the content in the source file.\n");
    exit(0);
}

int main(int argc, char *argv[]){
    int option;
    struct option longopts[] =
    {   // long options flag struct fot getopt_long function
        {"help", no_argument, NULL, 'h'},
        {"block-size", required_argument, NULL, 'b'},
        {"position", required_argument, NULL, 'p'},
        {"output", no_argument, NULL, 'o'},
        { 0, 0, 0, 0}
    };

    while ((option = getopt_long(argc, argv,"b:p:oh", longopts, 0)) != -1){
        if (option == -1){break;} // if end of option exit the loop

        switch(option){
            case 'b': // switch case for -b , --block-size option
            block = atoi(optarg); // parse block size argument string into int
            if(block < 0){
                // check for negative block size
                fprintf(stderr,"You have entered an invalid block number!\n");
                return 1;
            }
            isBlockGiven = 1;
            break;

            case 'p': // switch case for -p, --position option
            position = atoi(optarg); // parse position argument string into int
            isPositionGiven = 1;
            break;

            case 'o': // switch case for -o, --output option
            isPrinting = 1;
            break;

            case 'h': // switch case for -h , --help option
            help = 1;
            break;

            case '?':
            usage(stdout, argv); //print the help message
            break;
            default:
                return 0;
        }

    }
    if (help){
        usage(stdout, argv); // print the help message
    }
    getToFromFilePath(argv, &optind); // get the source file path and destination file path from the command line
    runTask();                        // run the task based on the command line options

    return 0;
    
}