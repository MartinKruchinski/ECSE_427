#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 100; // CHANGED FROM 3 TO 7

int help();
int quit();
int badcommand();
int set(char* var, char* value);
int set2(char* var, char* values[], int size); // NEW METHOD ( CHANGE NAME LATER )
int echo(char* var); // NEW METHOD
int my_ls(); // NEW METHOD
int print(char* var);
int run(char* script);
int badcommandFileDoesNotExist();
int badcommandTooManyTokens(); // NEW METHOD


// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		return badcommand();
	}

	// Check for multiple commands
	// Push commands into a file
	// Run file in batch mode
	// Delete file
	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();

	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		// printf("SET COMMAND");

		// OLD CODE
		// if (args_size != 3) return badcommand();
		// return set(command_args[1], command_args[2]);

		// NEW CODE
		if (args_size > 7) return badcommandTooManyTokens();
		return set2(command_args[1], command_args, args_size); // Here we need to pass all the params that can be included in the set, along with args size

	} else if (strcmp(command_args[0], "echo")==0) {
		if (args_size != 2) return badcommand();
		return echo(command_args[1]);

	}
	else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size != 1) return badcommand();
		return my_ls();

	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);

	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);

	} else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);
}

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

// For set command only
int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 2;
}

int set(char* var, char* value){

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
int set2(char* var, char* values[], int size){

	int i = 2;
	char *newValue = (char *)malloc(0);

	for (i; i < size - 1; i++){
		strcat(newValue, values[i]);
		strcat(newValue, " ");
	}

	strcat(newValue, values[i]); // Append last value without the space

	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, newValue);

	mem_set_value(var, newValue);

	return 0;
}

int echo(char* var){

	char firstChar = var[0];
	char* first = malloc(2*sizeof(char));
	first[0] = firstChar;
	first[1] = '\0';

	if (strcmp(first, "$") == 0) {
		if(strcmp(mem_get_value(var+1), "Variable does not exist") == 0){
			printf("\n");
		} else {
			printf("%s\n", mem_get_value(var+1));
		}
	} else {
		printf("%s\n", var);
	}

	return 0;
}

int my_ls(){

	DIR *d;
	struct dirent *dir;
	// char* list[];
    // int count = 0;

	d = opendir(".");
	if (d) {

		while ((dir = readdir(d)) != NULL){
			printf("%s\n", dir->d_name);
			// list[count] = dir->d_name;
            // count++;
		}
		closedir(d);
	}

	return 0;
}


int print(char* var){
	printf("%s\n", mem_get_value(var));
	return 0;
}

int run(char* script){
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1){
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}
