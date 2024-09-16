/****************
LE2: Introduction to Unnamed Pipes
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close
#include <iostream>
using namespace std;

int main () {
    // lists all the files in the root directory in the long format
    char* cmd1[] = {(char*) "ls", (char*) "-al", (char*) "/", nullptr};
    // translates all input from lowercase to uppercase
    char* cmd2[] = {(char*) "tr", (char*) "a-z", (char*) "A-Z", nullptr};

    // TODO: add functionality
    // Create pipe

	int p[2];

	if (pipe(p) < 0){
		perror("pipe failed");
			return 1;
	}

	// fork to child run writing stdout from command if fork is succesful

	pid_t c1 = fork();
	if (c1 < 0){
		perror("fork failed");
		return 1;
	}
	
	if (c1 == 0) {

		dup2(p[1], STDOUT_FILENO);
		close(p[0]);
		close(p[1]);

		execvp(cmd1[0], cmd1);
		perror("execvp failed");
		
	}
	// fork another child run reading stdout from command to write into p[0] to pipe contents from first command into second command
	pid_t c2 = fork();
	if (c2 < 0){
		perror("fork failed");
		return 1;
	}

	if (c2 == 0) {

		dup2(p[0], STDIN_FILENO);
		close(p[1]);
		close(p[0]);

		execvp(cmd2[0], cmd2);
		perror("execvp failed");
		
	}

	close(p[0]);
	close(p[1]);
	return 0;
    // Create child to run first command
    // In child, redirect output to write end of pipe
    // Close the read end of the pipe on the child side.
    // In child, execute the command




    // Create another child to run second command
    // In child, redirect input to the read end of the pipe
    // Close the write end of the pipe on the child side.
    // Execute the second command.
	//


    // Reset the input and output file descriptors of the parent.
}
