#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdbool.h>
// #include<argp.h>

int option;
int idx;
char *block = NULL;
char *positions = NULL;
char *fromPath = NULL;
char *toPath = NULL;
char *path = NULL;
char **positionals;
char *argcc = NULL;
bool isBlockGiven = false;
bool isPositionGiven = false;
bool isPrinting = false;

char copyFileContent( char *block,char *argv[], int *optind){
    //TODO file opening, file copying based on user input block size. 
    fprintf(stdout,"\nCopy File Content function\n");
    printf("---you entered %d as block size\n", block);
    int idx = optind;
    fromPath = argv[idx];
    toPath = argv[idx + 1];

    fprintf(stdout, "from: %s\n", fromPath);
    fprintf(stdout, "to: %s\n", toPath);
}

// char position() function

// char printOut() function


int getToFromFilePath( char *argv[], int *optind) {
    int idx = optind;
    char *from = argv[idx];
    char *to = argv[idx + 1];

    fprintf(stdout, "Origin File Path:%s, Destination Filepath:%s\n", from, to);

    /* printf("%d", optind);
    for (int idx = optind; idx < argc; idx++){
        fprintf(stdout, "Positional: %s, idx: %d\n", argv[idx], idx);
    } */
}


int main(int argc, char *argv[]){

    while ((option = getopt(argc, argv,"b:c:f:t:o:")) != -1){

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
            printf("this is flag -o\n");
            printf(" you entered %s as file path to print\n", path);
            // check toprint file path validity
            // call toPrint() function
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
                fprintf(stderr, "Usage: %s [-bcp] [arguments]\n", argv[0]);
                return 1;
        }

    }
    //copyFileContent(block, fromPath, toPath, &argv);

    //positionals = &argv[optind];
    /* printf("%d", optind);
    for (idx = optind; idx < argc; idx++){
        fprintf(stdout, "Positional: %s, idx: %d\n", argv[idx], idx);
    } */
    getToFromFilePath(/* positionals, argc, */ argv, optind);

    //continue the actual feature work here 
    if(isBlockGiven){
        copyFileContent(block, argv, optind);
    }
    if(isPositionGiven){}
    if(isPrinting){}

    return 0;
    
}