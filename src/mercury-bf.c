#include <stdio.h>
#include <string.h>
struct IPU{
    unsigned int head;
    unsigned long lap;             // loop address index
    unsigned char cachetape[200];
};

void forward(struct IPU* interpreter);
void backward(struct IPU* interpreter);
void increase(struct IPU* interpreter);
void decrease(struct IPU* interpreter);
void print(struct IPU* interpreter);



enum Token{
    TOKEN_UNKNOWN = 0x00,
    TOKEN_ARROW_RIGHT,
    TOKEN_ARROW_LEFT,
    TOKEN_DOT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
};

enum Token decode(const char keycode){
    switch (keycode) {
        case '<': return TOKEN_ARROW_LEFT;
        case '>': return TOKEN_ARROW_RIGHT;
        case '+': return TOKEN_PLUS;
        case '-': return TOKEN_MINUS;
        case '.': return TOKEN_DOT;
        case '[': return TOKEN_LEFT_BRACKET;
        case ']': return TOKEN_RIGHT_BRACKET;
        default: return  TOKEN_UNKNOWN;
    }
}



void execute(enum Token *token, struct IPU *interpreter){
    switch (*token) {
        case TOKEN_ARROW_LEFT:      return backward(interpreter);
        case TOKEN_ARROW_RIGHT:     return forward(interpreter);
        case TOKEN_PLUS:            return increase(interpreter);
        case TOKEN_MINUS:           return decrease(interpreter);
        case TOKEN_DOT:             return print(interpreter);
        default: break;
    }
}




void forward(struct IPU* interpreter){
    if(interpreter->head < 200) interpreter->head +=1;
    else interpreter->head = 0;
}
void backward(struct IPU* interpreter){
    if(interpreter->head > 0) interpreter->head -=1;
    else interpreter->head = 199;
}
void increase(struct IPU* interpreter){
    interpreter->cachetape[interpreter->head] +=1; 
}
void decrease(struct IPU* interpreter){
        interpreter->cachetape[interpreter->head] -=1; 
}
void print(struct IPU* interpreter){
    printf("%c",interpreter->cachetape[interpreter->head]);
}



void loop(struct IPU *interpreter, const char* code, unsigned long *pc, unsigned long lp){
    enum Token instruction = decode(code[*pc]);
    unsigned long isRunning = interpreter->cachetape[interpreter->lap];


    if(instruction == TOKEN_RIGHT_BRACKET && isRunning == 0){
          *pc += 1;
          return;
    }
    if(instruction == TOKEN_RIGHT_BRACKET && isRunning > 0){
        *pc = lp;
        return loop(interpreter, code, pc, lp);
    }else{
        execute(&instruction, interpreter);
        *pc += 1;
        return loop(interpreter, code, pc, lp);
    }

}


void interpreter_jit(const char* code, unsigned long* size){
    struct IPU interpreter = {0,0,0};
    unsigned long pc = 0;
    unsigned it = *size;

    while (pc < it) {
        enum Token instruction =   decode(code[pc]);
            if(instruction == TOKEN_LEFT_BRACKET && interpreter.cachetape[interpreter.head] == 0){
                while (decode(code[pc]) != TOKEN_RIGHT_BRACKET) {
                        pc++;
                }
                pc++;
            }
            else if(instruction == TOKEN_LEFT_BRACKET && interpreter.cachetape[interpreter.head] != 0)
            {
                pc++;
                interpreter.lap = interpreter.head;
                loop(&interpreter, code, &pc, pc);
            }
            else{
            execute(&instruction, &interpreter);
            pc++;
        }
    }
}



void handle_input(const char* code, int size){
    int extra;
    char ch;
    if (code[size-1] != '\n') {
        while (((ch = getchar()) != '\n') && (ch != EOF));
    }
}

int main(int argc, char **args){
    if(argc > 1){
        char path[200];
        char code[2000];
        snprintf(path, 200, "%s", args[1]);
        FILE* file = fopen(path, "r");
        fread(code, 1, 2000, file);
        unsigned long codeSize = strlen(code);
        interpreter_jit(code, &codeSize);
    }

    char code[2000];
    while (1) {
       printf("\nmercury: ");
        fgets(code, 2000, stdin);
        unsigned long size = strlen(code);

        handle_input(code, size);
        interpreter_jit(code,&size);
        putchar('\n');
    }
}