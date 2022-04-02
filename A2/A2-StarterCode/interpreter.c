#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shellmemory.h"
#include "shell.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))


#ifndef VARMEMSIZE
#define FRAMESIZE
#endif

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
int run2(char* script);
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

		if (args_size > 3) {
			return badcommand();
		}
		return echo(command_args[1]);

	}
	else if(strcmp(command_args[0], "exec") == 0){
		if(args_size < 3) {
			return badcommand();
		}
		if(args_size > 5) {
			return badcommand();
		}
		// if (duplicates(command_args, args_size)) {
		// 	return badcommandDuplicateScripts();
		// }
		else {
			exec(command_args, args_size);
		}
	} else if (strcmp(command_args[0], "resetmem")==0){
	    //help
	    if (args_size != 1) return badcommand();
		printf("Memory was Reset\n");
		mem_free_space(0, 1000);

	    return 0;

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
	char* allLines[1][1000];
	int lineCounts[1];
	int index = 0;

	for (int i = 1; i < 2; i++) {
		int errCode = 0;
		char line[1000];
		char address[] = "";
		char* newAddress = "";
		newAddress = strcat(address, script);
		FILE *p = fopen(newAddress,"rt");  // the program is in a file
		size_t lineS = 0;
		int lineCtr = 0;

		if(p == NULL) {
			return badcommandFileDoesNotExist();
		}

		fgets(line,999,p);
		while(1) {

			if(line[strlen(line)-1] != '\n'){
				line[strlen(line)+1] = '\0';
			}

			lineCtr += 1;
			allLines[i-1][lineCtr-1] = strdup(line);

			if(feof(p)) {
				break;
			}

			index += 1;

			fgets(line,999,p);
		}
		lineCounts[i - 1] = lineCtr;
	}
	// files loaded into allLines

	// load into directoryStore
	char* directoryStore[1000];

	for (int i = 0; i < 1000; i++) {
		directoryStore[i] = "none";
	}

	int offset = 0;
	int curProg = 0;
	// int i = 0;
	int maxLines = lineCounts[0];
	int sum = lineCounts[0];

	// printf("%d sum \n", sum);

	for (int i = 0; i < sum/2 + 1; i++) {
		if (offset < maxLines) {
			while (offset >= lineCounts[curProg]) {
				curProg = (curProg + 1) % 1;
			}
		}
		for (int j = 0 ; j < 3; j++ ) {
			if (j + offset < lineCounts[curProg]) {
				directoryStore[i * 3 + j] = allLines[curProg][j + offset];
			}
		}
		if (curProg + 1 == 1) {
			offset += 3;
		}
		curProg = (curProg + 1) % 1;
	}


	// for (int i = 0; i < 50; i++) {
	// 	printf("%s \n", directoryStore[i]);
	// }

	// loaded into frame

	// for (int i = 0; i < 6; i++) {
	// 	for (int j = 0; j < 2; j++) {
	// 		printf("%s \n", frameStore[3 * i + j]);
	// 	}
	// }

	// return 0;

	offset = 0;

	int frameIndex = 0;
	char* backingStore[1000];

	for (int i = 0; i < 1000; i++) {
		backingStore[i] = "none";
	}

	int prog1Done = 1;
	// int prog2Done = 1;
	// int prog3Done = 1;

	int curSetting;

	// load into backing store as RR
	while (prog1Done) {

		for (int i = 0; i < 1; i++) {
			for (int x = 0; x < 2; x++) {
				// printf("%d lineCont \n", lineCounts[i]);
				// printf("%d i\n", i);

				// printf("%d i\n", i);
				// printf("%d x\n", x);
				// printf("%d offset\n", offset);
				if (i == 0 && prog1Done == 0) {

				}
				else if (x + offset >= lineCounts[i]) {

					if (i == 0) {
						// printf("prog1Done\n");
						prog1Done = 0;
					}
				} else {
					// printf("%d \n", x);
					// printf("%s \n", allLines[i][x + offset]);
					backingStore[frameIndex] = strdup(allLines[i][x + offset]);
					// parseInput(allLines[i][x + offset]);
				}
				// printf("---------\n");

			frameIndex++;

			}
		}
		offset += 2;
	}

	// for (int i = 0; i < 50; i++) {
	// 	printf("%s \n", backingStore[i]);
	// }

	// return 0;

	// load into frame store

	char* frameStore[FRAMESIZE];

	int minFrame = MIN(FRAMESIZE, 18);

	for (int i = 0; i < FRAMESIZE; i++) {
		frameStore[i] = "free";
	}

	for (int i = 0; i < minFrame; i++) {
		frameStore[i] = strdup(directoryStore[i]);
	}


	// Start runnin commands
	frameIndex = minFrame % FRAMESIZE;
	int directoryIndex = minFrame;
	int i = 0;
	while (i < 500) {
		// search frameStore for command
		if (strcmp(backingStore[i], "none") != 0) {
			int found = 0;
			for (int j = 0; j < FRAMESIZE; j++) {
				if (strcmp(frameStore[j], backingStore[i]) == 0) {
					found = 1;
				}
			}

			if (found) {
				parseInput(backingStore[i]);
			} else {

				if (strcmp(frameStore[frameIndex], "free") == 0) {
					printf("%s command", backingStore[i]);
					char* temp = strdup(directoryStore[directoryIndex]);
					frameStore[frameIndex] = temp;

					temp = strdup(directoryStore[directoryIndex + 1]);
					frameStore[frameIndex + 1] = temp;

					temp = strdup(directoryStore[directoryIndex + 2]);
					frameStore[frameIndex + 2] = temp;

					frameIndex = (frameIndex + 3) % FRAMESIZE;
					directoryIndex += 3;

					// move commands around in the backingStore
					// even num means its the first command of 2
					if (i % 2 == 0){
						char* temp = strdup(backingStore[i]);
						char* temp2 = strdup(backingStore[i + 1]);

						backingStore[i] = backingStore[i + 2];
						backingStore[i + 1] = backingStore[i + 3];

						backingStore[i + 2] = backingStore[i + 4];
						backingStore[i + 3] = backingStore[i + 5];

						backingStore[i + 4] = temp;
						backingStore[i + 5] = temp2;

						i--;
					} else { // odd means that the second command in RR failed, need to do extra
						char* temp = strdup(backingStore[i]);
						backingStore[i - 1] = backingStore[i + 1];
						backingStore[i] = backingStore[i + 2];

						backingStore[i + 1] = backingStore[i + 3];
						backingStore[i + 2] = backingStore[i + 4];

						backingStore[i + 3] = "none";
						backingStore[i + 4] = temp;
						i--;
						i--;
					}
				} else {
					// PAGE FAULT HERE
					printf("Page fault! Victim Contents:\n");

					char* temp = strdup(frameStore[frameIndex]);
					printf("%s", temp);

					temp = strdup(frameStore[frameIndex + 1]);
					printf("%s", temp);

					temp = frameStore[frameIndex + 2];
					printf("%s", temp);

					printf("End of victim page contents.\n");

					// replace frameStore with directory store
					temp = strdup(directoryStore[directoryIndex]);
					frameStore[frameIndex] = temp;

					temp = strdup(directoryStore[directoryIndex + 1]);
					frameStore[frameIndex + 1] = temp;

					temp = strdup(directoryStore[directoryIndex + 2]);
					frameStore[frameIndex + 2] = temp;

					frameIndex = (frameIndex + 3) % FRAMESIZE;
					directoryIndex += 3;

					// move commands around in the backingStore
					// even num means its the first command of 2
					if (i % 2 == 0){
						// printf("here\n");
						char* temp = strdup(backingStore[i]);
						char* temp2 = strdup(backingStore[i + 1]);

						backingStore[i] = backingStore[i + 2];
						backingStore[i + 1] = backingStore[i + 3];

						backingStore[i + 2] = backingStore[i + 4];
						backingStore[i + 3] = backingStore[i + 5];

						backingStore[i + 4] = temp;
						backingStore[i + 5] = temp2;

						i--;
					} else { // odd means that the second command in RR failed, need to do extra
						char* temp = strdup(backingStore[i]);
						backingStore[i - 1] = backingStore[i + 1];
						backingStore[i] = backingStore[i + 2];

						backingStore[i + 1] = backingStore[i + 3];
						backingStore[i + 2] = backingStore[i + 4];

						backingStore[i + 3] = "none";
						backingStore[i + 4] = temp;
						i--;
						i--;
					}
				}

			}
		}
		i++;
	}

	return 0;
}

int exec(char* scripts[], int size) {
	if(size == 3){
			return run(scripts[1]);
	}
	else if(strcmp(scripts[size-1], "FCFS") == 0){
		for (int i = 1; i < size-1; i++)
		{
			run(scripts[i]);
		}
		return 0;
	}
	else if(strcmp(scripts[size-1], "SJF") == 0){
		return 0;
	}
	else if(strcmp(scripts[size-1], "RR") == 0) {

		if (size == 3) {
// load lines
			char* allLines[1][1000];
			int lineCounts[1];
			int index = 0;

			for (int i = 1; i < 2; i++) {
				int errCode = 0;
				char line[1000];
				char address[] = "";
				char* newAddress = "";
				newAddress = strcat(address, scripts[i]);
				FILE *p = fopen(newAddress,"rt");  // the program is in a file
				size_t lineS = 0;
				int lineCtr = 0;

				if(p == NULL) {
					return badcommandFileDoesNotExist();
				}

				fgets(line,999,p);
				while(1) {

					if(line[strlen(line)-1] != '\n'){
						line[strlen(line)+1] = '\0';
					}

					lineCtr += 1;
					allLines[i-1][lineCtr-1] = strdup(line);

					if(feof(p)) {
						break;
					}

					index += 1;

					fgets(line,999,p);
				}
				lineCounts[i - 1] = lineCtr;
			}
			// files loaded into allLines

			// load into directoryStore
			char* directoryStore[1000];

			for (int i = 0; i < 1000; i++) {
				directoryStore[i] = "none";
			}

			int offset = 0;
			int curProg = 0;
			int maxLines = lineCounts[0];
			int sum = lineCounts[0];


			for (int i = 0; i < sum/2 + 1; i++) {
				if (offset < maxLines) {
					while (offset >= lineCounts[curProg]) {
						curProg = (curProg + 1) % 1;
					}
				}
				for (int j = 0 ; j < 3; j++ ) {
					if (j + offset < lineCounts[curProg]) {
						directoryStore[i * 3 + j] = allLines[curProg][j + offset];
					}
				}
				if (curProg + 1 == 1) {
					offset += 3;
				}
				curProg = (curProg + 1) % 1;
			}

			// loaded into frame

			offset = 0;

			int frameIndex = 0;
			char* backingStore[1000];

			for (int i = 0; i < 1000; i++) {
				backingStore[i] = "none";
			}

			int prog1Done = 1;

			int curSetting;

			// load into backing store as RR
			while (prog1Done) {

				for (int i = 0; i < 1; i++) {
					for (int x = 0; x < 2; x++) {
						if (i == 0 && prog1Done == 0) {

						}
						else if (x + offset >= lineCounts[i]) {

							if (i == 0) {
								prog1Done = 0;
							}
						} else {
							backingStore[frameIndex] = strdup(allLines[i][x + offset]);
						}

					frameIndex++;

					}
				}
				offset += 2;
			}
			// load into frame store

			char* frameStore[FRAMESIZE];

			int minFrame = MIN(FRAMESIZE, 18);

			for (int i = 0; i < FRAMESIZE; i++) {
				frameStore[i] = "free";
			}

			for (int i = 0; i < minFrame; i++) {
				frameStore[i] = strdup(directoryStore[i]);
			}

			// loaded into frameStore

			// Start runnin commands
			frameIndex = minFrame % FRAMESIZE;
			int directoryIndex = minFrame;
			int i = 0;
			while (i < 500) {
				// search frameStore for command
				if (strcmp(backingStore[i], "none") != 0) {
					int found = 0;
					for (int j = 0; j < FRAMESIZE; j++) {
						if (strcmp(frameStore[j], backingStore[i]) == 0) {
							found = 1;
						}
					}

					if (found) {
						parseInput(backingStore[i]);
					} else {

						if (strcmp(frameStore[frameIndex], "free") == 0) {
							printf("%s command", backingStore[i]);
							char* temp = strdup(directoryStore[directoryIndex]);
							frameStore[frameIndex] = temp;

							temp = strdup(directoryStore[directoryIndex + 1]);
							frameStore[frameIndex + 1] = temp;

							temp = strdup(directoryStore[directoryIndex + 2]);
							frameStore[frameIndex + 2] = temp;

							frameIndex = (frameIndex + 3) % FRAMESIZE;
							directoryIndex += 3;

							// move commands around in the backingStore
							// even num means its the first command of 2
							if (i % 2 == 0){
								// printf("here\n");
								char* temp = strdup(backingStore[i]);
								char* temp2 = strdup(backingStore[i + 1]);

								backingStore[i] = backingStore[i + 2];
								backingStore[i + 1] = backingStore[i + 3];

								backingStore[i + 2] = backingStore[i + 4];
								backingStore[i + 3] = backingStore[i + 5];

								backingStore[i + 4] = temp;
								backingStore[i + 5] = temp2;

								i--;
							} else { // odd means that the second command in RR failed, need to do extra
								char* temp = strdup(backingStore[i]);
								backingStore[i - 1] = backingStore[i + 1];
								backingStore[i] = backingStore[i + 2];

								backingStore[i + 1] = backingStore[i + 3];
								backingStore[i + 2] = backingStore[i + 4];

								backingStore[i + 3] = "none";
								backingStore[i + 4] = temp;
								i--;
								i--;
							}
						} else {
							// PAGE FAULT HERE
							printf("Page fault! Victim Contents:\n");

							char* temp = strdup(frameStore[frameIndex]);
							printf("%s", temp);

							temp = strdup(frameStore[frameIndex + 1]);
							printf("%s", temp);

							temp = frameStore[frameIndex + 2];
							printf("%s", temp);

							printf("End of victim page contents.\n");

							// replace frameStore with directory store
							temp = strdup(directoryStore[directoryIndex]);
							frameStore[frameIndex] = temp;

							temp = strdup(directoryStore[directoryIndex + 1]);
							frameStore[frameIndex + 1] = temp;

							temp = strdup(directoryStore[directoryIndex + 2]);
							frameStore[frameIndex + 2] = temp;

							frameIndex = (frameIndex + 3) % FRAMESIZE;
							directoryIndex += 3;

							// move commands around in the backingStore
							// even num means its the first command of 2
							if (i % 2 == 0){
								// printf("here\n");
								char* temp = strdup(backingStore[i]);
								char* temp2 = strdup(backingStore[i + 1]);

								backingStore[i] = backingStore[i + 2];
								backingStore[i + 1] = backingStore[i + 3];

								backingStore[i + 2] = backingStore[i + 4];
								backingStore[i + 3] = backingStore[i + 5];

								backingStore[i + 4] = temp;
								backingStore[i + 5] = temp2;

								i--;
							} else { // odd means that the second command in RR failed, need to do extra
								char* temp = strdup(backingStore[i]);
								backingStore[i - 1] = backingStore[i + 1];
								backingStore[i] = backingStore[i + 2];

								backingStore[i + 1] = backingStore[i + 3];
								backingStore[i + 2] = backingStore[i + 4];

								backingStore[i + 3] = "none";
								backingStore[i + 4] = temp;
								i--;
								i--;
							}
						}

					}
				}
				i++;
			}

			return 0;


		} else if (size == 5) {
			// load lines
			char* allLines[3][1000];
			int lineCounts[3];
			int index = 0;

			for (int i = 1; i < 4; i++) {
				int errCode = 0;
				char line[1000];
				char address[] = "";
				char* newAddress = "";
				newAddress = strcat(address, scripts[i]);
				FILE *p = fopen(newAddress,"rt");  // the program is in a file
				size_t lineS = 0;
				int lineCtr = 0;

				if(p == NULL) {
					return badcommandFileDoesNotExist();
				}

				fgets(line,999,p);
				while(1) {

					if(line[strlen(line)-1] != '\n'){
						line[strlen(line)+1] = '\0';
					}

					lineCtr += 1;
					allLines[i-1][lineCtr-1] = strdup(line);

					if(feof(p)) {
						break;
					}

					index += 1;

					fgets(line,999,p);
				}
				lineCounts[i - 1] = lineCtr;
			}
			// files loaded into allLines

			// load into directoryStore
			char* directoryStore[1000];

			for (int i = 0; i < 1000; i++) {
				directoryStore[i] = "none";
			}

			int offset = 0;
			int curProg = 0;
			int maxLines = MAX(lineCounts[0], MAX(lineCounts[2], lineCounts[1]));
			int sum = lineCounts[0] + lineCounts[1] + lineCounts[2];


			for (int i = 0; i < sum/2 + 1; i++) {
				if (offset < maxLines) {
					while (offset >= lineCounts[curProg]) {
						curProg = (curProg + 1) % 3;
					}
				}
				for (int j = 0 ; j < 3; j++ ) {
					if (j + offset < lineCounts[curProg]) {
						directoryStore[i * 3 + j] = allLines[curProg][j + offset];
					}
				}
				if (curProg + 1 == 3) {
					offset += 3;
				}
				curProg = (curProg + 1) % 3;
			}

			// loaded into frame

			offset = 0;

			int frameIndex = 0;
			char* backingStore[1000];

			for (int i = 0; i < 1000; i++) {
				backingStore[i] = "none";
			}

			int prog1Done = 1;
			int prog2Done = 1;
			int prog3Done = 1;

			int curSetting;

			// load into backing store as RR
			while ((prog1Done || prog2Done || prog3Done)) {

				for (int i = 0; i < 3; i++) {
					for (int x = 0; x < 2; x++) {
						if (i == 0 && prog1Done == 0) {

						} else if (i == 1 && prog2Done == 0) {

						} else if (i == 2 && prog3Done == 0) {

						}
						else if (x + offset >= lineCounts[i]) {

							if (i == 0) {
								prog1Done = 0;
							} else if (i == 1) {
								prog2Done = 0;
							} else if (i == 2) {
								prog3Done = 0;
							}
						} else {
							backingStore[frameIndex] = strdup(allLines[i][x + offset]);
						}

					frameIndex++;

					}
				}
				offset += 2;
			}


			// load into frame store

			char* frameStore[FRAMESIZE];

			int minFrame = MIN(FRAMESIZE, 18);

			for (int i = 0; i < FRAMESIZE; i++) {
				frameStore[i] = "free";
			}

			for (int i = 0; i < minFrame; i++) {
				frameStore[i] = strdup(directoryStore[i]);
			}

			// loaded into frameStore

			// Start runnin commands
			frameIndex = minFrame % FRAMESIZE;
			int directoryIndex = minFrame;
			int i = 0;
			while (i < 500) {
				// search frameStore for command
				if (strcmp(backingStore[i], "none") != 0) {
					int found = 0;
					for (int j = 0; j < FRAMESIZE; j++) {
						if (strcmp(frameStore[j], backingStore[i]) == 0) {
							found = 1;
						}
					}

					if (found) {
						parseInput(backingStore[i]);
					} else {

						if (strcmp(frameStore[frameIndex], "free") == 0) {
							printf("%s command", backingStore[i]);
							char* temp = strdup(directoryStore[directoryIndex]);
							frameStore[frameIndex] = temp;

							temp = strdup(directoryStore[directoryIndex + 1]);
							frameStore[frameIndex + 1] = temp;

							temp = strdup(directoryStore[directoryIndex + 2]);
							frameStore[frameIndex + 2] = temp;

							frameIndex = (frameIndex + 3) % FRAMESIZE;
							directoryIndex += 3;

							// move commands around in the backingStore
							// even num means its the first command of 2
							if (i % 2 == 0){
								// printf("here\n");
								char* temp = strdup(backingStore[i]);
								char* temp2 = strdup(backingStore[i + 1]);

								backingStore[i] = backingStore[i + 2];
								backingStore[i + 1] = backingStore[i + 3];

								backingStore[i + 2] = backingStore[i + 4];
								backingStore[i + 3] = backingStore[i + 5];

								backingStore[i + 4] = temp;
								backingStore[i + 5] = temp2;

								i--;
							} else { // odd means that the second command in RR failed, need to do extra
								char* temp = strdup(backingStore[i]);
								backingStore[i - 1] = backingStore[i + 1];
								backingStore[i] = backingStore[i + 2];

								backingStore[i + 1] = backingStore[i + 3];
								backingStore[i + 2] = backingStore[i + 4];

								backingStore[i + 3] = "none";
								backingStore[i + 4] = temp;
								i--;
								i--;
							}
						} else {
							// PAGE FAULT HERE
							printf("Page fault! Victim Contents:\n");

							char* temp = strdup(frameStore[frameIndex]);
							printf("%s", temp);

							temp = strdup(frameStore[frameIndex + 1]);
							printf("%s", temp);

							temp = frameStore[frameIndex + 2];
							printf("%s", temp);

							printf("End of victim page contents.\n");

							// replace frameStore with directory store
							temp = strdup(directoryStore[directoryIndex]);
							frameStore[frameIndex] = temp;

							temp = strdup(directoryStore[directoryIndex + 1]);
							frameStore[frameIndex + 1] = temp;

							temp = strdup(directoryStore[directoryIndex + 2]);
							frameStore[frameIndex + 2] = temp;

							frameIndex = (frameIndex + 3) % FRAMESIZE;
							directoryIndex += 3;

							// move commands around in the backingStore
							// even num means its the first command of 2
							if (i % 2 == 0){
								// printf("here\n");
								char* temp = strdup(backingStore[i]);
								char* temp2 = strdup(backingStore[i + 1]);

								backingStore[i] = backingStore[i + 2];
								backingStore[i + 1] = backingStore[i + 3];

								backingStore[i + 2] = backingStore[i + 4];
								backingStore[i + 3] = backingStore[i + 5];

								backingStore[i + 4] = temp;
								backingStore[i + 5] = temp2;

								i--;
							} else { // odd means that the second command in RR failed, need to do extra
								char* temp = strdup(backingStore[i]);
								backingStore[i - 1] = backingStore[i + 1];
								backingStore[i] = backingStore[i + 2];

								backingStore[i + 1] = backingStore[i + 3];
								backingStore[i + 2] = backingStore[i + 4];

								backingStore[i + 3] = "none";
								backingStore[i + 4] = temp;
								i--;
								i--;
							}
						}
					}
				}
				i++;
			}

			return 0;

		} else {
			char* allLines[2][1000];
			int lineCounts[2];
			int index = 0;

			for (int i = 1; i < 3; i++) {
				int errCode = 0;
				char line[1000];
				char address[] = "";
				char* newAddress = "";
				newAddress = strcat(address, scripts[i]);
				FILE *p = fopen(newAddress,"rt");  // the program is in a file
				size_t lineS = 0;
				int lineCtr = 0;

				if(p == NULL) {
					return badcommandFileDoesNotExist();
				}

				fgets(line,999,p);
				while(1) {

					if(line[strlen(line)-1] != '\n'){
						line[strlen(line)+1] = '\0';
					}

					lineCtr += 1;
					allLines[i-1][lineCtr-1] = strdup(line);

					if(feof(p)) {
						break;
					}

					index += 1;

					fgets(line,999,p);
				}
				lineCounts[i - 1] = lineCtr;
			}
			// files loaded into allLines

			// load into directoryStore
			char* directoryStore[1000];

			for (int i = 0; i < 1000; i++) {
				directoryStore[i] = "none";
			}

			int offset = 0;
			int curProg = 0;
			int maxLines = MAX(lineCounts[0], lineCounts[1]);
			int sum = lineCounts[0] + lineCounts[1];

			for (int i = 0; i < sum/2 + 1; i++) {
				if (offset < maxLines) {
					while (offset >= lineCounts[curProg]) {
						curProg = (curProg + 1) % 2;
					}
				}
				for (int j = 0 ; j < 3; j++ ) {
					if (j + offset < lineCounts[curProg]) {
						directoryStore[i * 3 + j] = allLines[curProg][j + offset];
					}
				}
				if (curProg + 1 == 2) {
					offset += 3;
				}
				curProg = (curProg + 1) % 2;
			}

			// loaded into frame

			offset = 0;

			int frameIndex = 0;
			char* backingStore[1000];

			for (int i = 0; i < 1000; i++) {
				backingStore[i] = "none";
			}

			int prog1Done = 1;
			int prog2Done = 1;

			int curSetting;

			// load into backing store as RR
			while ((prog1Done || prog2Done)) {

				for (int i = 0; i < 2; i++) {
					for (int x = 0; x < 2; x++) {
						if (i == 0 && prog1Done == 0) {

						} else if (i == 1 && prog2Done == 0) {

						}
						else if (x + offset >= lineCounts[i]) {

							if (i == 0) {
								prog1Done = 0;
							} else if (i == 1) {
								prog2Done = 0;
							}
						} else {
							backingStore[frameIndex] = strdup(allLines[i][x + offset]);
						}

					frameIndex++;

					}
				}
				offset += 2;
			}


			// load into frame store

			char* frameStore[FRAMESIZE];

			int minFrame = MIN(FRAMESIZE, 18);

			for (int i = 0; i < FRAMESIZE; i++) {
				frameStore[i] = "free";
			}

			for (int i = 0; i < minFrame; i++) {
				frameStore[i] = strdup(directoryStore[i]);
			}

			// loaded into frameStore

			// Start runnin commands
			frameIndex = minFrame % FRAMESIZE;
			int directoryIndex = minFrame;
			int i = 0;
			while (i < 500) {
				// search frameStore for command
				if (strcmp(backingStore[i], "none") != 0) {
					int found = 0;
					for (int j = 0; j < FRAMESIZE; j++) {
						if (strcmp(frameStore[j], backingStore[i]) == 0) {
							found = 1;
						}
					}

					if (found) {
						parseInput(backingStore[i]);
					} else {

						if (strcmp(frameStore[frameIndex], "free") == 0) {
							printf("%s command", backingStore[i]);
							char* temp = strdup(directoryStore[directoryIndex]);
							frameStore[frameIndex] = temp;

							temp = strdup(directoryStore[directoryIndex + 1]);
							frameStore[frameIndex + 1] = temp;

							temp = strdup(directoryStore[directoryIndex + 2]);
							frameStore[frameIndex + 2] = temp;

							frameIndex = (frameIndex + 3) % FRAMESIZE;
							directoryIndex += 3;

							// move commands around in the backingStore
							// even num means its the first command of 2
							if (i % 2 == 0){
								// printf("here\n");
								char* temp = strdup(backingStore[i]);
								char* temp2 = strdup(backingStore[i + 1]);

								backingStore[i] = backingStore[i + 2];
								backingStore[i + 1] = backingStore[i + 3];

								backingStore[i + 2] = temp;
								backingStore[i + 3] = temp2;

								i--;
							} else { // odd means that the second command in RR failed, need to do extra
								char* temp = strdup(backingStore[i]);
								backingStore[i - 1] = backingStore[i + 1];
								backingStore[i] = backingStore[i + 2];

								backingStore[i + 1] = "none";
								backingStore[i + 2] = temp;

								i--;
								i--;
							}
						} else {
							// PAGE FAULT HERE
							printf("Page fault! Victim Contents:\n");

							char* temp = strdup(frameStore[frameIndex]);
							printf("%s", temp);

							temp = strdup(frameStore[frameIndex + 1]);
							printf("%s", temp);

							temp = frameStore[frameIndex + 2];
							printf("%s", temp);

							printf("End of victim page contents.\n");

							// replace frameStore with directory store
							temp = strdup(directoryStore[directoryIndex]);
							frameStore[frameIndex] = temp;

							temp = strdup(directoryStore[directoryIndex + 1]);
							frameStore[frameIndex + 1] = temp;

							temp = strdup(directoryStore[directoryIndex + 2]);
							frameStore[frameIndex + 2] = temp;

							frameIndex = (frameIndex + 3) % FRAMESIZE;
							directoryIndex += 3;

							// move commands around in the backingStore
							// even num means its the first command of 2
							if (i % 2 == 0){
								// printf("here\n");
								char* temp = strdup(backingStore[i]);
								char* temp2 = strdup(backingStore[i + 1]);

								backingStore[i] = backingStore[i + 2];
								backingStore[i + 1] = backingStore[i + 3];

								backingStore[i + 2] = temp;
								backingStore[i + 3] = temp2;

								i--;
							} else { // odd means that the second command in RR failed, need to do extra
								char* temp = strdup(backingStore[i]);
								backingStore[i - 1] = backingStore[i + 1];
								backingStore[i] = backingStore[i + 2];

								backingStore[i + 1] = "none";
								backingStore[i + 2] = temp;

								i--;
								i--;
							}
						}
					}
				}
				i++;
			}

			return 0;
		}
	}
	else if(strcmp(scripts[size-1], "AGING") == 0){
		return 0;
	}
	else {
		printf("awdadwa");
		return badcommand();
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
	char address[] = "";
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
	printf("%s\n", "Bad command: same file name");
	return 3;
}
