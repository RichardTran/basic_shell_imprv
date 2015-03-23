
#include "cmd.h"
#include "builtin.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/**
 * wrappers for the exit and chdir function
 */
int call_exit(char * status){
	if(status == NULL)
		exit(0);
	exit(atoi(status));
	return 0;
}//end of call_exit

/**
 * also handles the case of if command is just "cd" 
 * goes to HOME
 */
int call_cd(char * path){
	char * path_dup;
	if(path == NULL){
		path_dup = strdup(getenv("HOME"));
	}
	else{
		path_dup = strdup(path);
	}
	return chdir(path_dup);
}//end of call_cd

/**
 * return -1 if not built in function command
 * return 0 if yes built in function
 */
int is_builtin(cmdList *c){
	if(c->next != NULL)
		return -1;
	if(strcmp(c->argv[0], "exit\0") == 0
			|| strcmp(c->argv[0], "cd\0") == 0)
		return 0;
	else
		return -1;
}//end of isbuiltin

/**
 * executes the built in function that c corresponds to
 */
void builtin(cmdList *c){
	int (*exit_ptr)(char * status) = &call_exit;
	int (*cd_ptr)(char * path) = &call_cd;

	int i = 0, fx_length = 2;

	struct builtin * b_fx[2];

	struct builtin * b_fx1 = (struct builtin *)malloc(sizeof(struct builtin));
	b_fx1->name = strdup("exit\0");
	b_fx1->f = exit_ptr;

	struct builtin * b_fx2 = (struct builtin *) malloc(sizeof(struct builtin));
	b_fx2->name = strdup("cd\0");
	b_fx2->f = cd_ptr;

	b_fx[0] = b_fx1;
	b_fx[1] = b_fx2;

	char * command_name = strdup(c->argv[0]);

	for(i = 0; i < fx_length; i ++){
		if(strcmp(command_name, b_fx[i]->name) == 0){
			b_fx[i]->f(c->argv[1]);
		}//end of if matched
		else{
			//nothing 
		}//end of else NOT MATCHED 
	}//end of for through the builtin functions
}//end of builtin
