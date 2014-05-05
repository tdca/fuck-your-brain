
#include <stdio.h>
#include "stack.h"
#include <string.h>
#define MAGIC_NUMBER 65536

struct engine {
    int def_lock,
    b_lock,
    cache_cnt,
    call_ptr,
    data_ptr,
    ins_ptr;
    char current,
    cache;
};

struct thunk {
    stack_t block,
    fun,
    call,
    data,
    addr;
    int memory[MAGIC_NUMBER];
    char instruction[MAGIC_NUMBER];
};

typedef void (*inshdl_t)(struct engine*, struct thunk*);

inshdl_t __dispatch_tbl[256];
unsigned char iset[256] = {0};
#define regist_ins(c) iset[(int)c] = 1
#define regist_hdl(c,f) __dispatch_tbl[(int)c] = f
#define isdigit(c) ( (c) >= '0' && (c) <= '9' )

int data_ptr = 0, b_lock = 0, ins_ptr = 0;

void init_iset() {
    char c, *ins =
            /* --basic-- */ "<>+-.,[]"
            /* ext-function */ "$^;*!"
            /* ext-stack */ "&@:?"
            /* ext-literal */ "%"
            /* ext-assign */ "=";
    while ((c = *(ins++))) {
        regist_ins(c);
    }
}

void engine_init(struct engine* engine) {
    engine->def_lock = 0;
    engine->call_ptr = 0;
    engine->current = 'a';
    engine->cache_cnt = 0;
}

inshdl_t dispatch(char ins) {
    return __dispatch_tbl[(int) ins];
}

void ptr_mov_right(struct engine* eng, struct thunk* tk) {
    eng->data_ptr = (eng->data_ptr + 1) % MAGIC_NUMBER;
}

void ptr_mov_left(struct engine* eng, struct thunk* tk) {
    eng->data_ptr = (eng->data_ptr - 1) % MAGIC_NUMBER;
}

void data_inc(struct engine* eng, struct thunk* tk) {
    tk->memory[eng->data_ptr]++;
}

void data_dec(struct engine* eng, struct thunk* tk) {
    tk->memory[eng->data_ptr]--;
}

void input(struct engine* eng, struct thunk* tk) {
    // TODO: EOF
    tk->memory[eng->data_ptr] = fgetc(stdin);
}

void output(struct engine* eng, struct thunk* tk) {
    fputc(tk->memory[eng->data_ptr], stdout);
}

void block_begin(struct engine* eng, struct thunk* tk) {
    if (tk->memory[eng->data_ptr])
        eng->b_lock = 0, push(&tk->block, eng->ins_ptr);
    else
        eng->b_lock = 1;
}

void block_end(struct engine* eng, struct thunk* tk) {
    if (tk->memory[eng->data_ptr])
        eng->ins_ptr = top(&tk->block);
    else
        pop(&tk->block), eng->b_lock = 0;
}

void def_begin(struct engine* eng, struct thunk* tk) {
    push(&tk->fun, eng->ins_ptr);
    eng->def_lock = 1;
}

void def_end(struct engine* eng, struct thunk* tk) {
    if (eng->def_lock)
        eng->def_lock = 0;
    else
        eng->ins_ptr = pop(&tk->call);

}

void def_return(struct engine* eng, struct thunk* tk) {
    eng->ins_ptr = pop(&tk->call);
}

void call_ptr_inc(struct engine* eng, struct thunk* tk) {
    eng->call_ptr++;
}

void call(struct engine* eng, struct thunk* tk) {
    push(&tk->call, eng->ins_ptr);
    eng->ins_ptr = tk->fun.chunk[eng->call_ptr];
    eng->call_ptr = 0;
}

void push_addr(struct engine* eng, struct thunk* tk) {
    push(&tk->addr, eng->data_ptr);
}

void pop_addr(struct engine* eng, struct thunk* tk) {
    eng->data_ptr = pop(&tk->addr);
}

void push_data(struct engine* eng, struct thunk* tk) {
    push(&tk->data, tk->memory[eng->data_ptr]);
}

void pop_data(struct engine* eng, struct thunk* tk) {
    tk->memory[eng->data_ptr] = pop(&tk->data);
}

void power(struct engine* eng, struct thunk* tk) {
    eng->cache_cnt = 0;
    eng->cache = tk->instruction[eng->ins_ptr - 2];
    eng->current = tk->instruction[eng->ins_ptr];
    while (isdigit(eng->current)) {
        eng->cache_cnt = eng->cache_cnt * 10 + (eng->current - '0');
        eng->current = tk->instruction[++eng->ins_ptr];
    }
    eng->ins_ptr--, eng->cache_cnt--;
    if (eng->cache_cnt <= 0) eng->cache_cnt = 0;
        return;
}

void assign(struct engine* eng, struct thunk* tk) {
    eng->current = tk->instruction[eng->ins_ptr++];
    switch (eng->current) {
        case '@': tk->memory[eng->data_ptr] = top(&tk->addr);
            break;
        case '^': tk->memory[eng->data_ptr] = top(&tk->call);
            break;
        case ':': tk->memory[eng->data_ptr] = top(&tk->data);
            break;
        case '[': tk->memory[eng->data_ptr] = top(&tk->block);
            break;
        case '>':
            tk->memory[eng->data_ptr] = tk->memory[(eng->data_ptr + 1) % MAGIC_NUMBER];
            break;
        case '<':
            tk->memory[eng->data_ptr] = tk->memory[(eng->data_ptr - 1) % MAGIC_NUMBER];
            break;
        case '&': eng->data_ptr = tk->memory[eng->data_ptr];
            break;
        case '!': push(&tk->call, eng->ins_ptr);
            eng->ins_ptr = tk->memory[eng->data_ptr];
            break;
        default: eng->ins_ptr--;
            break;
    }
}

void exec(struct thunk* tk) {
    struct engine eng = {};
    engine_init(&eng);
    while (eng.current) {
        if (eng.cache_cnt > 0) {
            eng.current = eng.cache;
            eng.cache_cnt--;
        } else
            eng.current = tk->instruction[eng.ins_ptr++];
        if (eng.b_lock && (eng.current != ']')) continue;
        if (eng.def_lock && (eng.current != ';')) continue;
        if(dispatch(eng.current)) dispatch(eng.current)(&eng, tk);
    }
}

void dispatch_init() {
    int i;
    char instbl[] = {
        '>', '<',
        '+', '-',
        ',', '.',
        '[', ']',
        '$', ';',
        '^', '*', '!',
        '&', '@',
        '?', ':',
        '%', '='
    };
    inshdl_t hdltbl[] = {
        ptr_mov_right,  ptr_mov_left,
        data_inc,       data_dec,
        input,          output,
        block_begin,    block_end,
        def_begin,      def_end,
        def_return,     call_ptr_inc,   call,
        push_addr,      pop_addr,
        push_data,      pop_data,
        power,          assign
    };
    for (i = 0; i < sizeof (instbl); i++){
//        printf("%c %x\n",instbl[i],(unsigned int)hdltbl[i]);
        regist_hdl(instbl[i], hdltbl[i]);
    }
}

int main(int argc, char* argv[]) {
    FILE* b;
    short c;
    char instruction[MAGIC_NUMBER] = "";
    int ins_ptr = 0;
    init_iset();
    dispatch_init();
    if (argc - 1) {
        if (!(b = fopen(argv[1], "r"))) return -1;
        while ((c = fgetc(b)) != EOF) {
            if (iset[c]) {
                instruction[ins_ptr++] = c;
                if (c == '%') {
                    c = fgetc(b);
                    while (isdigit(c) || c == ' ') {
                        if (c != ' ')
                            instruction[ins_ptr++] = c;
                        c = fgetc(b);
                    }
                    ungetc(c, b);
                }
            }
        }
        struct thunk tk = {};
        fclose(b);
        memcpy(tk.instruction, instruction, ins_ptr);
//        printf("%x\n", (int)&tk);
//        printf("%s\n", tk.instruction);
        exec(&tk);
    } else {
        puts("[USAGE]- bfi <script-file>");
    }
    return 0;
}

