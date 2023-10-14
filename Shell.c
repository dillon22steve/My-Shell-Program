#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string.h>

#define BUFFSIZE 4096


/**
* Parses the cmd array into a null terminated array of strings. If the user enters an I/O
* redirection character: "<", ">", or ">>", it will assign the next string to either the inputFile
* or outputFile variable and will not add the redirection character or the file name to the argsv
* array. It will also update the appropriate "boolean" variables: inpIsRedir, outIsRedir, and
* isAppend.
*
* @param cmd the command array to be parsed into strings.
* @param argsv the char pointer array that will be populated by the parsed strings.
* @param numArgs a pointer to the variable containing the number of arguments in the argsv array.
* @param inputFile the variable that will store the input file if the user includes "<" as an
* argument.
*
* @param outputFile the variable that will store the input file if the user includes ">" or ">>"
* as an argument.
*
* @param inpIsRedir used as a boolean value to tell if the user has entered an input redirect
* operator, assigned a value of 1 if the user enters "<".
*
* @param outIsRedir used as a boolean value to tell if the user has entered an output redirect,
* assigned a value of 1 if the user enters ">" or ">>".
*
* @param isAppend used as a boolean value to tell if the user has entered an output redirect to
* append to a file, assigned a value of 1 if the user enters ">>".
*/
void parseCmd(char cmd[], char* argsv[], int * numArgs, char** inputFile, char** outputFile,
int * inpIsRedir, int * outIsRedir, int * isAppend)
{
   *numArgs = 0;
   int argIndex = 0;
   int nextIsInpFile = 0;
   int nextIsOutFile = 0;
   char* token = strtok(cmd, " ");
 
   while (token != NULL) {
 
       //If the user entes an I/O redirect argument, the loop will not add it to argsv, and it will
       //assign the next argument to either the inputFile or outputFile variable depending on which
       //operator the user enters. It also assigns a "true" value to the inpIsRedir, outIsRedir, or
       //isAppend variables. Else it will add the argument to the argsv array.
       if (!strcmp(token, "<")) {
           nextIsInpFile = 1;
           *inpIsRedir = 1;
       } else if (!strcmp(token, ">") || !strcmp(token, ">>")) {
           nextIsOutFile = 1;
           *outIsRedir = 1;
           if (!strcmp(token, ">>")) *isAppend = 1;
       } else {
 
           //If the previous token was an I/O redirect character, the program will assign the
           //current token to either the inputFile or outputFile variables. Else it will add the
           //token to the argsv array.
           if (nextIsInpFile == 1) {
               *inputFile = token;
               nextIsInpFile = 0;
           } else if (nextIsOutFile == 1) {
               *outputFile = token;
               nextIsOutFile = 0;
           } else {
               argsv[argIndex] = token;
               argIndex++;
               (*numArgs)++;
           } //if
       } //if
 
       token = strtok(NULL, " ");
   } //while

   //Just to "use" the variables so the program will compile.
   if (inputFile == NULL)
   if (outputFile == NULL)
 
   //Assigns a null value to the last element of the argsv array so it is a null-terminated array
   argsv[argIndex] = NULL;
} //parseCmd

 
 
/**
* Resets the argsv array so that each element is set to NULL.
*
* @param argsv the array storing the user's command line arguments which will be reset to NULL.
* @param numArgs the number of arguments currently stored in argsv.
*/
void resetArgsv(char* argsv[], int numArgs) {
   for (int i = 0; i < numArgs; i++) {
       argsv[i] = NULL;
   } //for
} //resetArgsv
 
 

int main()
{
    setbuf(stdout, NULL); // makes printf() unbuffered
    int n;
    char cmd[BUFFSIZE];
    pid_t pid;
    char cwdBuffer[BUFFSIZE];


    char* home = getenv("HOME");
    if(chdir(home) == -1) perror("chdir");

    //Will store the user's current working directory.
    char* cwd;
 
    //Will be used to check if the user's home directory is a substring of the current working directory.
    char* homeSubString;


    // inifite loop that repeated prompts the user to enter a command
    while (1) {
        cwd = getcwd(cwdBuffer, BUFFSIZE);
        homeSubString = strstr(cwd, home);

        printf("1730sh:");
 

        //The first branch checks that the user's home directory does not appear at all in the
        //the current working directory. If so, it prints the full current working directory before
        //the $.The second branch checks if the user's home directory is the same as the current
        //working directory. If it is, it simply prints a ~. The else branch will be reached if the
        //user's home directory is a substring of the current working directory. If so, it will
        //print a ~ followed by the part of the current working directory that does not appear in
        //the home directory.
 
        if (homeSubString == NULL) {
            printf("%s$ ", cwd);
        } else if (strcmp(homeSubString, home) == 0) {
            printf("~$ ");
        } else {
            int len = strlen(cwd) - strlen(home);
            char cwdMinusHome[len];
            cwdMinusHome[len] = '\0';
 
            //creates a string of the portion of the current working directory that does not include
            //the home directory
            for (int i = 0, charToAdd = strlen(home); i < len; i++, charToAdd++) {
                cwdMinusHome[i] = cwd[charToAdd];
            } //for

            printf("~%s$ ", cwdMinusHome);
        } //if
 
 
        n = read(STDIN_FILENO, cmd, BUFFSIZE);
 
        // if user enters a non-empty command
        if (n > 1) {
            cmd[n-1] = '\0'; // replaces the final '\n' character with '\0' to make a proper string
 
 
            // Lab 06 TODO: parse/tokenize cmd by space to prepare the
            // command line argument array that is required by execvp().
            // For example, if cmd is "head -n 1 file.txt", then the
            // command line argument array needs to be
            // ["head", "-n", "1", "file.txt", NULL].
 
            char* argsv[BUFFSIZE];
            char * inputFile, * outputFile;
            int numArgs = 0;
 
 
            //boolean values that store whether or not the user has redirected the input or output.
            int inpIsRedir = 0;
            int outIsRedir = 0;
            int isAppend = 0;

 
            /*
             *Parses the command and populates argsv with a NULL terminated array of strings that
             *can be used in the execvp function later. Additionally, the function parses any I/O
             *redirection operators inputted by the user. If any redirection operator is found, it
             *updates the inpIsRedir, isAppend, or outIsRedir variable, depending on if it is an
             *input or output operator and assigns the file name to the inputFile or outputFile
             *variable.
            */
 
            parseCmd(cmd, argsv, &numArgs, &inputFile, &outputFile, &inpIsRedir, &outIsRedir, &isAppend);
 
 
 
            //If the user has entered an I/O redirect character, the program will open the file that
            //the user has provided, else it will do nothing.
            int fdi, fdo;
 
            //If the user has entered "<", it opens the input file and assigns its descriptor to fdi
            if (inpIsRedir == 1) {
                if ((fdi = open(inputFile, O_RDWR)) == -1) perror("open input");
            } else {
                inputFile = "";
            } //if
 
            //If the user has entered ">" or ">>", it opens the file if it exists, or creates the
            //file if it doesn't. If the user has entered ">>", it opens the file in append mode. It
            //also assigns the descriptor to fdo.
            if (outIsRedir == 1) {
                if (isAppend == 0) {
                    int oflags = O_RDWR | O_CREAT;
                    if ((fdo = open(outputFile, oflags, 0644)) == -1) perror("open output");
                } else {
                    int oflags = O_RDWR | O_CREAT | O_APPEND;
                    if ((fdo = open(outputFile, oflags, 0644)) == -1) perror("open output");
                } //if
            } else {
                outputFile = "";
            } //if


            if (strcmp(cmd, "exit") == 0) {
                return 0;
            } //if
 

 
            //strcmp returns 0 if the two strings are equal. If the user enters cd as the command,
            //the program will change the current directory and continue to the next iteration of
            //the while loop. If not, it will fork a separate process to execute whatever command
            //the user has entered, wait for the child process to finish, and then continue to the
            //next iteration of the while loop.
            if (strcmp(argsv[0], "cd") == 0) {
                if(chdir(argsv[1]) == -1) perror("chdir");
            } else {
                if ((pid = fork()) < 0) {
                    perror("fork");
                } else if (pid == 0) {
 
                    //Redirects standard input or standard output if the user has requested to do so.
                    if (inpIsRedir == 1) dup2(fdi, STDIN_FILENO);
                    if (outIsRedir == 1) dup2(fdo, STDOUT_FILENO);
 
                    if (execvp(argsv[0], argsv) < 0) {
                        perror("execvp");
                        exit(EXIT_FAILURE);
                    } //if
 
 
                } else {
                    int status;
                    wait(&status);
                } //if
            } //if
 
            resetArgsv(argsv, numArgs);
        } // if
    } // while
 
} // main