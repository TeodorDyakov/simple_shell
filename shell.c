#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<sys/wait.h>

struct cmd{
	char* cmd;
	char* argv[32];
};

struct cmd cmds[32];

/*
This is a simple shell interpreter that supports the pipe operator
Example:
gcc shell.c -o shell

./shell "ls -l | wc"
*/

int main(int argc, char** argv){
	int idx = 0;
	char* pch = strtok(argv[1] ," ");
	
	if(argc  < 1){
		printf("not enough arguments");
		exit(-1);
	}

	while(pch != NULL){
		if (strcmp(pch, "|") == 0){
			pch = strtok(NULL, " ");
		}
		cmds[idx].cmd = pch;
		int argIdx = 0;	
		
		while(pch != NULL && (strcmp(pch, "|") != 0)){
			cmds[idx].argv[argIdx] = pch;
			argIdx++;
			pch = strtok(NULL ," ");
		}
		cmds[idx].argv[argIdx] = NULL;
		idx++;
	}

	//the first cmd reads from STDIN
	int prevInFd = 0;

	for(int i = 0; i < idx; i++){
		char path[32] = "/bin/";
		strcat(path, cmds[i].cmd);

		int fd[2];
		pipe(fd);

		int cpid = fork();

		if(cpid == -1){
			perror("could not fork");
			exit(-1);
		}
		if(cpid == 0){

			dup2(prevInFd, 0);			

			//redirect stdout to write end of pipe if this is not the last command 
			if(i != idx -1){
				dup2(fd[1], 1);
			}
			execv(path, cmds[i].argv);			
		}

		if(i != 0){
			close(prevInFd);		
		}

		prevInFd = fd[0];
		close(fd[1]);
	}

	close(prevInFd);

	for(int i = 0; i < idx; i++){
		wait(NULL);
	}

}
