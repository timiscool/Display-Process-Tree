#include <stdio.h>
// Add your system includes here.

#include "ptree.h"
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

// Defining the constants described in ptree.h
const unsigned int MAX_PATH_LENGTH = 1024;

// If TEST is defined (see the Makefile), will look in the tests
// directory for PIDs, instead of /proc.
#ifdef TEST
    const char *PROC_ROOT = "tests";
#else
    const char *PROC_ROOT = "/proc";
#endif


/*
 * Creates a PTree rooted at the process pid. The root of the tree is
 * placed in root. The function returns 0 if the tree was created
 * successfully and 1 if the tree could not be created or if at least
 * one PID was encountered that could not be found or was not an
 * executing process.
 */
int generate_ptree(struct TreeNode **root, pid_t pid) {
    // Here's a trick to generate the name of a file to open. Note
    // that it uses the PROC_ROOT variable



    //instantize required parsing variables
    char procfile[MAX_PATH_LENGTH + 1]; // contains procfile directory
    char children_path[MAX_PATH_LENGTH + 1]; // contains children directory
    char cmdline[MAX_PATH_LENGTH + 1]; // contains cmdline directory
    char taskfile[MAX_PATH_LENGTH + 1]; // contains taskfile directory
    char cmdpathcontents[MAX_PATH_LENGTH+1]; // contains commandline contents
    int return_value = 0; // if encounter error, return 1
    int is_name = 0; // if cmdline has a name, set to 1
    int contains_path = 1; // if cmdline has no path, set to 0
    int root_child = 0; // first child of pid tree
    FILE *child_file; // file to open children
    FILE *command_line_file; // file to open commandline contents
    char *name; //holds the command line name
    int child_pid; // stores child_pid
    char children_contents[1024]; // stores children
    children_contents[0] = '\0'; //ensure children file is empty to start
    struct TreeNode *child_node = NULL; // node to build  children
    struct TreeNode *sibling = NULL; // node to build siblings
    char *resettok; // pointer for strtok_r
    char *child_pointer; // to hold contents of children file

    // Set the root of the tree
    *root = malloc(sizeof(struct TreeNode));
    (*root)->child_procs = NULL;
    (*root)->next_sibling = NULL;
    (*root)->pid = pid;
    // set the name after finding it in cmdline

    // Check for proc filename
    if (sprintf(procfile, "%s/%d/exe", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a filename\n");
        return 1;
    }
    // Check for children pathfile
    if (sprintf(children_path, "%s/%d/task/%d/children", PROC_ROOT, pid, pid) < 0){
        fprintf(stderr, "sprintf failed to produce children");
        return 1;
    }
    // Check for commandline filename
    if (sprintf(cmdline, "%s/%d/cmdline", PROC_ROOT, pid) < 0){
        fprintf(stderr, "sprintf failed to produce commandline\n");
        return 1;
    }
    // Check for taskfile
    if (sprintf(taskfile, "%s/%d/task", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a filename\n");
        return 1;
    }

    // check if the file executable exists
    struct stat *cstat;
    cstat = malloc(sizeof(struct stat));
    //check if the exe exists
    if (lstat(procfile,cstat) < 0) {
        fprintf(stderr,"THE PID executable does not exist\n");
        return 1;
    }

    command_line_file = fopen(cmdline, "r");
    // check if command line file was succesfully opened
    if (command_line_file == NULL){
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    // take the command line string
    if (fscanf(command_line_file, "%s", cmdpathcontents) != EOF){
        is_name = 1;
        // get the name
        if (strpbrk("/", cmdpathcontents) != NULL){
            name = strrchr(cmdpathcontents, '/');
        }

    else{
        contains_path =0;
    }
}
    //set the PIDs name
    if (is_name){
        (*root)->name = malloc(sizeof(char)*(MAX_PATH_LENGTH+1));
        if (contains_path){
            printf("has a name\n");
            strncpy((*root)->name, name, MAX_PATH_LENGTH);
        }
        else{
            strncpy((*root)->name, cmdpathcontents, MAX_PATH_LENGTH+1);
        }
    }
    else{
        // if the PID has no name, set it's name to NULL
        (*root)->name = NULL;
    }


    // get children
    child_file = fopen(children_path, "r");
    // check if children file succesfully opens
    if (child_file == NULL){
        fprintf(stderr, "Error opening children file\n");
        return 1;
    }

    // store our file contents
    child_pointer = fgets(children_contents, 1024, child_file);
    if (strlen(children_contents) != 0){
        child_pointer = strtok_r(children_contents, " ", &resettok);

    while (child_pointer != NULL){
        child_pid = strtol(child_pointer, NULL, 10);
        if (generate_ptree(&child_node, child_pid)==1){
            child_node = NULL;
            return_value = 1;
            child_pointer = strtok_r(NULL, " ", &resettok);

        }

        if (!(root_child)){
            root_child = 1;
            // link children
            (*root)->child_procs = child_node;
        }
        else{
            // link sibling
            sibling->next_sibling = child_node;
        }
        sibling = child_node;
        child_node = NULL;
        child_pointer = strtok_r(NULL, " ", &resettok);
    }
}

//  attempt succesfully close children file, 1 if not
int children_file_error = fclose(child_file);
if (children_file_error != 0){
    printf("Error closing children file\n");
    return 1;
}
//  attempt succesfully close command line file, 1 if not
int command_line_file_error = fclose(command_line_file);
if (command_line_file_error != 0){
    printf("Error closing command line file\n");
    return 1;
    }

    return return_value;
}

/*
 * Prints the TreeNodes encountered on a preorder traversal of an PTree
 * to a specified maximum depth. If the maximum depth is 0, then the
 * entire tree is printed.
 */
void print_ptree(struct TreeNode *root, int max_depth) {

    static int depth = 0; // maintain our depth in the tree
    printf("%*s", depth * 2, ""); // format deph correctly

    // if the PID has a name, print it
    if((*root).name != NULL){
    printf("%d: %s\n", (*root).pid, (*root).name);
    }
    // otherwise, just print the PID
    else{
    printf("%d: \n", (*root).pid);
    }
    // level ourselves in the tree
    depth ++;

    if ((max_depth == 0) || (depth <= max_depth - 1)){
        if ((*root).child_procs != NULL){
            print_ptree((*root).child_procs, max_depth);
        }
    }
    //relevel ourselves in the tree
    depth --;
    if ((*root).next_sibling != NULL){
        print_ptree((*root).next_sibling, max_depth);
    }
    else{
        return;
    }
}
