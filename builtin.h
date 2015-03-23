#ifndef BUILTIN_H
#define BUILTIN_H

#include "cmd.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

struct builtin{
	char *name; //command name
	int (*f)(); // pointer to function
};

int is_builtin(cmdList *c);
void builtin(cmdList *c);
int call_exit(char *  status);
int call_cd(char * path);

#endif
