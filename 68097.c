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
    int idx = *optind;
    fromPath = argv[idx];
    toPath = argv[idx + 1];

    if(access(fromPath,F_OK) == 0 && access(toPath,F_OK) == 0){ // check if the file exists in the directory specified
        fprintf(stdout, "Origin File Path:%s\n", fromPath);
        if(toPath){ fprintf(stdout, "Destination Filepath:%s\n", toPath); }
    }
}

int getFileLength(int fd){
    int fileLength = lseek( fd, 0, SEEK_END );  // get file length using lseek()
    if ( fileLength < 0 ){
        exit(1);
    }
    lseek(fd, 0, SEEK_SET); // return to beginning position of file after seek to EOF above

    return fileLength;
}

int openFromFile(char *path){
    int fd = open(path,O_RDONLY); // open the source file with O_RDONLY permission 
    if(fd < 0){
		perror("Error open file!");
		exit(1); // exit program if errpr opening source file 
	}
    return fd;
}

int openToFile(char *path){
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

    if(fromPath == NULL){
        fprintf(stderr,"Error: No file specified.\n");
        exit(1); //exit program if not found source file
    }
    
    int fd = openFromFile(fromPath);
    int fileLength = getFileLength(fd);
    char *buf[fileLength]; // buffer to hold the content of source file

    fprintf(stdout, "\nContent of source file: \n");
    read(fd, buf, fileLength);// read content from source file fd
    write(1, buf, fileLength);// write to stdout
    close(fd);
    fprintf(stdout, "--- EOF ---\n");
}

void copyBlock(){
    int fd1 = openFromFile(fromPath);
    int fd2 = openToFile(toPath);
    int fileLength = getFileLength(fd1);
    char buf[block]; // buffer to hold the content of teh source file

    // Assign the file size as the block size if user input block size exceeded the file size
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

    int fd1 = openFromFile(fromPath);
    int fd2 = openToFile(toPath);
    int fileLength = getFileLength(fd1);

    // lseek to the position user input
    lseek(fd1, position, SEEK_SET);
    fprintf(stdout,"Position entered: %d\n", position);

    char *buf[fileLength];                 // buffer to hold the content of source file
    read(fd1, buf, fileLength-position);   // read the content of the source file into buffer
    write(fd2, buf, fileLength-position);  // write the buffer to the destination file
    close(fd1);                            // close fd1
    close(fd2);                            // close fd2
}

void copyBlockandPosition(){

    int fd1 = openFromFile(fromPath);
    int fd2 = openToFile(toPath);
    int fileLength = getFileLength(fd1);
    char buf[block];        // buffer to hold the content of the source file

    // lseek to the position user input in switch option argument
    lseek(fd1, position, SEEK_SET);
    fprintf(stdout,"Position entered: %d\n", position);

    // Assign the file size minus the position as block size if block size input exceeds source file size
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

    if(isPrinting){
        printFile();
    }
    if(isBlockGiven && isPositionGiven){
        copyBlockandPosition();
    }

    else if(isBlockGiven){
        copyBlock();
    }

    else if(isPositionGiven){
        copyPositional();
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
    {   // long options flag struct
        {"help", no_argument, NULL, 'h'},
        {"block-size", required_argument, NULL, 'b'},
        {"position", required_argument, NULL, 'p'},
        {"output", no_argument, NULL, 'o'},
        { 0, 0, 0, 0}
    };

    while ((option = getopt_long(argc, argv,"b:p:oh", longopts, 0)) != -1){
        if (option == -1){break;}

        switch(option){
            case 'b':
            block = atoi(optarg); // parse block size argument string into int
            if(block < 0){
                // check for negative block size
                fprintf(stderr,"You have entered an invalid block number!\n");
                return 1;
            }
            isBlockGiven = 1;
            break;

            case 'p':
            position = atoi(optarg); // parse position argument string into int
            isPositionGiven = 1;
            break;

            case 'o':
            isPrinting = 1;
            break;

            case 'h':
            help = 1;
            break;

            case '?':
            usage(stdout, argv);
            break;
            default:
                return 0;
        }

    }
    if (help){
        usage(stdout, argv);
    }
    getToFromFilePath(argv, &optind);
    runTask();

    return 0;
    
}