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
int fileLength(char* file);
int duplicates(char* scripts[], int size);
int badcommandDuplicateScripts();
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int my_ls();
int echo();
int exec(char* scripts[], int size);

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
		// for (int i = 0; i < args_size; i++)
		// {
		// 	printf("Arg %d is %s\n", i, command_args[i]);
		// 	if(i == 2){
		// 	}
		// }

		if (args_size > 2) return badcommand();
		return echo(command_args[1]);

	}
	else if(strcmp(command_args[0], "exec") == 0){
		if(args_size < 3) {
			return badcommand();
		}
		if(args_size > 5) {
			return badcommand();
		}
		if (duplicates(command_args, args_size)) {
			return badcommandDuplicateScripts();
		} else {
			exec(command_args, args_size);
		}
	} else {
		return badcommand();
	}
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
	char address[] = "../A2_testcases_public/";
	char* newAddress = "";
	newAddress = strcat(address, script);
	FILE *p = fopen(newAddress,"rt");  // the program is in a file
	size_t lineS = 0;
	int lineCtr = 0;

	char allCommands[10000]; //Holds all the commands in the file separated by a ;

	if(p == NULL) {
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1) {
		lineS = lineS + sizeof(line);

		if(line[strlen(line)-1] != '\n'){
			line[strlen(line)] = '\n';
			line[strlen(line)+1] = '\0';
		}

		char string = lineCtr + '0';
		char lineString[2] = "";
		strncat(lineString ,&string, 1);
		// printf("line is %s\n", line);
		set(lineString,line);
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

	// for (int k = 0; k < 20; k++) {
	// 	printf("%d here\n", allCommands[k]);
	// }

	return errCode;

}

int exec(char* scripts[], int size) {
	if(size == 3){
			return run(scripts[1]);
	}
	else if(strcmp(scripts[size-1], "FCFS") == 0){
		for (int i = 1; i < size -1; i++)
		{
			run(scripts[i]);
		}
		return 0;
	}
	else if(strcmp(scripts[size-1], "SJF") == 0){

		if (size == 4) {
			// Need to sort jobs
			int lengths[2];
			char* files[2];

			for (int i = 1; i < 3; i++)
			{
				int length = fileLength(scripts[i]);
				lengths[i - 1] = length;
				files[i - 1] = strdup(scripts[i]);
			}
			if (lengths[0] > lengths[1]) {
				// swap
				int tempLength = lengths[0];
				char* tempScript = files[0];
				lengths[0] = lengths[1];
				files[0] = files[1];
				lengths[1] = tempLength;
				files[1] = tempScript;
			}

			for (int j = 0; files[j]; j++) {
				run(files[j]);
			}
		} else {

			// Need to sort jobs
			int lengths[3];
			char* files[3];

			for (int i = 1; i < 4; i++)
			{
				int length = fileLength(scripts[i]);
				lengths[i - 1] = length;
				files[i - 1] = strdup(scripts[i]);
			}

			if (lengths[0] > lengths[2]) {
				// swap
				int tempLength = lengths[0];
				char* tempScript = files[0];
				lengths[0] = lengths[2];
				files[0] = files[2];
				lengths[2] = tempLength;
				files[2] = tempScript;
			}

			if (lengths[0] > lengths[1]) {
				// swap
				int tempLength = lengths[0];
				char* tempScript = files[0];
				lengths[0] = lengths[1];
				files[0] = files[1];
				lengths[1] = tempLength;
				files[1] = tempScript;
			}

			if (lengths[1] > lengths[2]) {
				// swap
				int tempLength = lengths[1];
				char* tempScript = files[1];
				lengths[1] = lengths[2];
				files[1] = files[2];
				lengths[2] = tempLength;
				files[2] = tempScript;
			}

			for (int j = 0; files[j]; j++) {
				run(files[j]);
			}

		}

		return 0;
	}
	else if(strcmp(scripts[size-1], "RR") == 0) {

		if (size == 3) {
			run(scripts[1]);
		} else if (size == 5) {
			char* allLines[3][1000];
			int lineCounts[3];
			int index = 0;

			for (int i = 1; i < 4; i++) {
				int errCode = 0;
				char line[1000];
				char address[] = "../A2_testcases_public/";
				char* newAddress = "";
				newAddress = strcat(address, scripts[i]);
				// printf("%s line 336\n", scripts[i]);
				FILE *p = fopen(newAddress,"rt");  // the program is in a file
				size_t lineS = 0;
				int lineCtr = 0;

				if(p == NULL) {
					return badcommandFileDoesNotExist();
				}

				fgets(line,999,p);
				while(1) {

					if(line[strlen(line)-1] != '\n'){
						line[strlen(line)] = '\n';
						line[strlen(line)+1] = '\0';
					}

					lineCtr += 1;
					// printf("%s line \n", line);
					// printf("%d index \n", index);
					// printf("%d i \n", i);
					allLines[i-1][lineCtr-1] = strdup(line);

					if(feof(p)) {
						break;
					}

					index += 1;

					fgets(line,999,p);
				}
				lineCounts[i - 1] = lineCtr-1;
			}

			int offset = 0;
			int prog1Done = 1;
			int prog2Done = 1;
			int prog3Done = 1;

			// printf("%s \n", allLines[1][0]);

			while ((prog1Done || prog2Done || prog3Done)) {

				for (int i = 0; i < 3; i++) {
					for (int x = 0; x < 2; x++) {
						// printf("%s \n", allLines[0][x]);
						// printf("%d i \n", i);
						// printf("%d x \n", x);
						// printf("%d x + offset \n", x + offset);
						// printf("%d lineCounts \n", lineCounts[i]);

						if (x + offset >= lineCounts[i]) {

							if (i == 0) {
								prog1Done = 0;
							} else if (i == 1) {
								prog2Done = 0;
							} else if (i == 2) {
								prog3Done = 0;
							}
							// printf("%d 1\n", prog1Done);
							// printf("%d 2\n", prog2Done);
							// printf("%d 3\n", prog3Done);
						} else {
							parseInput(allLines[i][x + offset]);
						}
					}
				}
				offset += 2;
			}
		} else {
			char* allLines[2][1000];
			int lineCounts[2];
			int index = 0;

			for (int i = 1; i < 3; i++) {
				int errCode = 0;
				char line[1000];
				char address[] = "../A2_testcases_public/";
				char* newAddress = "";
				newAddress = strcat(address, scripts[i]);
				// printf("%s line 336\n", scripts[i]);
				FILE *p = fopen(newAddress,"rt");  // the program is in a file
				size_t lineS = 0;
				int lineCtr = 0;

				if(p == NULL) {
					return badcommandFileDoesNotExist();
				}

				fgets(line,999,p);
				while(1) {

					if(line[strlen(line)-1] != '\n'){
						line[strlen(line)] = '\n';
						line[strlen(line)+1] = '\0';
					}

					lineCtr += 1;
					// printf("%s line \n", line);
					// printf("%d index \n", index);
					// printf("%d i \n", i);
					allLines[i-1][lineCtr-1] = strdup(line);

					if(feof(p)) {
						break;
					}

					index += 1;

					fgets(line,999,p);
				}
				lineCounts[i - 1] = lineCtr-1;
			}

			int offset = 0;
			int prog1Done = 1;
			int prog2Done = 1;

			// printf("%s \n", allLines[1][0]);

			while ((prog1Done || prog2Done )) {

				for (int i = 0; i < 2; i++) {
					for (int x = 0; x < 2; x++) {
						// printf("%s \n", allLines[0][x]);
						// printf("%d i \n", i);
						// printf("%d x \n", x);
						// printf("%d x + offset \n", x + offset);
						// printf("%d lineCounts \n", lineCounts[i]);

						if (x + offset >= lineCounts[i]) {

							if (i == 0) {
								prog1Done = 0;
							} else if (i == 1) {
								prog2Done = 0;
							}
							// printf("%d 1\n", prog1Done);
							// printf("%d 2\n", prog2Done);
							// printf("%d 3\n", prog3Done);
						} else {
							parseInput(allLines[i][x + offset]);
						}
					}
				}
				offset += 2;
			}
		}
	}
	else if(strcmp(scripts[size-1], "AGING") == 0){

		if (size == 3) {
			run(scripts[1]);
		} else if (size == 5) {
			char* allLines[3][1000];
			int age[3];
			int lengths[3];
			int curCmd[3] = {0, 0, 0};
			int index = 0;

			for (int i = 1; i < 4; i++) {
				int errCode = 0;
				char line[1000];
				char address[] = "../A2_testcases_public/";
				char* newAddress = "";
				newAddress = strcat(address, scripts[i]);
				// printf("%s line 336\n", scripts[i]);
				FILE *p = fopen(newAddress,"rt");  // the program is in a file
				size_t lineS = 0;
				int lineCtr = 0;

				if(p == NULL) {
					return badcommandFileDoesNotExist();
				}

				fgets(line,999,p);
				while(1) {

					if(line[strlen(line)-1] != '\n'){
						line[strlen(line)] = '\n';
						line[strlen(line)+1] = '\0';
					}

					lineCtr += 1;
					// printf("%s line \n", line);
					// printf("%d index \n", index);
					// printf("%d i \n", i);
					allLines[i-1][lineCtr-1] = strdup(line);

					if(feof(p)) {
						break;
					}

					index += 1;

					fgets(line,999,p);
				}
				age[i - 1] = lineCtr-1;
				lengths[i - 1] = lineCtr-1;
			}

			int progIndexes[] = {0, 1, 2};

			// Sort commands

			if (age[0] > age[2]) {
				// Swap
				int tempAge = age[2];
				int tempLength = lengths[2];
				int tempIndex = progIndexes[2];
				int tempCurCmd = curCmd[2];
				age[2] = age[0];
				lengths[2] = lengths[0];
				progIndexes[2] = progIndexes[0];
				curCmd[2] = curCmd[0];
				age[0] = tempAge;
				lengths[0] = tempLength;
				progIndexes[0] = tempIndex;
				curCmd[0] = tempCurCmd;
			}

			if (age[0] > age[1]) {
				// Swap
				int tempAge = age[1];
				int tempLength = lengths[1];
				int tempIndex = progIndexes[1];
				int tempCurCmd = curCmd[1];
				age[1] = age[0];
				lengths[1] = lengths[0];
				progIndexes[1] = progIndexes[0];
				curCmd[1] = curCmd[0];
				age[0] = tempAge;
				lengths[0] = tempLength;
				progIndexes[0] = tempIndex;
				curCmd[0] = tempCurCmd;
			}

			if (age[1] > age[2]) {
				// Swap
				int tempAge = age[1];
				int tempLength = lengths[1];
				int tempIndex = progIndexes[1];
				int tempCurCmd = curCmd[1];
				age[1] = age[2];
				lengths[1] = lengths[2];
				progIndexes[1] = progIndexes[2];
				curCmd[1] = curCmd[2];
				age[2] = tempAge;
				lengths[2] = tempLength;
				progIndexes[2] = tempIndex;
				curCmd[2] = tempCurCmd;
			}

			// printf("index\n");
			// for (int i = 0; i < 3; i++) {
			// 	printf("%d ", progIndexes[i]);
			// }

			// printf("\n cmd\n");
			// for (int i = 0; i < 3; i++) {
			// 	printf("%d ", curCmd[i]);
			// }

			// printf("\n lengths\n");
			// for (int i = 0; i < 3; i++) {
			// 	printf("%d ", lengths[i]);
			// }

			// printf("\n age \n");

			// for (int i = 0; i < 3; i++) {
			// 	printf("%d ", age[i]);
			// }
			// printf("\n\n");

			int progDone[3] = {1, 1, 1};

			// // printf("%s \n", allLines[1][0]);
			// // Shortest command should be at index 0 right now
			while ((progDone[0] || progDone[1] || progDone[2])) {

				// printf("\nindex ");
				// for (int i = 0; i < 3; i++) {
				// 	printf("%d ", progIndexes[i]);
				// }

				// printf("\n cmd\n");
				// for (int i = 0; i < 3; i++) {
				// 	printf("%d ", curCmd[i]);
				// }

				// printf("\n lengths\n");
				// for (int i = 0; i < 3; i++) {
				// 	printf("%d ", lengths[i]);
				// }

				// printf("\n age  ");
				// for (int i = 0; i < 3; i++) {
				// 	printf("%d ", age[i]);
				// }
				// printf("\n");

				if (curCmd[0] == lengths[0]) {
					progDone[0] = 0;
					age[0] = 10000;
				} else {
					// Run first command
					// printf("%s", allLines[progIndexes[0]][curCmd[0]]);
					parseInput(allLines[progIndexes[0]][curCmd[0]]);
					// Increment curCmd and decrement age values
					curCmd[0] += 1;
					if (curCmd[0] == lengths[0]) {
						progDone[0] = 0;
						age[0] = 10000;
					}
					if (age[1] != 0) {
						age[1] -= 1;
					}

					if (age[2] != 0) {
						age[2] -= 1;
					}
				}
				// printf("\n");
				// sort programs

				if (age[0] > age[1]) {
					// swap 0 and 1
					int tempAge = age[0];
					int tempIndex = progIndexes[0];
					int tempLength = lengths[0];
					int tempCur = curCmd[0];
					int tempDone = progDone[0];
					age[0] = age[1];
					curCmd[0] = curCmd[1];
					progIndexes[0] = progIndexes[1];
					lengths[0] = lengths[1];
					progDone[0] = progDone[1];
					age[1] = tempAge;
					curCmd[1] = tempCur;
					progIndexes[1] = tempIndex;
					lengths[1] = tempLength;
					progDone[1] = tempDone;

					// swap 1 and 2
					tempAge = age[1];
					tempIndex = progIndexes[1];
					tempCur = curCmd[1];
					tempLength = lengths[1];
					tempDone = progDone[1];
					age[1] = age[2];
					curCmd[1] = curCmd[2];
					progIndexes[1] = progIndexes[2];
					lengths[1] = lengths[2];
					progDone[1] = progDone[2];
					age[2] = tempAge;
					curCmd[2] = tempCur;
					progIndexes[2] = tempIndex;
					lengths[2] = tempLength;
					progDone[2] = tempDone;

				}

				if (age[0] > age[1]) {
					// swap
					int tempAge = age[0];
					int tempIndex = progIndexes[0];
					int tempLength = lengths[0];
					int tempCur = curCmd[0];
					int tempDone = progDone[0];
					age[0] = age[1];
					curCmd[0] = curCmd[1];
					progIndexes[0] = progIndexes[1];
					lengths[0] = lengths[1];
					progDone[0] = progDone[1];
					age[1] = tempAge;
					curCmd[1] = tempCur;
					progIndexes[1] = tempIndex;
					lengths[1] = tempLength;
					progDone[1] = tempDone;
				}

				if (age[0] > age[2]) {
					// swap
					int tempAge = age[0];
					int tempLength = lengths[0];
					int tempIndex = progIndexes[0];
					int tempCur = curCmd[0];
					int tempDone = progDone[0];
					age[0] = age[2];
					progIndexes[0] = progIndexes[2];
					curCmd[0] = curCmd[2];
					lengths[0] = lengths[2];
					progDone[0] = progDone[2];
					age[2] = tempAge;
					progIndexes[2] = tempIndex;
					curCmd[2] = tempCur;
					lengths[2]= tempLength;
					progDone[2] = tempDone;
				}


				if (age[1] >= age[2]) {
					// swap
					int tempAge = age[1];
					int tempIndex = progIndexes[1];
					int tempCur = curCmd[1];
					int tempLength = lengths[1];
					int tempDone = progDone[1];
					age[1] = age[2];
					curCmd[1] = curCmd[2];
					progIndexes[1] = progIndexes[2];
					lengths[1] = lengths[2];
					progDone[1] = progDone[2];
					age[2] = tempAge;
					curCmd[2] = tempCur;
					progIndexes[2] = tempIndex;
					lengths[2] = tempLength;
					progDone[2] = tempDone;
				}

			}
		} else {

			char* allLines[2][1000];
			int age[2];
			int lengths[2];
			int curCmd[2] = {0, 0};
			int index = 0;

			for (int i = 1; i < 3; i++) {
				int errCode = 0;
				char line[1000];
				char address[] = "../A2_testcases_public/";
				char* newAddress = "";
				newAddress = strcat(address, scripts[i]);
				// printf("%s line 336\n", scripts[i]);
				FILE *p = fopen(newAddress,"rt");  // the program is in a file
				size_t lineS = 0;
				int lineCtr = 0;

				if(p == NULL) {
					return badcommandFileDoesNotExist();
				}

				fgets(line,999,p);
				while(1) {

					if(line[strlen(line)-1] != '\n'){
						line[strlen(line)] = '\n';
						line[strlen(line)+1] = '\0';
					}

					lineCtr += 1;
					// printf("%s line \n", line);
					// printf("%d index \n", index);
					// printf("%d i \n", i);
					allLines[i-1][lineCtr-1] = strdup(line);

					if(feof(p)) {
						break;
					}

					index += 1;

					fgets(line,999,p);
				}
				age[i - 1] = lineCtr-1;
				lengths[i - 1] = lineCtr-1;
			}

			int progIndexes[] = {0, 1};

			// Sort commands

			if (age[0] > age[1]) {
				// Swap
				int tempAge = age[1];
				int tempLength = lengths[1];
				int tempIndex = progIndexes[1];
				int tempCurCmd = curCmd[1];
				age[1] = age[0];
				lengths[1] = lengths[0];
				progIndexes[1] = progIndexes[0];
				curCmd[1] = curCmd[0];
				age[0] = tempAge;
				lengths[0] = tempLength;
				progIndexes[0] = tempIndex;
				curCmd[0] = tempCurCmd;
			}

			// printf("index\n");
			// for (int i = 0; i < 3; i++) {
			// 	printf("%d ", progIndexes[i]);
			// }

			// printf("\n cmd\n");
			// for (int i = 0; i < 3; i++) {
			// 	printf("%d ", curCmd[i]);
			// }

			// printf("\n lengths\n");
			// for (int i = 0; i < 3; i++) {
			// 	printf("%d ", lengths[i]);
			// }

			// printf("\n age \n");

			// for (int i = 0; i < 3; i++) {
			// 	printf("%d ", age[i]);
			// }
			// printf("\n\n");

			int progDone[3] = {1, 1};

			// // printf("%s \n", allLines[1][0]);
			// // Shortest command should be at index 0 right now
			while (progDone[0] || progDone[1]) {

				// printf("\nindex ");
				// for (int i = 0; i < 3; i++) {
				// 	printf("%d ", progIndexes[i]);
				// }

				// printf("\n cmd\n");
				// for (int i = 0; i < 3; i++) {
				// 	printf("%d ", curCmd[i]);
				// }

				// printf("\n lengths\n");
				// for (int i = 0; i < 3; i++) {
				// 	printf("%d ", lengths[i]);
				// }

				// printf("\n age  ");
				// for (int i = 0; i < 3; i++) {
				// 	printf("%d ", age[i]);
				// }
				// printf("\n");

				if (curCmd[0] == lengths[0]) {
					progDone[0] = 0;
					age[0] = 10000;
				} else {
					// Run first command
					// printf("%s", allLines[progIndexes[0]][curCmd[0]]);
					parseInput(allLines[progIndexes[0]][curCmd[0]]);
					// Increment curCmd and decrement age values
					curCmd[0] += 1;
					if (curCmd[0] == lengths[0]) {
						progDone[0] = 0;
						age[0] = 10000;
					}
					if (age[1] != 0) {
						age[1] -= 1;
					}

					if (age[2] != 0) {
						age[2] -= 1;
					}
				}
				// printf("\n");
				// sort programs

				if (age[0] > age[1]) {
					// swap 0 and 1
					int tempAge = age[0];
					int tempIndex = progIndexes[0];
					int tempLength = lengths[0];
					int tempCur = curCmd[0];
					int tempDone = progDone[0];
					age[0] = age[1];
					curCmd[0] = curCmd[1];
					progIndexes[0] = progIndexes[1];
					lengths[0] = lengths[1];
					progDone[0] = progDone[1];
					age[1] = tempAge;
					curCmd[1] = tempCur;
					progIndexes[1] = tempIndex;
					lengths[1] = tempLength;
					progDone[1] = tempDone;

				}

			}
		}
	}
	else {
		return  badcommand();
	}
}

int duplicates(char* scripts[], int size) {

	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			if (!strcmp(scripts[i], scripts[j])) {
				return 1;
			}
		}
	}
	return 0;
}

int fileLength(char* file) {
	int errCode = 0;
	char line[1000];
	char address[] = "../A2_testcases_public/";
	char* newAddress = "";
	newAddress = strcat(address, file);
	FILE *p = fopen(newAddress,"rt");  // the program is in a file
	size_t lineS = 0;
	int lineCtr = 0;

	if(p == NULL) {
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1) {

		if(line[strlen(line)-1] != '\n'){
			line[strlen(line)] = '\n';
			line[strlen(line)+1] = '\0';
		}

		lineCtr += 1;
		// printf("%s \n", line);
		if(feof(p)) {
			break;
		}
		fgets(line,999,p);
	}

	return lineCtr;
}

int badcommandDuplicateScripts(){
	printf("%s\n", "Bad command: Duplicate scripts found");
	return 3;
}
