/*
**Brainfuck Virtual Machine
*/

#include <stdio.h>


/***Configure***/
#define T 65536

/***type define***/
typedef enum _OPCODE{
	in=1,
	out=2,
	ls=3,
	le=4,
	dd=6,
	di=5,
	pd=8,
	pi=7,
	end=0
} opcode;

/***Main structure***/
unsigned char ds[T]="";	//Data stack
int is[T]={end};	//Instruction stack
unsigned char port[T]="";	//Port to i/o
/*
**Data pointer,Instruction pointer
**Program state word,Instruction Cache
*/
int dp=0,ip=0,pwd=1,ic;
//Data register & Instruction register
#define dr ds[dp]
#define ir is[ip]

/***Non-Runtime structure***/
char ss[T]="";	//Source code stack
unsigned char bs[T]="";	//Bytecode stack
int sp=0,bp=0;	//Stack pointer & Bytecode pointer

/***Get Files***/
void gs(const char* sf);	//Get source

/***Put Files***/
void pb(const char* bf);	//Put bytecode
void lb(const char* bf);	//Load bytecode

/***Compilers***/
void s2b();	//Source to Bytecode

/***Interpreters***/
void b2i();	//Bytecode to Instruction

/***Executors***/
void id();

/***Debuggers***/
void debug();


int main(int argc, char *argv[]) {
	FILE *prog_file;
	int i,c;
	if(!(argc-2)||!(argc-1)){
		puts("Too few arguments");
		exit(0);
	}else if(!strcmp(argv[1],"-c") ){	//Compile source code to bytecodes
		gs(argv[2]);
		s2b();
		pb(argv[3]);
	}else if(!strcmp(argv[1],"-e") ){	//Executes the bytecode
	    lb(argv[2]);
		b2i();
		id();
	}else if(!strcmp(argv[1],"-i") ){	//Compile Source & Interpretes
		gs(argv[2]);
		s2b();
		pb("temp.byte");
		lb("temp.byte");
		b2i();
        id();
	}else if(!strcmp(argv[1],"--debug") ){	//Executes Bytecode in Debugging Mode.s
	    lb(argv[2]);
		b2i();
		debug();
	}else{
        puts("Wrong arguments.");
	}
}


void s2b(){
	char c;
	unsigned char b=0;
	sp=bp=0;
	while(c=ss[sp]){
		switch(c){
			case '+':sp++,b+=(di*0x10);break;
			case '-':sp++,b+=(dd*0x10);break;
			case '>':sp++,b+=(pi*0x10);break;
			case '<':sp++,b+=(pd*0x10);break;
			case '[':sp++,b+=(ls*0x10);break;
			case ']':sp++,b+=(le*0x10);break;
			case '.':sp++,b+=(out*0x10);break;
			case ',':sp++,b+=(in*0x10);break;
			case '\0':b+=end;break;
		}
		switch(ss[sp]){
			case '+':sp++,b+=(di);break;
			case '-':sp++,b+=(dd);break;
			case '>':sp++,b+=(pi);break;
			case '<':sp++,b+=(pd);break;
			case '[':sp++,b+=(ls);break;
			case ']':sp++,b+=(le);break;
			case '.':sp++,b+=(out);break;
			case ',':sp++,b+=(in);break;
			case '\0':b+=(end);break;
		}
		bs[bp++]=b;
		b=0;
	}
}

void gs(const char* sf){
	FILE* fp;
	char c;
	fp = fopen(sf,"r");
	if( !fp ){
		perror("Error: Could not open program file");
		exit(-1);
	}
	while((c=fgetc(fp))!=EOF)
		if(c=='<'||c=='>'||c==','||c=='.'||c=='-'||c=='+'||c=='['||c==']')
			ss[sp++] = c;
	fclose(fp);
}

void lb(const char* bf){
    FILE* fp;
	char c;
	bp=0;
	fp = fopen(bf,"rb");
    if( !fp ){
		perror("Error: Could not open byte file");
		exit(-1);
	}
	while((c=fgetc(fp))!=EOF)
		bs[bp++]=c;
    fclose(fp);
}

void b2i(){
	unsigned char c;
	int i;
	ip=bp=0;
	while(c=bs[bp]){
        is[ip]=c/0x10,ip++,is[ip]=c%0x10,ip++,bp++;
	}
    ip=0;
}

/***Put Files***/
void pb(const char* bf){
	FILE* fp;
	fp = fopen(bf,"wb");
	if( !fp ){
		perror("Error: Could not open program file");
		exit(-1);
	}
	fputs(bs,fp);
	fclose(fp);
}

/***Executors***/
void id(){
    opcode c;
    ip=dp=0;
	while(c=is[ip]){
	    if(!pwd&&c!=le){	//if flow-control was not avalible
			ip++;
			continue;
		}else{
			switch(c){
				case pi:((dp!=T-1)?(dp++):(dp=0));break;	//pointer moves right
				case pd:((dp)?(dp--):(dp=T-1));break;	//pointer moves left
				case di:dr++;break;	//increase the byte at the pointer
				case dd:dr--;break;	//decrease the byte at the pointer
				case out:putchar(dr);break;	//out put the byte
				case in:dr=getchar();break;	//get an value & put it into byte at the pointer
				case ls:(dr?(pwd=1,ic=ip):(pwd=0));break;	//flow-control begins
				case le:(dr&&pwd?(ip=ic):(ic=ic));break;	//flow-control ends
				default:break;	//else then do nothing
			}
			ip++;
		}
	}
}

/***Debuggers***/
void debug(){
    opcode c;
    ip=dp=0;
    puts("BFM debugging:");
    puts("Load instructions now:");
	while(c=is[ip]){
	    if(!pwd&&c!=le){	//if flow-control was not avalible
			ip++;
			continue;
		}else{
			switch(c){
				case pi:((dp!=T-1)?(dp++):(dp=0));break;	//pointer moves right
				case pd:((dp)?(dp--):(dp=T-1));break;	//pointer moves left
				case di:dr++;break;	//increase the byte at the pointer
				case dd:dr--;break;	//decrease the byte at the pointer
				case out:putchar('\n');putchar(dr);break;	//out put the byte
				case in:dr=getchar();break;	//get an value & put it into byte at the pointer
				case ls:(dr?(pwd=1,ic=ip):(pwd=0));break;	//flow-control begins
				case le:(dr&&pwd?(ip=ic):(ic=ic));break;	//flow-control ends
				default:break;	//else then do nothing
			}
            printf("CI:%.2d\tNI:%.2d\tCD:%.3d\tCP:%.5d",is[ip],is[ip+1],ds[dp],dp);
            getchar();
			ip++;
		}
	}
}

