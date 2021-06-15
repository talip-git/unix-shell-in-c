/*
 * CMPE 382 Homework 1 Unix Shell Implementation
 * Ahmet Cemal Sert ID:43999263780
 * Talip Sina Postacı ID:19390083036
 * */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#define MAXCHARS 512 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

/*
 * Takes 2 parameters  commands and pipedarguments and it loops the arguments and
 * separate them according to '|' and parses the new arguments to the pipedArguments array
 * returns 1 if there are piped arguments, if not returns 0
 */
int parsePipedCommands(char *command, char **pipedArguments) {
    int i;
    for (i = 0; i < 3; i++) {
        pipedArguments[i] = strsep(&command, "|");
        if (pipedArguments[i] == NULL)
            break;
    }
    // if no pipe is found executes this block
    if (pipedArguments[1] == NULL)
        return 0;
    else {
        return 1;
    }
}
/*
 * Takes 2 parameters  commands and pipedarguments and it loops the arguments and
 * separate them according to ';' and parses the new arguments to the pipedArguments array
 * returns 1 if there are piped arguments, if not returns 0
 */
int parseSemiColonCommands(char *command, char **pipedArguments) {
    int i;
    for (i = 0; i < 3; i++) {
        pipedArguments[i] = strsep(&command, ";");
        if (pipedArguments[i] == NULL)
            break;
    }
    // if no pipe is found executes this block
    if (pipedArguments[1] == NULL)
        return 0;
    else {
        return 2;
    }
}
/*
 * Takes two parameters command and parsedArguments
 * it loops through the parsedArguments array and separates it according to space (" ")
 * */
void parseSpace(char *command, char **parsedArguments) {
    int i;
    for (i = 0; i < MAXLIST; i++) {
        parsedArguments[i] = strsep(&command, " ");
        if (parsedArguments[i] == NULL)
            break;
    }
}

/*
 * The function that gets the current directory using getcwd() function
 * and prints it to screen by printf
 * * */
void getCurrentDirectory() {
    char dir[512];
    getcwd(dir, sizeof(dir));
    printf("\nCurrent Directory: %s", dir);
}

/*
 * Forks a child using fork() function from unistd.h
 * if pid is -1 it means fork operation is not successful and prints an error message
 * if pid is 0 and execvp function is smaller than 0 prints an error message(since it means it is not successful)
 * else executes the system command
 * */
void executeCommands(char **parsedArguments) {
    pid_t pid = fork();
    if (pid == -1) {
        printf("\n ERROR! Fork failed !!");
        return;
    } else if (pid == 0) {
        if (execvp(parsedArguments[0], parsedArguments) < 0) {
            printf("\n ERROR!! Command can't be executed!!");
        }
        exit(0);
    } else {
        wait(NULL); // waits the child
        return;
    }
}

/*
 *  This function executes the piped commands which separated with '|'
 * */
void executePipedCommands(char **parsedArguments, char **pipedArguments, char **pipedArguments2) {

    int pipefd[2]; // file descriptor array
    pid_t p1, p2, p3;

    /*
     * Creates a pipe using pipe () function if it returns a value smaller than 0
     * It means pipe operation isn't successful and this if block is executed
     * */
    if (pipe(pipefd) < 0) {
        printf("\n ERROR! Pipe couldn't created !!");
        return;
    }
    p1 = fork(); // forks a child

    /*
     * if p1<0 it means fork operation isn't successfull and this block will be executed
     * */

    if (p1 < 0) {
        printf("\nERROR! Fork operation is not successful.");
        return;
    }

    // this block contains the operations that will happen in the child process
    if (p1 == 0) {

        close(pipefd[0]); // closes the file descriptor
        dup2(pipefd[1], 1);
        close(pipefd[1]); // closes the file descriptor

        if (execvp(parsedArguments[0], parsedArguments) < 0) {
            printf("\nERROR!! Command couln't executed!!");
            exit(0);
        }
    }
        // this block contains the operations that will happen in the parent process
    else {
        p2 = fork();

        if (p2 < 0) {
            printf("\n ERROR! Fork failed !!");
            return;
        }

        // Child is executing
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], 0);
            close(pipefd[0]);
            if (execvp(pipedArguments[0], pipedArguments) < 0) {
                printf("\n ERROR!! Command couldn't executed!!");
                exit(0);
            }
        } else {
            p3 = fork();
            if (p3 < 0) {
                printf("\n ERROR! Fork failed !!");
                return;
            }

            // Child is executing
            if (p3 == 0) {
                close(pipefd[0]); // closes the file descriptor
                dup2(pipefd[1], 1);
                close(pipefd[1]); // closes the file descriptor
                if (execvp(pipedArguments2[0], pipedArguments2) < 0) {
                    printf("\n ERROR!! Command couldn't executed!!");
                    exit(0);
                }
            } else {
                wait(NULL);
                wait(NULL);
                wait(NULL);
            }
        }
    }
}
/*
 *  This function executes the piped commands which separated with ';'
 * */
void executeSemiColonCommands(char **parsedArguments, char **pipedArguments,char **pipedArguments2){
    pid_t p1,p2,p3;

    p1 = fork();
    /*
    * if p1<0 it means fork operation isn't successful and this block will be executed
    * */
    if (p1 < 0) {
        printf("\n ERROR! Fork failed !!");
        return;
    }
    //If fork successful than id == 0
    if (p1 == 0){
        //Execute the command from kernel
        if (execvp(parsedArguments[0], parsedArguments) < 0) {
            printf("\n ERROR!! Command couln't executed!!");
            exit(0);
        }
        exit(0);
    }
    p2 = fork();
    /*
    * if p2<0 it means fork operation isn't successful and this block will be executed
    * */
    if (p2 < 0) {
        printf("\n ERROR! Fork failed !!");
        return;
    }
    //If fork successful than id == 0
    if (p2 == 0){
        //Execute the command from kernel
        if (execvp(pipedArguments[0], pipedArguments) < 0) {
            printf("\n ERROR!! Command couln't executed!!");
            exit(0);
        }
        exit(0);
    }
    p3 = fork();
    /*
    * if p3<0 it means fork operation isn't successful and this block will be executed
    * */
    if (p3 < 0) {
        printf("\n ERROR! Fork failed !!");
        return;
    }
    //If fork successful than id == 0
    if (p3 == 0){
        //Execute the command from kernel
        if (execvp(pipedArguments2[0], pipedArguments2) < 0) {
            printf("\n ERROR!! Command couln't executed!!");
            exit(0);
        }
        exit(0);
    }
    else {
        wait(NULL);
        wait(NULL);
        wait(NULL);
    }
}
/*
* The function that execute builtin commands
* Takes parsedArguments as parameter
* In our case there are 2 default arguments which is cd and quit
* It checks if the parsedArgument checks with our default arguments.
* If count is 1 it means  it is the means quit command and it will quit the program using exit() function
* If count is 2 it means cd command and it will change the directory using chdir() function and returns 1
* */
int commandHandler(char **parsedArguments) {
    char *builtInArguments[2];
    builtInArguments[0] = "quit";
    builtInArguments[1] = "cd";
    int count = 0;
    for (int i = 0; i < 2; i++) {
        if (strcmp(parsedArguments[0], builtInArguments[i]) == 0) {
            count = i + 1;
            break;
        }
    }
    if (count == 1) {
        printf("Quitting from the Shell!\n");
        exit(0);
    } else if (count == 2) {
        chdir(parsedArguments[1]);
        return 1;
    }
    return 0;
}
/*
 * Function that handles the main command
 * if first checks whether command is a piped or semi coloned argument by sending
 * the main command into functions parsePipedArguments() and parseSemiColonCommands()
 * if the the command is a piped command than we separate the piped into three different parts
 * and assign them to different arrays
 * and return 1+piped = 2 to the main function
 * if the the command is a semi coloned command than we separate the semi colon  into three different parts
 * and assign them to different arrays
 * and return 1+semiColon = 3 to the main function
 * */
int StringHandler(char *command, char **parsedArguments, char **pipedArguments,char **pipedArguments2) {
    char *pipedArr[3];
    char *semiArgs[3];
    int semiColon = 0;
    int piped = 0;

    piped = parsePipedCommands(command, pipedArr);
    semiColon = parseSemiColonCommands(command, semiArgs);

    if (piped) {
        parseSpace(pipedArr[0], parsedArguments);
        parseSpace(pipedArr[1], pipedArguments);
        parseSpace(pipedArr[2], pipedArguments2);

    }
    else if(semiColon){
        parseSpace(semiArgs[0], parsedArguments);
        parseSpace(semiArgs[1], pipedArguments);
        parseSpace(semiArgs[2], pipedArguments2);
    }
    else {

        parseSpace(command, parsedArguments);
    }
    if (commandHandler(parsedArguments))
        return 0;
    else if(piped){
        return 1+piped;
    }
    else
        return 1 + semiColon;
}
/*
    * readCommands function reads the command from keyboard
    * If the length of the command is not zero adds list to history list then copies the list into command and returns 0
    * If the command has more than 512 characters it prints an error message and returns 1
    * Else it returns 1
    * */

int readCommands(char *command) {
    char *list;

    list = readline("\nPromt> ");
    if (strlen(list) != 0) {
        add_history(list);
        strcpy(command, list);
        return 0;

    } else if (strlen(list) >= 512) {
        printf("Eror:Too much characters!\n");
        return 1;
    }
    else {
        return 1;
    }
}
int main() {
    char commands[MAXCHARS]; // initialize commands array with 512 max characters
    char *parsedArguments[MAXLIST]; // initialize parsedArguments array with size of 100
    char *pipedArguments[MAXLIST]; // initialize pipedArguments array with size of 100
    char *pipedArguments2[MAXLIST];
    int loopFlag = 0;

    while (1) {

        // prints current directory
        getCurrentDirectory();

        // take commands
        if (readCommands(commands))
            continue;

        loopFlag = StringHandler(commands,parsedArguments, pipedArguments,pipedArguments2);

        /*
         * loopFlag returns zero if there are no commands
         * returns 1 if it is a simple command
         * returns 2 if it is a piped command
         * returns 3 if it is a semi coloned command
         * */
        // execute
        if (loopFlag == 1)
            executeCommands(parsedArguments);
        if (loopFlag == 2)
            executePipedCommands(parsedArguments, pipedArguments, pipedArguments2);
        if (loopFlag == 3)
            executeSemiColonCommands(parsedArguments, pipedArguments,pipedArguments2);
    }
    return 0;
}

