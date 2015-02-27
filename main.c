#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

// 02/26/2015, Aaron Woodrow Sedlacek//

char ** getpaths() {
	char * mypath = getenv("MYPATH");

	if(mypath == NULL) {
		printf("env variable $MYPATH is empty.\n");
		exit(1);
	}
	int i;
	int count;

	for(i = 0, count = 1; mypath[i]; i++) {

		if(mypath[i] == '#') {
			count++;
		}
	}

	char ** paths = (char **) malloc(count);

	char * token = strtok(mypath, "#");
	paths[0] = token;
	i = 1;
	while(token != NULL) {
		token = strtok(NULL, "#");
		paths[i] = token;
		i++;
	}
	return paths;
}

int run_command(char * cmd, char ** args) {

	int child_pid = fork();
	//printf("pid: %d\n", child_pid);
	//printf("cmd: %s\n", cmd);
	int i = 0;
	while(args[i] != NULL) {
		//printf("arg %d --> %s\n",i,args[i]);
		i++;
	}

	if(child_pid != 0) {
		return child_pid;
	}
	else {
		execv(cmd,args);
		printf("An error occured in execv\n");
	}
	return 1;
}

int main() {

	while(1) {
		char input[1001];
		printf("/>");
		//ensures string is null terminated, to prevent buffer overflows
		fgets(input,1001,stdin);
		//count the number of arguments in the input
		int i, count;
		for(i = 0, count = 0; input[i]; i++) {
			if(input[i] == ' ') {
				count++;
			}
		}
		//remove the newline char
		char * temp = strtok(input,"\n");
		if (temp == NULL) {
			continue;
		}
		char * token = strtok(temp," ");
		//get the command
		char * cmd = token;

		//allocate memory to store arguments
		char ** args = (char**) malloc(count);
		//put arguments into args variable
		i = 0;
		while(token != NULL) {
			token = strtok(NULL, " ");
			args[i] = token;
			i++;
		}
		int error = 0;
		//if cd command
		if(strcmp(cmd,"cd") == 0) {

			if (args[0] == NULL) {

				error = chdir(getenv("HOME"));
			}
			else {
				error = chdir(args[0]);
			}
			if(error == -1) {
				perror("chdir");
			}	
		}
		else if (strcmp(cmd,"exit") == 0) {
			exit(0);
		}
		else {
			char ** paths = getpaths();

			i = 0;
			while(paths[i] != NULL) {
				char * foo = paths[i];
				strcat(foo,"/");
				strcat(foo,cmd);
				struct stat buf;
				if(stat(foo,&buf) == 0) {
					//printf("can access %s\n",foo);
					run_command(foo,args);
					break;
				}
				i++;
			}
		}
	}
	return 0;
}