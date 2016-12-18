//Ankita Tank
//cs361
//HW3
//Make a Shell
//Reference from the book used in CS 361 : Computer Systems, a programmers perspective



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAX 200 
#define ARGCAPACITY 600 

//Signal handler for Ctrl+C
void signal_handlerCrtlc(int sig) 
{
	//printf("at ctrlc\n");
	write(1, "SIGINT handled\n361", 66);
}

//Signal Handler for Ctrl+Z
void signal_handlerCrtlz(int sig) 
{
	//printf("ctrlz here\n");
	write(1, "SIGTSTP handled\n361> ", 66);
}



// reference from the book, page number 735
// evaluate reads and executes the commands from the shell

void evaluate(char * command_line)
{
	int file;
	char in[66];
	char out[66];
	char *argv[ARGCAPACITY];
	char buffer[MAX];
	int temp;
	int status;
	pid_t pid;
	strcpy(buffer, command_line);
	temp = parseline(buffer, argv);

	if (argv[0] == NULL)
	{
		return;
	}

	if (!ret_value(argv))
	{
		if ((pid = fork()) == 0)
		{
			int i;
			for (i = 0; argv[i] != NULL; i++)
			{
				if (strcmp(argv[i], "<") == 0)
				{
					argv[i] = '\0';
					file = open(argv[i + 1], O_RDONLY, 0);
					dup2(file, 0);
					close(file);
				}

				else if (strcmp(argv[i], ">") == 0)
				{
					argv[i] = '\0';
					file = open(argv[i + 1], O_CREAT | O_TRUNC | O_WRONLY, 0666);
					dup2(file, 1);
					close(file);
				}

				else if (strcmp(argv[i], ">>") == 0)
				{
					argv[i] = '\0';
					file = open(argv[i + 1], O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
					dup2(file, 1);
					close(file);
				}
			}

			if (execvp(argv[0], argv) < 0)
			{
				perror("execvp");
				_exit(1);
			}
		}

//		int status;
		if (wait(&status)<0)
			//changed made here
			printf("Wait foreground :: wait error");
		else if((status==0))
		{
			printf("Exit: 0\n");
		}
		printf("PID:%i status:%i\n", pid, status);
	}
	return;  //return 0; //return nothing
}

//Parsing the lines from the command line.
int  parseline(char *buffer, char **argv)
{
	char *box;
	int register_argument;
	int temp;

	buffer[strlen(buffer) - 1] = ' ';
	while (*buffer && (*buffer == ' '))
		buffer++;
	register_argument = 0;

	while ((box = strchr(buffer, ' '))) {
		argv[register_argument++] = buffer;
		*box = '\0';
		buffer = box + 1;
		while (*buffer && (*buffer == ' '))
			buffer++;
	}

	argv[register_argument] = NULL;

	if (register_argument == 0)
		return 1;
	if ((temp = (*argv[register_argument - 1] == '&')) != 0)
		argv[--register_argument] = NULL;

	return temp;
}


// Pg 735 from the book
// function to check  exit value so as to exit from the shell

int ret_value(char **argv)
{

if (!strcmp(argv[0], "&"))
		return 1;

if (!strcmp(argv[0], "exit"))
		exit(0);
 return 0;
}



int main()
{
	char command_line[MAX];
	while (1)
	{
		printf("361> ");
		signal(SIGTSTP, signal_handlerCrtlz);		// signal_handler_function handler for cltr + z
		signal(SIGINT, signal_handlerCrtlc);		// signal_handler_function haldler for cltr + c 
		fgets(command_line, MAX, stdin);
		if (feof(stdin))
			exit(0);
		evaluate(command_line);
	}
}

