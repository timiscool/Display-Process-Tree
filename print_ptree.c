#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "ptree.h"
#include <unistd.h>
#include <ctype.h>

/* Parse the command line to get appropiate arguments to generate and
print our tree. If arguments are insufficient, throw errors to the user*/

int main(int argc, char **argv) {
    struct TreeNode *root = NULL; // root to give our tree
    char *dvalue = NULL;
    int d;
    int depth = 0; // to set required printing depth (initially 0)
    opterr = 0;

    // if we have a flag
    if (argc == 2 || argc == 4){
        if (argc == 4){
            depth = strtol(argv[2],NULL,10);
        }
   // while there is an argument, parse the cmdline
    while((d = getopt (argc, argv, "d:")) != -1){
        switch(d)
        {
            case 'd':
            dvalue = optarg;
            break;

        case '?':
        if (optopt == 'd'){
            fprintf(stderr, "Unkown option\n");
        }
        else if(isprint (optopt)){
            fprintf(stderr, "unkown option\n");
        }
        else{
            fprintf(stderr, "Unkown option\n");
            return 1;
        }
    default:
        abort();
    }
    }
}
    else{
        fprintf(stderr, "useage -d N [PID] \n");
        return 1;
    }
    
    dvalue ++;
    generate_ptree(&root, strtol(argv[optind], NULL, 10));
    print_ptree(root, depth); // set the required depth
    return 0;

}
