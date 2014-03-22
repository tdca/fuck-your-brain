
#include <stdio.h>
#include "stack.h"

#define T 65536

int memory[T];
char instruction[T];

unsigned char iset[256] = {0};
#define regist_ins(c) iset[(int)c] = 1

stack_t block;
stack_t fun;
stack_t call;
//stack_t data_stack;
//stack_t addr_stack;
int p=0,f=0,i=0;

void debug();

void init_iset(){
	// basic
	regist_ins('>');
	regist_ins('<');
	regist_ins('+');
	regist_ins('-');
	regist_ins('.');
	regist_ins(',');
	regist_ins('[');
	regist_ins(']');
	
	// ext-fun
	regist_ins('$');
	regist_ins('^');
	regist_ins(';');
	regist_ins('*');
	regist_ins('!');
}

void exec(){
	int fdef = 0,fcall = 0;
	char c='\1';	//current-instruction
	while(c){
		c=instruction[i++];	//get each instruction
		if(f&&(c!=']')){ continue; }
		if(fdef&&(c!=';')){ continue; }
		switch(c){
			case '>': p=(p+1)%T;  break;
			case '<': p=(p-1)%T;  break;
			case '+': memory[p]++;  break;
			case '-': memory[p]--;  break;
			case '.': putchar(memory[p]);  break;
			case ',': if((memory[p]=fgetc(stdin)) == EOF) return; break;
			case '[': if(memory[p]){ f = 0; push(&block,i);}else{ f = 1; }       break;
			case ']': (memory[p]?(i=top(&block)):(pop(&block),f=0));    break;
			case '$': push(&fun,i); fdef=1; break;
			case ';': if(fdef) fdef = 0;else i=pop(&call); break;
			case '^': i=pop(&call);  break;
			case '*': fcall++;       break;
			case '!': push(&call,i); i = fun.chunk[fcall]; fcall=0; break;
			default:break;
		}
	}
}

int main(int argc,char* argv[]){
	FILE* b;
	short c;
	int i=0;
	init_iset();
	if(argc-1){
		if( !(b=fopen(argv[1],"r")) ) return -1;
		while((c=fgetc(b))!=EOF)
			if(iset[c]) instruction[i++] = c;
        fclose(b);
		exec();
	}else{
	    puts("Interactive mode now on");
	    puts("Brainfuck Intepreter");
	    puts("  --  By Kimmy Leo <kenpusney@gmail.com>");
	    printf("- ");
	    while(scanf("%s",instruction+i) != EOF){
	    	exec();
	    	printf("\n- ");
	    }
	    puts("\nByebye!");
	}
	return 0;
}

void debug(){
	int c;
	while((c = pop(&fun))){
		printf("%d",c);
		printf("%c",instruction[c]);
	}
}
