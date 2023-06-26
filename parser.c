#include <stdio.h>
#include <stdlib.h>

typedef enum {read, write, id, literal, becomes,
                add, sub, mul, division, lparen, rparen, eof} token;
int xc = 0;
int xt = 0;


extern token scan();
extern char token_image[];

char* names[] = {"read", "write", "id", "literal", "becomes",
                "add", "sub", "mul", "division", "lparen", "rparen", "eof"};

static token input_token;
static token temp_token;

void error() {
    printf("\n");
    printf("at line there is error  %d\n",xc);
    printf("Syantax error\n");
    printf("\n");
    exit(1);
}

void success() {
    printf("\n");
    printf("lines read =  %d\n", xc);
    printf("No errors found \n");
    exit(0);
}

void match(token expected) {
    if (input_token == expected) {
    	temp_token = expected;
        printf("Found match for %s", names[input_token]);
        if (input_token == id || input_token == literal){
        	printf(": %s", token_image);
        }
        printf("\n");
        input_token = scan(xc);
        if(names[temp_token] == names[input_token] && xt != xc){
        	printf("No operator found to separate two id tokens in one line\n");
        	xc--;

        	error();
        }
        xt = xc;

    }
    else {
    	printf("Leaking token: %s\n", names[expected]);
    	error();
    }
}

void program();
void stmt_list();
void stmt();
void expr();
void term_tail();
void term();
void factor_tail();
void factor();
void add_op();
void mult_op();

void program() {
    //printf("predict program\n");
    switch (input_token) {
        case id:
        case read:
        case write:
        case eof:
            stmt_list();
            match(eof);
            success();
            break;
        default: error();
    }
}

void stmt_list() {
    //printf("predict stmt_list\n");
    switch (input_token) {
        case id:
        case read:
        case write:
            stmt();
            stmt_list();

            break;
        case eof:
            break;          /*  epsilon production */
        default: error();
    }
}

void stmt() {
    //printf("predict stmt\n");
    switch (input_token) {
        case id:
            xc++;
            match(id);
            match(becomes);
            expr();
            break;
        case read:
            xc++;
            match(read);
            match(id);
            break;
        case write:
            xc++;
            match(write);
            expr();
            break;
        default: error();
    }
}

void expr() {
    //printf("predict expr\n");
    switch (input_token) {
        case id:
        case literal:
        case lparen:
            term();
            term_tail();
            break;
        default: error();
    }
}

void term_tail() {
    //printf("predict term_tail\n");
    switch (input_token) {
        case add:
        case sub:
            add_op();
            term();
            term_tail();
            break;
        case rparen:
        case id:
        case read:
        case write:
        case eof:
            break;          /*  epsilon production */
        default: error();
    }
}

void term() {
    //printf("predict term\n");
    switch (input_token) {
        case id:
        case literal:
        case lparen:
            factor();
            factor_tail();
            break;
        default: error();
    }
}

void factor_tail() {
    //printf("predict factor_tail\n");
    switch (input_token) {
        case mul:
        case division:
            mult_op();
            factor();
            factor_tail();
            break;
        case add:
        case sub:
        case rparen:
        case id:
        case read:
        case write:
        case eof:
            break;          /*  epsilon production */
        default: error();
    }
}

void factor() {
    //printf("predict factor\n");
    switch (input_token) {
        case id :
            match(id);
            break;
        case literal:
            match(literal);
            break;
        case lparen:
            match(lparen);
            expr();
            match(rparen);
            break;
        default: error();
    }
}

void add_op() {
    //printf("predict add_op\n");
    switch (input_token) {
        case add:
            match(add);
            break;
        case sub:
            match(sub);
            break;
        default: error();
    }
}

void mult_op() {
    //printf("predict mult_op\n");
    switch (input_token) {
        case mul:
            match(mul);
            break;
        case division:
            match(division);
            break;
        default: error();
    }
}

int main() {
    input_token = scan(xc);
   program();
}
