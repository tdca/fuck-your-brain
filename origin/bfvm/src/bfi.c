#include <stdio.h>

#define T 65536

unsigned char s[T]="";	//stack
char q[T];	//instruction-queue

void exec(){
	int p=0,i=0,f=1,r;	//pointer,iterator,flow-flag,point-register
	char c='\1';	//current-instruction,
	while(c){
		c=q[i];	//get each instruction
		if(!f&&c!=']'){	//if flow-control was not avalible
			i++;
			continue;
		}else{
			switch(c){
				case '>':((p!=T-1)?(p++):(p=0));break;	//pointer moves right
				case '<':((p)?(p--):(p=T-1));break;	//pointer moves left
				case '+':s[p]++;break;	//increase the byte at the pointer
				case '-':s[p]--;break;	//decrease the byte at the pointer
				case '.':putchar(s[p]);break;	//out put the byte
				case ',':s[p]=getchar();break;	//get an value & put it into byte at the pointer
				case '[':(s[p]?(f=1,r=i):(f=0));break;	//flow-control begins
				case ']':(s[p]&&f?(i=r):(r=r));break;	//flow-control ends
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
				q[i++] = c;
        fclose(b);
	}else{
	    puts("Interactive mode now on.");
	    puts("Brainfuck Intepreter by KPSN.Leo");
		scanf("%s",q);	//get instructions source code
	}
	exec();
}
