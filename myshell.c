// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed:_____Bradley Snyder______ Date:______4/18/2019______

// 3460:426 Lab 4A - Basic C shell

/* Basic shell */

/*
 * This is a very minimal shell. It finds an executable in the
 * PATH, then loads it and executes it (using execv). Since
 * it uses "." (dot) as a separator, it cannot handle file
 * names like "minishell.h"
 *
 * The focus on this exercise is to use fork, PATH variables,
 * and execv. 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS	64
#define MAX_ARG_LEN	16
#define MAX_LINE_LEN	80
#define WHITESPACE	" ,\t\n"

struct command_t {
   char *name;
   int argc;
   char *argv[MAX_ARGS];
};

/* Function prototypes */
int parseCommand(char *, struct command_t *);
void printPrompt();
void readCommand(char *);

int main(int argc, char *argv[]) {
    int pid;        /* the process id */ 
    int status;     /* status of our process */
    char cmdLine[MAX_LINE_LEN];
    struct command_t command;

    while (1) { 
        printPrompt();
        /* Read the command line and parse it */
        readCommand(cmdLine);
        /* build argv and command object */
        parseCommand(cmdLine, &command);

        char* argument = command.argv[0]; 

        /* give the last char NULL value for standard. */ 
        command.argv[command.argc] = NULL;

        /* H (help) displays the manual of the linux commands */
        if(strcmp(command.argv[0],"H") == 0) {
            printf("%s\n", "Usage: C file1 file2 cp file1 file2"); 
            printf("%s\n", "D file rm file ");
            printf("%s\n", "E comment ; echo comment on the screen  ");
            printf("%s\n", "H help ; ");
            printf("%s\n", "L list contents of current direcor ");
            printf("%s\n", "M file nano file; terminate with [Ctrl][X]");
            printf("%s\n", "P file display the contents of the named file on screen.");
            printf("%s\n", "Q quit the shell ");
            printf("%s\n", "S firefox");
            printf("%s\n", "W clear the screen");
            printf("%s\n", "X program execute the named program");  
        }

        /* if command.argv[0] = C , Copy files in buffer */ 
        else if(strcmp(command.argv[0],"C") == 0) {    
            /* Create a child process to execute the command */
            if ((pid = fork()) == 0) {
            /* Child executing command */
	    	  execvp("cp", command.argv);
          	}
            /* Wait for the child to terminate */
          	wait(&status);
	    }

        /* Delete a file */ 
	    else if(strcmp(command.argv[0],"D") == 0) 
		{
            if ((pid = fork()) == 0) {
                execvp("rm", command.argv);
            }
            wait(&status);
		}

        /* echo string after command */ 
		else if(strcmp(command.argv[0],"E") == 0) 
		{
            if ((pid = fork()) == 0) 
            {
                execvp("echo", command.argv);
            }
            wait(&status);
        }

        /* List the files in the directory, and the directory */ 
		else if(strcmp(command.argv[0],"L") == 0) 
		{
            if ((pid = fork()) == 0) 
            {
                printf("%s\n", "");
                execvp("pwd", command.argv);

            }
            wait(&status);            
            if ((pid = fork()) == 0) 
            {
                printf("%s\n", "");
                command.argv[1] = "-l";
                execvp("ls", command.argv);
            }
            wait(&status);
		}

        /* Open nano editor with the CL argument */ 
        else if(strcmp(command.argv[0],"M") == 0) 
		{
            if ((pid = fork()) == 0) 
            {
                /* Child executing command */
                execvp("nano", command.argv);
            }
            /* Wait for the child to terminate */
            wait(&status); 
        }

        /* Print the file to screen  */ 
        else if(strcmp(command.argv[0],"P") == 0) 
        {
            if ((pid = fork()) == 0) 
            {
                execvp("more", command.argv);
            }
            wait(&status); 
		}

        /* Quit the Terminal */ 
		else if(strcmp(command.argv[0],"Q") == 0) {

            if (pid == 0) 
            {
                execvp("exit", command.argv);
            }
            else{
                wait(&status);
                exit(0);
            }
            return 0; 
        }
        
        /* Open firefox */ 
		else if(strcmp(command.argv[0],"S") == 0) 
		{
            if ((pid = fork()) == 0) 
            {
                execvp("firefox", command.argv);
            }
            wait(&status); 
		}
        /* Clear the screen */ 
		else if(strcmp(command.argv[0],"W") == 0) 
		{
            if ((pid = fork()) == 0) 
            {
                execvp("clear", command.argv);
            }

            wait(&status); 
		}
        /* Execute the given program n */ 
		else if(strcmp(command.argv[0],"X") == 0)
		{
            if ((pid = fork()) == 0) 
            {
                execvp(command.argv[1], command.argv);
            }
            wait(&status); 
		}
        /* if not in our command list, try normal execution */ 
        else { 
            if ((pid = fork()) == 0) 
            {
                execvp(command.argv[0], command.argv);
            }
            wait(&status); 
        }
	
   }

   /* Shell termination */
   printf("\n\n shell: Terminating successfully\n");
   return 0;
}

/* End basic shell */

/* Parse Command function */

/* Determine command name and construct the parameter list.
 * This function will build argv[] and set the argc value.
 * argc is the number of "tokens" or words on the command line
 * argv[] is an array of strings (pointers to char *). The last
 * element in argv[] must be NULL. As we scan the command line
 * from the left, the first token goes in argv[0], the second in
 * argv[1], and so on. Each time we add a token to argv[],
 * we increment argc.
 */
int parseCommand(char *cLine, struct command_t *cmd) {
   int argc;
   char **clPtr;
   /* Initialization */
   clPtr = &cLine;	/* cLine is the command line */
   argc = 0;
   cmd->argv[argc] = (char *) malloc(MAX_ARG_LEN);
   /* Fill argv[] */
   while ((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
      cmd->argv[++argc] = (char *) malloc(MAX_ARG_LEN);
   }

   /* Set the command name and argc */
   cmd->argc = argc-1;
   cmd->name = (char *) malloc(sizeof(cmd->argv[0]));
   strcpy(cmd->name, cmd->argv[0]);
   return 1;
}

/* End parseCommand function */

/* Print prompt and read command functions - pp. 79-80 */

void printPrompt() {
   /* Build the prompt string to have the machine name,
    * current directory, or other desired information
    */
   char* promptString = "linux (brs80)|>";
   printf("%s ", promptString);
}

void readCommand(char *buffer) {
   /* This code uses any set of I/O functions, such as those in
    * the stdio library to read the entire command line into
    * the buffer. This implementation is greatly simplified,
    * but it does the job.
    */
   fgets(buffer, 80, stdin);
}

/* End printPrompt and readCommand */