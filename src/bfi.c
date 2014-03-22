
#include <stdio.h>
#include "stack.h"

#define MAGIC_NUMBER 65536

int memory[MAGIC_NUMBER];
char instruction[MAGIC_NUMBER];

unsigned char iset[256] = {0};
#define regist_ins(c) iset[(int)c] = 1

#define isdigit(c) ( (c) >= '0' && (c) <= '9' )

stack_t block;
stack_t fun;
stack_t call;
//stack_t data_stack;
stack_t addr;
int data_ptr=0,b_lock=0,ins_ptr=0;

void init_iset(){
	char c,*ins=
		/* --basic-- */    "<>+-.,[]" 
		/* ext-function */ "$^;*!"
		/* ext-stack */    "&@"
		/* ext-literal */  "%";
	while((c = *(ins++))){
		regist_ins(c);
	}
}

void exec(){
	int def_lock = 0,call_ptr = 0;
	char current='\1';
	char cache;
	int cache_cnt=0;
	while(current){
		if(cache_cnt){ 
			current=cache; cache_cnt--;
		}else
			current=instruction[ins_ptr++];
		if(b_lock&&(current!=']')) continue;
		if(def_lock&&(current!=';')) continue;
		switch(current){
			case '>': data_ptr=(data_ptr+1)%MAGIC_NUMBER;  break;
			case '<': data_ptr=(data_ptr-1)%MAGIC_NUMBER;  break;
			case '+': memory[data_ptr]++;  break;
			case '-': memory[data_ptr]--;  break;
			case '.': putchar(memory[data_ptr]);  break;
			case ',': 
				if((memory[data_ptr]=fgetc(stdin)) == EOF)
					return;
				break;
			case '[': 
				if(memory[data_ptr])
					b_lock = 0,push(&block,ins_ptr);
				else 
					b_lock = 1;
				break;
			case ']': 
				if(memory[data_ptr])
					ins_ptr=top(&block);
				else
					pop(&block),b_lock=0;
				break;
			
			case '$': push(&fun,ins_ptr); def_lock=1; break;
			case ';': 
				if(def_lock) 
					def_lock = 0;
				else 
					ins_ptr=pop(&call); 
				break;
			case '^': ins_ptr=pop(&call);  break;
			case '*': call_ptr++;          break;
			case '!': 
				push(&call,ins_ptr); 
				ins_ptr = fun.chunk[call_ptr]; 
				call_ptr=0; 
				break;
			case '&':
				push(&addr,data_ptr);
				break;
			case '@':
				data_ptr=pop(&addr);
				break;
			case '%': 
				cache_cnt=0;
				cache=instruction[ins_ptr-2];
				current=instruction[ins_ptr];
				while(isdigit(current)){
					cache_cnt = cache_cnt*10 + (current-'0');
					current=instruction[++ins_ptr];
				}
				ins_ptr--,cache_cnt--;
				if(cache_cnt <= 0) cache_cnt = 0;
				break;
			default:break;
		}
	}
}

int main(int argc,char* argv[]){
	FILE* b;
	short c;
	int ins_ptr=0;
	init_iset();
	if(argc-1){
		if( !(b=fopen(argv[1],"r")) ) return -1;
		while((c=fgetc(b))!=EOF){
			if(iset[c]){ 
				instruction[ins_ptr++] = c;
				if(c == '%'){
					c=fgetc(b);
					while(isdigit(c)){
						instruction[ins_ptr++] = c;
						c=fgetc(b);
					}
					ungetc(c,b);
				}
			}
		}
        fclose(b);
		exec();
	}else{
	    puts("Interactive mode now on");
	    puts("Brainfuck Intepreter");
	    puts("  --  By Kimmy Leo <kenpusney@gmail.com>");
	    printf("- ");
	    while(scanf("%s",instruction+ins_ptr) != EOF){
	    	exec();
	    	printf("\n- ");
	    }
	    puts("\nByebye!");
	}
	return 0;
}

