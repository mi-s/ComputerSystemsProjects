#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int hasBatch = 0;
FILE* batch = NULL;

void writeStr(char* msg){
    write(STDOUT_FILENO, msg, strlen(msg));
}

/* error: prints error message
 * if kill != 0, exit process
 * else, return to process */
void error(char kill){
    char error_message[30] = "An error has occurred\n";
    writeStr(error_message);
    if(kill){
        if(hasBatch) {fclose(batch);}
        exit(0);
    }
}

/* lenError: handler for commands that are
 * longer than 512 characters long, not including "\n" */
void lenError(){
    char extraInput;
    if(hasBatch){
        while(((extraInput = getc(batch)) != '\n') && (extraInput != '\0')){
            write(STDOUT_FILENO, &extraInput, 1);
        }
    }
    else{ 
        while(((extraInput = getc(stdin)) != '\n') && (extraInput != '\0')){
            write(STDOUT_FILENO, &extraInput, 1);
        }
    }
    write(STDOUT_FILENO, &extraInput, 1);
    error(0);
}

/* checkEmpty: returns 1 for strings that have any character other than a 
 * space, tab, or arrow */
int checkEmpty(char* input){
    if(input == NULL) {return 0;}
    int i = 0;
    char c = input[i];
    while(c != '\0'){
        if((c != ' ') && (c != '\t') && (c != '\n')) {return 1;}
        i++;
        c = input[i];
    }
    return 0;
}

/* checkRedirect: returns 1 for one '>', 2 for one '>+', 3 for more than one of
 * either '>' and '>+', and 0 if no signs appear */
int checkRedirect(char* input){
    int arrow = 0, arrowpl = 0;
    int i = 0;
    char c = input[i];
    while(c != '\0'){
        if(c == '>') {
	    char next = input[i + 1];
	    if(next == '+'){
                i++;
		arrowpl++;
	    } else {arrow++;}
	}
	i++;
	c = input[i];
    }
    if((arrow && arrowpl) | (arrow > 1) | (arrowpl > 1)) {return 3;}
    if(arrow) {return 1;}
    if(arrowpl) {return 2;}
    return 0;
}

// exitCmd: handles exit cmd
void exitCmd(){
    exit(0);
}

// cdCmd: handles cd cmd
void cdCmd(char* arg2){
    int err;
    if(arg2 == NULL){
        char* home = getenv("HOME");
	err = chdir(home);
    } else {err = chdir(arg2);}
    if(err) {error(0);}
}

// pwdCmd: handles pwd cmd
void pwdCmd(){
    char* cwd = getcwd(NULL, 0);
    if(cwd == NULL) {error(0);}
    else{
	writeStr(cwd);
        writeStr("\n");
        free(cwd);
    }
}

void prepend(int outputfd, int tempfd){
    char byte;
    while(read(outputfd, &byte, 1)) {write(tempfd, &byte, 1);}
    lseek(tempfd, 0, SEEK_SET);
    ftruncate(outputfd, 0);
    while(read(tempfd, &byte, 1)) {write(outputfd, &byte, 1);}
    remove("tmp");
}

// execCmd: handles non-built-in functions and their potential redirection
void execCmd(char* arg, char* arg2, char** saveptr, char* outputpath, int redirect){
    int cid = fork();
    if(cid) {
        wait(NULL);
        if((redirect == 2) && (!(access("tmp", F_OK)))){
            char* pathptr;
            char* path = strtok_r(outputpath, " \t", &pathptr);
            if(path != NULL){
                if(strtok_r(NULL, " \t", &pathptr) != NULL) {error(0);}
                else{
                    int outputfd = open(path, O_RDWR | O_APPEND);
                    int tempfd = open("tmp", O_RDWR | O_APPEND);
                    prepend(outputfd, tempfd);
                    close(outputfd);
                    close(tempfd);
                }
            } else{
                error(0);
            }
        }
    }
    else{
	int outputfd = -1;
        if(redirect){
	    char* pathptr;
	    char* path = strtok_r(outputpath, " \t", &pathptr);
            int tempfd = -1;
	    if(strtok_r(NULL, " \t", &pathptr) != NULL) {error(1);}
	    if(redirect == 2) {
	        outputfd = open(path, O_RDWR | O_CREAT, S_IRWXU);
                tempfd = open("tmp", O_RDWR | O_CREAT, S_IRWXU);
	    } else{
		outputfd = open(path, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);
	    }
	    if(outputfd == -1) {
                if(!(access("tmp", F_OK))) {
                    remove("tmp");
                    close(tempfd);
                }
                error(1);
            }
            if(tempfd != -1) {dup2(tempfd, STDOUT_FILENO);}
	    else {dup2(outputfd, STDOUT_FILENO);}
	}
	char* argv[256];
	argv[0] = arg;
	argv[1] = arg2;
	int i = 2;
	char* nextarg = strtok_r(NULL, " \t", saveptr);
	while(nextarg != NULL){
	    argv[i] = nextarg;
	    i++;
	    nextarg = strtok_r(NULL, " \t", saveptr);
	}
	argv[i] = NULL;
	int err = execvp(argv[0], argv);
	if(err == -1) {error(1);}
	exit(0);
    }
}

/* argHandler: handles one or more arguments in a job and writes output
*  to the appropriate file indicated by outputpath */
void argHandler(char* job, char* outputpath, int redirect){
    char* saveptr;
    char* arg = strtok_r(job, " \t", &saveptr);
    char* arg2 = strtok_r(NULL, " \t", &saveptr);
    if(arg == NULL) {return;}
    else if(!(strcmp(arg, "exit"))){
        if((arg2 != NULL) | redirect) {error(0);}
        else {exitCmd();}
    } else if(!(strcmp(arg, "cd"))){
	char* arg3 = strtok_r(NULL, " \t", &saveptr);
     	if((arg3 != NULL) | redirect) {error(0);}
	else {cdCmd(arg2);}
    } else if(!(strcmp(arg, "pwd"))) {
	if((arg2 != NULL) | redirect) {error(0);}
	else{pwdCmd();}
    } else {execCmd(arg, arg2, &saveptr, outputpath, redirect);}
}

// lineHandler: handles >=1 command separated by ';' in a cmd line 
void lineHandler(char* input){
    char* saveptr;
    char* cmd = strtok_r(input, ";\n", &saveptr);
    while(cmd != NULL){
        int redirect = checkRedirect(cmd);
        if(redirect == 3) {
            error(0);
            cmd = strtok_r(NULL, ";\n", &saveptr);
            continue;
        }
        char* job;
	char* outputpath;
        if(redirect){
	    char* saveptr2;
            job = strtok_r(cmd, ">", &saveptr2);
	    outputpath = strtok_r(NULL, ">", &saveptr2);
            if((redirect == 2) && (outputpath != NULL)) {outputpath++;}
            int emptyJob = !(checkEmpty(job));
            int emptyPath = !(checkEmpty(outputpath));
            if(emptyJob | emptyPath | (outputpath == NULL)){
                error(0);
                cmd = strtok_r(NULL, ";\n", &saveptr);
                continue;
            }
        } else {job = cmd;}
        argHandler(job, outputpath, redirect);
        cmd = strtok_r(NULL, ";\n", &saveptr);
    }
}

int main(int argc, char *argv[]){
    if(argc > 2) {error(1);}
    if(argc == 2) {
        batch = fopen(argv[1], "r");
     	if(batch == NULL) {error(1);}
        hasBatch = 1;
    }
    char* input;
    char cmdBuff[514];
    while(1){
        if(hasBatch) {
            input = fgets(cmdBuff, 514, batch);
            if(input == NULL) {return 0;}
        }
	else{
            writeStr("myshell> ");
	    input = fgets(cmdBuff, 514, stdin);
	}
	if((strlen(input) == 513) && (input[512] != '\n')){
	    writeStr(input);
            lenError();
	    continue;
	}
	int notEmpty = checkEmpty(input);
	if(notEmpty) {
	    writeStr(input);
	    lineHandler(input);
	}
    }
    return 0;
}
