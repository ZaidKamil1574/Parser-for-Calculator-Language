
#include <stdio.h>
#include <stdlib.h>

typedef enum {read, write, id, literal, becomes,
              add, sub, mul, division, lparen,
			  rparen, eof, program, stmt_list,
			  stmt, expr, term_tail, term,
			  factor_tail, factor, add_op,  mult_op,
			  epsilon, invalid} token;

char* names[] = {"read", "write", "id", "literal", "becomes",
                 "add", "sub", "mul", "division", "lparen",
				 "rparen", "eof","program", "stmt_list",
				 "stmt", "expr", "term_tail", "term",
				 "factor_tail", "factor", "addop", "mulop",
				 "epsilon", "invalid"};

extern token scan();
extern char token_image[];
static token input_token;
static token temp_token; //use to remember previous id

int MAX_SIZE = 100;
token stack[100];   //max
int top = -1;		//start from -1
int xc = 0;//count the line read
int xt = 0;
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


int terminal(token expected){
	if(expected == read || expected == write || expected == id ||
	   expected == literal || expected == becomes || expected == add ||
	   expected == sub || expected == mul || expected == division ||
	   expected == lparen || expected == rparen || expected == eof) return 1;
	else return 0;
}

//Stack part
int empty() {
	if(top == -1)
		return 1;
	else
		return 0;
}

int full() {
	if(top == MAX_SIZE)
		return 1;
	else
		return 0;
}

token pop() {
	int target;
	if(!empty()) {
		target = stack[top];
		top--;
		return target;
	} else {
		printf("\n");
		printf("Stack hungry\n");
		printf("\n");
	}
}

void push(token data) {
	if(!full()) {
		top++;
		stack[top] = data;
	} else {
		printf("\n");
		printf("Stack Ate\n");
		printf("\n");
	}
}


void chudu(token a, token b) {
	switch (a) {
		case program:
			switch (b) {
				case id:
				case read:
				case write:
				case eof:
					push(eof);
					push(stmt_list);
					break;
				default:
					error();
			}
			break;

		case stmt_list:
			switch (b) {
				case id:
				case read:
				case write:
					push(stmt_list);
					push(stmt);
					break;
				case eof:
					break;
				default:
					error();
			}
			break;

		case stmt:
			switch (b) {
				case id:
					push(expr);
					push(becomes);
					push(id);
				    xc++;
					break;
				case read:
					push(id);
					push(read);
					xc++;
					break;
				case write:
					push(expr);
					push(write);
					xc++;
					break;
				default:
					error();
			}
			break;

		case expr:
			switch (b) {
				case lparen:
				case id:
				case literal:
					push(term_tail);
					push(term);
					break;
				default:
					error();
			}
			break;

		case term_tail:
			switch (b) {
				case add:
				case sub:
					push(term_tail);
					push(term);
					push(add_op);
					break;
				case rparen:
				case id:
				case read:
				case write:
				case eof:
					break;
				default:
					error();
			}
			break;

		case term:
			switch (b) {
				case lparen:
				case id:
				case literal:
					push(factor_tail);
					push(factor);
					break;
				default:
					error();
			}
			break;

		case factor_tail:
			switch (b) {
				case mul:
				case division:
					push(factor_tail);
					push(factor);
					push(mult_op);
					break;
				case add:
				case sub:
				case rparen:
				case id:
				case read:
				case write:
				case eof:
					break;
				default:
					error();
			}
			break;

		case factor:
			switch (b) {
				case lparen:
					push(rparen);
					push(expr);
					push(lparen);
					break;
				case id:
					push(id);
					break;
				case literal:
					push(literal);
					break;
				default:
					error();
			}
			break;

		case add_op:
			switch (b) {
				case add:
					push(add);
					break;
				case sub:
					push(sub);
					break;
				default:
					error();
			}
			break;

		case  mult_op:
			switch (b) {
				case mul:
					push(mul);
					break;
				case division:
					push(division);
					break;
				default:
					error();
			}
			break;

		default:
			printf("\n");
			printf("Thelidhu \n");

	    	error();
	}
}


void parser() {
	push(program);

	while(1){
		if (empty()) {
			input_token = scan(xc);
			if(input_token == eof) return;
		}

		token expect = pop();

		if (terminal(expect)) {
			match(expect);
			if (expect == eof) {
				success();
				return;
			}
		} else {
			chudu(expect, input_token);
		}
	}
}

int main(int argc, char *argv[]) {
    input_token = scan(xc);
   parser();
}
