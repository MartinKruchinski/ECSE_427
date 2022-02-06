#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 100; // CHANGED FROM 3 TO 100

int help();
int quit();
int badcommand();
int set(char* var, char* value);
int set2(char* var, char* value); // NEW METHOD ( CHANGE NAME LATER )
int echo(char* var); // NEW METHOD
int my_ls(); // NEW METHOD
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();
int badcommandTooManyTokens(); // NEW METHOD
int tooManyInstructions();
int invalidCommand();
int endofthefile();

// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size) {
	int i;
	int nCommandWords = 0;
	int nCommands = 0;

	char * inputArray[10][10] = {0};


	if (args_size > MAX_ARGS_SIZE) {
		return badcommand();
	}
	if(args_size < 1){
		return endofthefile();
	}

	for(int k=0; k < args_size; k++){
		if(nCommands == 10){
			return tooManyInstructions();
		}
		if(strcmp(command_args[k], ";") == 0){
			nCommands++;
			nCommandWords = 0;
		}
		else if(strchr(command_args[k], ';') != NULL){
			char tempString[strlen(command_args[k])];
			command_args[k][strlen(command_args[k]) -1] = '\0';
			strcpy(tempString, command_args[k]);
			int sizeStr = strlen(command_args[k]);
			inputArray[nCommands][nCommandWords] = command_args[k];
			nCommands++;
			nCommandWords = 0;
		}
		else {
			inputArray[nCommands][nCommandWords] = command_args[k];
			nCommandWords++;
		}

	}

	int a = 0;
	for ( i=0; i<args_size; i++) { //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}
	while(inputArray[a][0] != NULL){
		 if (strcmp(inputArray[a][0], "help")==0) {
			//help
			if (inputArray[a][1] != NULL) return badcommand();
			a++;
			help();

		} else if (strcmp(inputArray[a][0], "quit")==0) {
			//quit
			if (inputArray[a][1] != NULL) return badcommand();
			a++;
			quit();

		} else if (strcmp(inputArray[a][0], "set")==0) {

			// NEW CODE
			if (inputArray[a][7] != NULL) return badcommandTooManyTokens();

			int i = 2;
			char *newValue = (char *)malloc(0);

			for (i; inputArray[a][i]; i++) {
				strcat(newValue, inputArray[a][i]);
				strcat(newValue, " ");
			}

			set2(inputArray[a][1], newValue); // Here we need to pass all the params that can be included in the set, along with args size
			a++;

		} else if (strcmp(inputArray[a][0], "echo")==0) {
			if (inputArray[a][2] != NULL) return invalidCommand();
			if(inputArray[a][1] == NULL || strcmp(inputArray[a][1], " ")==0) return invalidCommand();
			echo(inputArray[a][1]);
			a++;

		}
		else if (strcmp(inputArray[a][0], "my_ls")==0) {
			if (inputArray[a][1] != NULL) return badcommand();
			my_ls();
			a++;

		} else if (strcmp(inputArray[a][0], "print")==0) {
			if (inputArray[a][2] != NULL) return badcommand();
			print(inputArray[a][1]);
			a++;

		} else if (strcmp(inputArray[a][0], "run")==0) {
			if (inputArray[a][2] != NULL) return badcommand();
			run(inputArray[a][1]);
			a++;

		
		} else return badcommand();
	}
}

int help() {

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit() {
	printf("%s\n", "Bye!");
	exit(0);
}

int badcommand() {
	printf("%s\n", "Unknown Command");
	return 1;
}

int endofthefile(){
	freopen("/dev/tty", "r", stdin);
	return 0;
}

// For run command only
int badcommandFileDoesNotExist() {
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int tooManyInstructions(){
	printf("%s\n", "Too many chained instructions");
	return 1;
}

// For set command only
int badcommandTooManyTokens() {
	printf("%s\n", "Bad command: Too many tokens");
	return 2;
}

int invalidCommand(){
	printf("%s\n", "Invalid command.");
	return 4;
}

int set(char* var, char* value) {

	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;

}

/**
	This method is used for when there are multiple words for a variable
	$ set x 20 bob alice toto xyz
	$ print x
	20 bob alice toto xyz
	$ set x 12345 20 bob alice toto xyz
	Bad command: Too many tokens
	$ print x
 	20 bob alice toto xyz
**/
int set2(char* var, char* value) {

	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;
}

int echo(char* var) {

	char firstChar = var[0];
	char* first = malloc(2*sizeof(char));
	first[0] = firstChar;
	first[1] = '\0';

	if (strcmp(first, "$") == 0) {
		if(strcmp(mem_get_value(var+1), "Variable does not exist") == 0) {
			printf("\n");
		} else {
			printf("%s\n", mem_get_value(var+1));
		}
	} else {
		printf("%s\n", var);
	}

	return 0;
}

int my_ls() {

	DIR *d;
	struct dirent *dir;

	char *list[100];
	char *tmp;

	int i;
	int count = 0;

	d = opendir(".");
	if (d) {

		while ((dir = readdir(d)) != NULL) {

			list[count] = dir->d_name;
			count++;
		}

		for (i=0; list[i]; i++) {
			for (int j=0; list[j]; j++) {
				if (strcmp(list[i], list[j]) < 0) {
					tmp = list[i];
					list[i] = list[j];
					list[j] = tmp;
				}
			}
		}
		closedir(d);

		for (i = 0; i < count; i++) {
			printf("%s\n", list[i]);
		}
	}

	return 0;
}


int print(char* var) {
	printf("%s\n", mem_get_value(var));
	return 0;
}

int run(char* script) {
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL) {
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1) {
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)) {
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}
