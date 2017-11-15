
#include <array>
#include <stack>
#include <vector>
#include <unordered_map>
#include <cstdio>

#define MAGIC_NUMBER 65536

using BFStack = std::stack<int>;

constexpr size_t SIZE = 65536;

struct engine
{
    int def_lock;
    int b_lock;
    int cache_cnt;
    int call_ptr;
    int data_ptr;
    int ins_ptr;
    char current = '\1';
    char cache;
};

struct thunk
{
    BFStack block;
    std::vector<int> fun;
    BFStack call;
    BFStack data;
    BFStack addr;
    std::array<int, SIZE> memory{};
    std::array<char, SIZE> instruction{};
};

using HandlerTable = std::unordered_map<char, std::function<void(engine *, thunk *)>>;

HandlerTable table;

#define isdigit(c) ((c) >= '0' && (c) <= '9')

std::function<void(engine *, thunk *)> dispatch(char ins)
{
    return table[ins];
}

void ptr_mov_right(struct engine *eng, struct thunk *tk)
{
    eng->data_ptr = (eng->data_ptr + 1) % SIZE;
}

void ptr_mov_left(struct engine *eng, struct thunk *tk)
{
    eng->data_ptr = (eng->data_ptr - 1) % SIZE;
}

void data_inc(struct engine *eng, struct thunk *tk)
{
    tk->memory[eng->data_ptr]++;
}

void data_dec(struct engine *eng, struct thunk *tk)
{
    tk->memory[eng->data_ptr]--;
}

void input(struct engine *eng, struct thunk *tk)
{
    // TODO: EOF
    tk->memory[eng->data_ptr] = fgetc(stdin);
}

void output(struct engine *eng, struct thunk *tk)
{
    fputc(tk->memory[eng->data_ptr], stdout);
}

void block_begin(struct engine *eng, struct thunk *tk)
{
    if (tk->memory[eng->data_ptr])
    {
        eng->b_lock = 0;
        tk->block.push(eng->ins_ptr);
    }
    else
        eng->b_lock = 1;
}

void block_end(struct engine *eng, struct thunk *tk)
{
    if (tk->memory[eng->data_ptr])
        eng->ins_ptr = tk->block.top();
    else
    {
        tk->block.pop();
        eng->b_lock = 0;
    }
}

void def_begin(struct engine *eng, struct thunk *tk)
{
    tk->fun.push_back(eng->ins_ptr);
    eng->def_lock = 1;
}

void def_end(struct engine *eng, struct thunk *tk)
{
    if (eng->def_lock)
        eng->def_lock = 0;
    else
    {
        eng->ins_ptr = tk->call.top();
        tk->call.pop();
    }
}

void def_return(struct engine *eng, struct thunk *tk)
{
    eng->ins_ptr = tk->call.top();
    tk->call.pop();
}

void call_ptr_inc(struct engine *eng, struct thunk *tk)
{
    eng->call_ptr++;
}

void call(struct engine *eng, struct thunk *tk)
{
    tk->call.push(eng->ins_ptr);
    eng->ins_ptr = tk->fun.at(eng->call_ptr - 1);
    eng->call_ptr = 0;
}

void push_addr(struct engine *eng, struct thunk *tk)
{
    tk->addr.push(eng->data_ptr);
}

void pop_addr(struct engine *eng, struct thunk *tk)
{
    eng->data_ptr = tk->addr.top();
    tk->addr.pop();
}

void push_data(struct engine *eng, struct thunk *tk)
{
    tk->data.push(tk->memory[eng->data_ptr]);
}

void pop_data(struct engine *eng, struct thunk *tk)
{
    if (!tk->data.empty())
    {
        tk->memory[eng->data_ptr] = tk->data.top();
        tk->data.pop();
    }
}

void power(struct engine *eng, struct thunk *tk)
{
    eng->cache_cnt = 0;
    eng->cache = tk->instruction[eng->ins_ptr - 2];
    eng->current = tk->instruction[eng->ins_ptr];
    while (isdigit(eng->current))
    {
        eng->cache_cnt = eng->cache_cnt * 10 + (eng->current - '0');
        eng->current = tk->instruction[++eng->ins_ptr];
    }
    eng->ins_ptr--;
    eng->cache_cnt--;
    if (eng->cache_cnt <= 0)
        eng->cache_cnt = 0;
    return;
}

void assign(struct engine *eng, struct thunk *tk)
{
    eng->current = tk->instruction[eng->ins_ptr++];
    switch (eng->current)
    {
    case '@':
        tk->memory[eng->data_ptr] = tk->addr.top();
        break;
    case '^':
        tk->memory[eng->data_ptr] = tk->call.top();
        break;
    case ':':
        tk->memory[eng->data_ptr] = tk->data.top();
        break;
    case '[':
        tk->memory[eng->data_ptr] = tk->block.top();
        break;
    case '>':
        tk->memory[eng->data_ptr] = tk->memory[(eng->data_ptr + 1) % SIZE];
        break;
    case '<':
        tk->memory[eng->data_ptr] = tk->memory[(eng->data_ptr - 1) % SIZE];
        break;
    case '&':
        eng->data_ptr = tk->memory[eng->data_ptr];
        break;
    case '!':
        tk->call.push(eng->ins_ptr);
        eng->ins_ptr = tk->memory[eng->data_ptr];
        break;
    default:
        eng->ins_ptr--;
        break;
    }
}

void exec(thunk &tk)
{
    engine eng{};
    while (eng.current)
    {
        if (eng.cache_cnt > 0)
        {
            eng.current = eng.cache;
            eng.cache_cnt--;
        }
        else
            eng.current = tk.instruction[eng.ins_ptr++];
        if (eng.b_lock && (eng.current != ']'))
            continue;
        if (eng.def_lock && (eng.current != ';'))
            continue;
        if (dispatch(eng.current))
            dispatch(eng.current)(&eng, &tk);
    }
}

void dispatch_init()
{
    char instbl[] = {
        '>', '<',
        '+', '-',
        ',', '.',
        '[', ']',
        '$', ';',
        '^', '*', '!',
        '&', '@',
        '?', ':',
        '%', '='};
    std::function<void(engine *, thunk *)> hdltbl[] = {
        ptr_mov_right, ptr_mov_left,
        data_inc, data_dec,
        input, output,
        block_begin, block_end,
        def_begin, def_end,
        def_return, call_ptr_inc, call,
        push_addr, pop_addr,
        push_data, pop_data,
        power, assign};
    for (int i = 0; i < std::size(instbl); i++)
    {
        table[instbl[i]] = hdltbl[i];
    }
}

int main(int argc, char *argv[])
{
    FILE *b;
    short c;
    char instruction[MAGIC_NUMBER]{};
    int ins_ptr = 0;
    dispatch_init();
    if (argc - 1)
    {
        if (!(b = fopen(argv[1], "r")))
            return -1;
        while ((c = fgetc(b)) != EOF)
        {
            if (table.find(c) != table.end())
            {
                instruction[ins_ptr++] = c;
                if (c == '%')
                {
                    c = fgetc(b);
                    while (isdigit(c) || c == ' ')
                    {
                        if (c != ' ')
                            instruction[ins_ptr++] = c;
                        c = fgetc(b);
                    }
                    ungetc(c, b);
                }
            }
        }
        thunk tk{};
        fclose(b);
        std::memcpy(tk.instruction.data(), instruction, ins_ptr);
        //        printf("%x\n", (int)&tk);
        //        printf("%s\n", tk.instruction);
        exec(tk);
    }
    else
    {
        puts("[USAGE]- bfi <script-file>");
    }
    return 0;
}
