#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 7;
int UNIQUE_PID = 0;

int help();
int quit();
int badcommand();
int badcommandTooManyTokens();
int badcommandFileDoesNotExist();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int my_ls();
int echo();

int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		if (strcmp(command_args[0], "set")==0 && args_size > MAX_ARGS_SIZE) {
			return badcommandTooManyTokens();
		}
		return badcommand();
	}

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
		//set
		if (args_size < 3) return badcommand();
		char* value = (char*)calloc(1,150);
		char spaceChar = ' ';

		for(int i = 2; i < args_size; i++){
			strncat(value, command_args[i], 30);
			if(i < args_size-1){
				strncat(value, &spaceChar, 1);
			}
		}
		return set(command_args[1], value);
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size > 2) return badcommand();
		return my_ls();
	
	}else if (strcmp(command_args[0], "echo")==0) {
		if (args_size > 2) return badcommand();
		return echo(command_args[1]);
	
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

int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 2;
}

int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
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

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}


int my_ls(){
	int errCode = system("ls | sort");
	return errCode;
}

int echo(char* var){
	if(var[0] == '$'){
		var++;
		printf("%s\n", mem_get_value(var)); 
	}else{
		printf("%s\n", var); 
	}
	return 0; 
}

struct memoryLocation{
	int position;
	int length;
};

typedef struct pcb {
	int PID; //has to be unique
	struct memoryLocation location; //start position and length of script
	int currentInstruction;
	struct pcb *next;
} pcb_t;




int run(char* script) {
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file
	size_t lineS = 0;
	int lineCtr = 0;

	char allCommands[10000]; //Holds all the commands in the file separated by a ;

	if(p == NULL) {
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1) {
		lineS = lineS + sizeof(line);
		// errCode = parseInput(line);	// which calls interpreter()
		char string = lineCtr + '0';
		char lineString[2] = "";
		strncat(lineString ,&string, 1);
		set(lineString, line);
		lineCtr += 1;
		if(feof(p)) {
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	char index = 0 + '0';
	char startingPoint[2] = "";
	strncat(startingPoint ,&index, 1);
	int pos = mem_get_location(startingPoint);
	struct memoryLocation memloc = {
		.position = pos,
		.length = lineCtr
	};
	UNIQUE_PID = UNIQUE_PID + 1;

	pcb_t *head = NULL;
	head = malloc(sizeof(pcb_t));
	head->PID = UNIQUE_PID;
	head->location = memloc;
	head->next = NULL;
	int m = 0;
	// mem_free_space(0,6);
	while(head != NULL){
		for (int i = head->location.position; i < head->location.length; i++)
		{	
			m++;
			char in = i + '0';
			char stPoint[2] = "";
			strncat(stPoint ,&in, 1);
			parseInput(mem_get_value(stPoint));
			head->currentInstruction += 1;
		}
		mem_free_space(head->location.position,head->location.length);
		head = head->next;
	}

	return errCode;
		
}
