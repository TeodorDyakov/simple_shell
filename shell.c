#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<sys/wait.h>

#define LEN 32

char buf[LEN];

int main(int argc, char** argv){
	int fd[2];
	pipe(fd);

	int pid = fork();

	if(pid == 0){
		close(fd[0]);
		dup2(fd[1], 1);
		execlp("cat", "cat", "words", NULL);
	}

	int fd1[2];
	pipe(fd1);

	pid = fork();

	if(pid == 0){
		close(fd[1]);
		dup2(fd[0], 0);
		close(fd1[0]);
		dup2(fd1[1], 1);
		execlp("wc", "wc", NULL);
	}
	
	close(fd[0]);
	close(fd[1]);

	pid = fork();

	if(pid == 0){
		close(fd1[1]);
		dup2(fd1[0], 0);
		execlp("wc", "wc", NULL);
	}

	close(fd1[1]);
	close(fd1[0]);
	wait(NULL);
	wait(NULL);
	wait(NULL);
}