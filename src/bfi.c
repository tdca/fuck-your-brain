
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#define T 65536

int memory[T];
char instruction[T];
stack_t block;
//stack_t fun_stack;
//stack_t data_stack;
//stack_t addr_stack;

void exec(){
	int p=0,i=0,f=1;	//pointer,iterator,flow-flag,point-register
	char c='\1';	//current-instruction,
	while(c){
		c=instruction[i];	//get each instruction
		if(!f&&c!=']'){	//if flow-control was not avalible
			i++;
			continue;
		}else{
			switch(c){
				case '>': p=(p+1)%T;break;	//pointer moves right
				case '<': p=(p-1)%T;break;	//pointer moves left
				case '+': memory[p]++;break;	//increase the byte at the pointer
				case '-': memory[p]--;break;	//decrease the byte at the pointer
				case '.': putchar(memory[p]);break;	//out put the byte
				case ',': memory[p]=getchar();break;	//get an value & put it into byte at the pointer
				case '[': (memory[p]?(f=1,push(&block,i)):(f=0));break;	//flow-control begins
				case ']': (memory[p]?(i=top(&block)):pop(&block));break;	//flow-control ends
				default:break;	//else then do nothing
			}
		}
		i++;	//next instruction
	}
}

int main(int argc,char* argv[]){
	FILE* b;
	char c;
	int i=0;
	if(argc-1){
		b=fopen(argv[1],"r");
		if( !b ){
			perror("Error: Could not open program file");
			exit(-1);
		}
		while((c=fgetc(b))!=EOF)
			if(c=='<'||c=='>'||c==','||c=='.'||c=='-'||c=='+'||c=='['||c==']')
				instruction[i++] = c;
        fclose(b);
	}else{
	    puts("Interactive mode now on.");
	    puts("Brainfuck Intepreter by KPSN.Leo");
		scanf("%s",instruction);	//get instructions source code
	}
	exec();
	return 0;
}
