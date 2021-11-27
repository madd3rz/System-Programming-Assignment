#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<getopt.h>
// #include<argp.h>

int block = 0;
char *positions = NULL;
char *fromPath = NULL;
char *toPath = NULL;
bool isBlockGiven = false;
bool isPositionGiven = false;
bool isPrinting = false;
double static featureCheck = 0;

char copyFileContent( char *block,char *argv[], int *optind){
    //TODO file opening, file copying based on user input block size. 
    fprintf(stdout,"\nCopy File Content function\n");
    printf("---you entered %s as block size\n", block);
    int idx = *optind;
    fromPath = argv[idx];
    toPath = argv[idx + 1];

    fprintf(stdout, "from: %s\n", fromPath);
    fprintf(stdout, "to: %s\n", toPath);
}

int getToFromFilePath( char *argv[], int *optind) {
    int idx = *optind;
    fromPath = argv[idx];
    toPath = argv[idx + 1];

    if(access(fromPath,F_OK) == 0 && access(toPath,F_OK) == 0){ // check if the file exists in the directory specified
        fprintf(stdout, "\nOrigin File Path:%s\n", fromPath);
        if(toPath){ fprintf(stdout, "Destination Filepath:%s\n", toPath); }
    }
}

void checkFeatures(){
    double baseFeatureCheck = 2;
    double blockFeature = 1;
    double positionFeature = 2;
    double printingFeature = 3;

    if(isBlockGiven){
        //copyFileContent(block, argv, optind);
        fprintf(stdout, "\nfeatureCheck before = %f, basefeat: %f\n", featureCheck, baseFeatureCheck);
        fprintf(stdout, "blockfeat = %f\n", blockFeature);
        featureCheck += pow(baseFeatureCheck, blockFeature);
        
        fprintf(stdout, "featureCheck after = %f\n", featureCheck);
    }
    if(isPositionGiven){
        fprintf(stdout, "\nfeatureCheck before 2= %f, basefeat: %f\n",featureCheck ,baseFeatureCheck);
        fprintf(stdout, "position = %f\n", positionFeature);
        featureCheck += pow(baseFeatureCheck, positionFeature);
        
        fprintf(stdout, "featureCheck after 2= %f\n", featureCheck);
    }
    // if(isPrinting){
    //     fprintf(stdout, "\nprintingCheck before 3= %f, basefeat: %f\n",featureCheck ,baseFeatureCheck);
    //     fprintf(stdout, "printing = %f\n", printingFeature);
    //     featureCheck += pow(baseFeatureCheck, printingFeature);
        
    //     fprintf(stdout, "printingCheck after 2= %f\n", featureCheck);
    // }
}

void printFile(){

    int fd = open(fromPath, O_RDONLY );

	if(fd < 0){
		perror("Cannot open file!");
		exit(1);
	}

	printf("%s open succussful\n", fromPath);

    int fileLength = lseek( fd, 0, SEEK_END );  // get file length using lseek()
    if ( fileLength < 0 ){
        exit(1);
    }
    lseek(fd, 0, SEEK_SET); // return to beginning position of file

    char *content[fileLength];
    read(fd, content, fileLength);// read content from fd
    write(1, content, fileLength);// 1 refers to stdout

    close(fd);

}

void runTask(){

    checkFeatures();

    if(isPrinting){
        printFile();
    }
    switch((int)featureCheck){

        case 2:
            fprintf(stdout, "block given here size= %d\n", block);
            break;
        case 4:
            fprintf(stdout, "position given here position= %d\n", *positions);
            break;
        case 6:
            fprintf(stdout, "block and position given here\n");
            break;
        default:
            fprintf(stdout, "\n--------------------------------\n");
    }
}

int main(int argc, char *argv[]){
    int option;

    while ((option = getopt(argc, argv,"b:c:f:t:o::")) != -1){

        switch(option){
            case 'b':
            block = atoi(optarg);
            if(block < 0){
                printf("Negative block size entered.\n");
                break;
            }
            printf("this is flag -b\n");
            printf("You entered %d as block size\n", block);
            //copyFileContent(block, &argv, optind);
            isBlockGiven = true;
            break;

            case 'c':
            positions = optarg;
            printf("this is flag -c\n");
            printf(" you entered %s as position\n", positions);
            isPositionGiven = true;
            break;

            /* case 'f':
            fromPath = optarg;
            printf("this is flag -f\n");
            printf(" you entered %s as origin file path\n", fromPath);
            // check origin file path validity
            break;

            case 't':
            toPath = optarg;
            printf("this is flag -t\n");
            printf(" you entered %s as destination file path\n", toPath);
            // check destination file path validity
            break; */

            case 'o':
            //path = optarg;
            printf("this is flag -o\n");
            //printf(" you entered %s as file path to print\n", path);
            // check toprint file path validity
            // call toPrint() function
            isPrinting = true;
            break;

            case '?':
            if (optopt == 'b')
            {
                printf("Missing block size input after -b flag\n");
            }
            else if (optopt == 'c')
            {
                printf("Missing the position input after -c flag\n");
            }
            /* else if (optopt == 'f')
            {
                printf("Missing the original file path input after -p1 flag\n");
            }
            else if (optopt == 't')
            {
                printf("Missing the destination file path input after -p2 flag\n");
            } */
            else if (optopt == 'o')
            {
                printf("Missing the file path input after -o flag\n");
            }
            else{
                printf("Invalid option");
            }
            break;
            default:
                fprintf(stderr, "Usage: %s [-bco] <arguments>\n",argv[0]);
                exit(1);
        }

    }
    //copyFileContent(block, fromPath, toPath, &argv);

    //positionals = &argv[optind];
    /* printf("%d", optind);
    for (idx = optind; idx < argc; idx++){
        fprintf(stdout, "Positional: %s, idx: %d\n", argv[idx], idx);
    } */
    getToFromFilePath(/* positionals, argc, */ argv, &optind);

    runTask();

    return 0;
    
}