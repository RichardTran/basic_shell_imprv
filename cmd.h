#ifndef CMD_H
#define CMD_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct cmdListStruct{
	int argc;
	char* argv[51];
	struct cmdListStruct* next;
};
typedef struct cmdListStruct cmdList;

#endif
