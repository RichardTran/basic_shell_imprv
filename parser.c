#include "cmd.h"
#include "builtin.h"
#define MAX 512


// usually the first cmd since it requires no input. 
void runsrc2(int pfd[], int pfd2[],char* cmd1[]){
	int pid;
	switch(pid = fork()){
	
	case 0:
		dup2(pfd[1],1); // replace stdout with pfd[1]. can be read by pfd[0] due to piping
		close(pfd[0]);
//		close(pfd2[0]);
//		close(pfd2[1]);
		execvp(cmd1[0], cmd1); // writes into stdout. into pfd[1];
		perror(cmd1[0]);
	default:
		break;	
	case -1:
		perror("fork");
		exit(1);
	}
}
void transfer(int pfd[], int pfd2[], char*cmd2[]){
	int pid;
	switch(pid = fork()){
	case 0:
		dup2(pfd[0],0);
		close(pfd[1]);
		dup2(pfd2[1],1);
		close(pfd2[0]);
		execvp(cmd2[0],cmd2);
		perror(cmd2[0]);
	default:
//		wait(NULL);
		break;
	
	case -1:
		perror("fork");
		exit(1);
	}
}

// reads input if it can, then produces output to stdout. Last step to print into terminal
void rundest2(int pfd[], int pfd2[], char* cmd2[]){
	int pid;
	switch(pid = fork()){
	case 0: // child process
		close(pfd[0]);
		dup2(pfd2[0],0); // pfd[0] becomes stdin // becomes the input for 2nd program
		close(pfd[1]);
		close(pfd2[1]);// the magic key
		execvp(cmd2[0],cmd2);//reads whatever is in pfd[0]
		perror(cmd2[0]);
	default:
		break;
	case -1:
		perror("fork");
		exit(1);	
	}
}

void runsrc(int pfd[], char* cmd[]){
	int pid;
	switch(pid = fork()){
	case 0:
		dup2(pfd[1],1);
		close(pfd[0]);
		execvp(cmd[0],cmd);
		perror(cmd[0]);
	default:
		break;
	case -1:
		perror("fork");
		exit(1);
	}
}
void rundest(int pfd[], char* cmd[]){
	int pid;
	switch(pid = fork()){
	case 0:
		dup2(pfd[0],0);
		close(pfd[1]);
		execvp(cmd[0],cmd);
		perror(cmd[0]);
	default:
		break;
	case -1:
		perror("fork");
		exit(1);
	}
}

int numOfCmds(cmdList *head){
	cmdList* ptr = head;
	int i = 0;
	while(ptr!=NULL){
		i=i+1;
		ptr = ptr->next;
	}
	return i;
}
void piping(cmdList *head){
	int pid, status;
	int child_status;
	int exe_id;
	int cmdCount = numOfCmds(head);
	int fd[2];
	int fd2[2];
//	int fd3[2];
	cmdList* cmd1 = head;
//	cmdList* cmd2 = head->next;
	pipe(fd); // produces two unique file descriptors. 
		  // whatever is written in fd[1] can be read in fd[0]
	pipe(fd2);
//	pipe(fd3);
	if(cmdCount == 1){
		switch(pid = fork()){
		case 0:
			execvp(cmd1->argv[0],cmd1->argv);
			exit(0);
		default:
	//		while((pid = wait(&status)) != -1){
	//		}
			pid = wait(&status);
			fprintf(stderr,"process %d exits with %d\n", pid, WEXITSTATUS(status));
			return;
		}
	}
	// two commands	
	else if(cmdCount == 2)
	{
		runsrc(fd,cmd1->argv);
		rundest(fd,cmd1->next->argv);
		close(fd[0]); close(fd[1]);
		while ((pid = wait(&status)) != -1){	/* pick up all the dead children */
			fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
		}
		return;
	}
	// three commands
	else if(cmdCount == 3){
		runsrc2(fd,fd2,cmd1->argv);
		transfer(fd, fd2, cmd1->next->argv);
		rundest2(fd,fd2,cmd1->next->next->argv);
		close(fd[0]);// close(fd2[1]); close(fd[1]); close(fd2[1]);
		close(fd2[0]);
		close(fd[1]);
		close(fd2[1]);
		while ((pid = wait(&status)) != -1){	/* pick up all the dead children */
			fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
		}
		return;
	}
	else{
		return;
	}
}

cmdList* insertCmd(char* arguments[],int count, cmdList* cmdTail){
	char c;
	int i;
	int stringIndex = 0;
	cmdList* ptr = cmdTail;
	if(cmdTail!=NULL){
		ptr->next = malloc(sizeof(struct cmdListStruct));
		ptr = ptr->next;
	}
	else{
		ptr = malloc(sizeof(struct cmdListStruct));
	}
	ptr->argc = count;
	for(i = 0; i<count; i++){
		ptr->argv[i] = (arguments[i]);
	}
	ptr->argv[count] = NULL;
	return ptr;
}

cmdList* parseCmds(char* input){
	int i = 0;
	int ptrFlag = 0;
	int quoteFlag = 0;
	int bufferIndex = 0;
	char c = (char)*input;	// c = input[0];
	char* buffer[51];
	cmdList* cmdListHead = NULL;
	cmdList* cmdListTail = NULL;
	while(c!='\0'){
		if( (c=='|' || c=='\n' || c=='\0')  && quoteFlag == 0){
			if(ptrFlag==1){ // Case when no space between string and pipe character
				*(input+i)='\0';
			}
			ptrFlag = 0;
			if(cmdListHead == NULL && bufferIndex!=0){
				cmdListHead = insertCmd(buffer,bufferIndex,cmdListHead);
				cmdListTail = cmdListHead;
			}
			else if(cmdListHead != NULL&& bufferIndex!=0){
				cmdListTail->next = insertCmd(buffer,bufferIndex,cmdListTail);
				cmdListTail = cmdListTail->next;
			}
			bufferIndex = 0;
		}
		else if(ptrFlag==0 && c!=' ' && c!= '\t' && quoteFlag == 0){
			ptrFlag=1;
			if(bufferIndex>50){
				printf("Too many arguments. Only 50 are allowed\n");
				return NULL;
			}
			if(c=='\''){
				quoteFlag = 1;
				buffer[bufferIndex] = (char*)(input+i+1);
			}
			else if(c=='\"'){
				quoteFlag = 2;
				buffer[bufferIndex] = (char*)(input+i+1);
			}
			else{
				buffer[bufferIndex] = (char*)(input+i);
			}
			bufferIndex = bufferIndex+1;
		}
		else if(ptrFlag == 1 && quoteFlag ==0 && c== ' '){
			ptrFlag = 0;
			*(input+i) = '\0';
		}
		else if(ptrFlag == 1 && quoteFlag == 0 && (c == '\"' || c == '\'')){
			ptrFlag = 1;
			if(c == '\''){
				quoteFlag = 1;
			}
			else if (c == '\"'){
				quoteFlag = 2;
			}
			buffer[bufferIndex] = (char*)(input+i+1);
			bufferIndex = bufferIndex+1;
			*(input+i) = '\0';
		}
		else if(quoteFlag == 1 && c == '\'' || quoteFlag == 2 && c == '\"'){//closing quote
			quoteFlag = 0;
			ptrFlag = 0;
			*(input+i) = '\0';
		}
		i = i+1;
		c = *(input+i);
	}
	if(quoteFlag!=0){
		printf("Error: Mismatched quotes\n");
		return NULL;
	}
	
	if(bufferIndex!=0){
		if(cmdListHead == NULL){
			cmdListHead = insertCmd(buffer,bufferIndex,cmdListHead);
			cmdListHead->next = NULL;
			cmdListTail = cmdListHead;
		}
		else{
			cmdListTail->next = insertCmd(buffer, bufferIndex, cmdListTail);
			cmdListTail = cmdListTail->next;
			cmdListTail->next = NULL;
		}
	}
	return cmdListHead;
}

int main(int argc, char** argv){
	char cmdLineInput[MAX];
	int showprompt = isatty(0);
	cmdList* list;
	cmdList* DEBUG_PTR;
	int exitStatus;
	while(1==1){
		if(showprompt){
			printf("$ ");
			if(exitStatus=fgets(cmdLineInput,MAX,stdin)==0){
				exit(0);
			}
		}
		list = parseCmds(cmdLineInput);
		if(list!=NULL&&is_builtin(list) == 0){
			builtin(list);
		}
		else if(list!=NULL){
			piping(list);
		}
	}
}
