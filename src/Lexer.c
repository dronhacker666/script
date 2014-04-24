#ifndef _LEXER_H
#define _LEXER_H

#include "Array.c"
#include "Stack.c"

typedef struct{
	enum{
		VAR,
		CONST_NUMBER,
		CONST_STRING,
		SEPARATOR,

		OPERATOR_CP,
		OPERATOR_GT,
		OPERATOR_LT,
		OPERATOR_ADD,
		OPERATOR_SUB,
		OPERATOR_MUL,
		OPERATOR_DIV,
		OPERATOR_EQ,

		C_IF,
		C_ELSE,
		C_WHILE,
	} type;
	union{
		float f_value;
		char* s_value;
		char c_value;
	};
} Lexem;

char* getLexName(int type){
	switch(type){
		case VAR: return "VAR"; break;
		case CONST_NUMBER: return "CONST_NUMBER"; break;
		case CONST_STRING: return "CONST_STRING"; break;
		case SEPARATOR: return "SEPARATOR"; break;

		case OPERATOR_CP: return "OPERATOR_CP"; break;
		case OPERATOR_GT: return "OPERATOR_GT"; break;
		case OPERATOR_LT: return "OPERATOR_LT"; break;
		case OPERATOR_ADD: return "OPERATOR_ADD"; break;
		case OPERATOR_SUB: return "OPERATOR_SUB"; break;
		case OPERATOR_MUL: return "OPERATOR_MUL"; break;
		case OPERATOR_DIV: return "OPERATOR_DIV"; break;
		case OPERATOR_EQ: return "OPERATOR_EQ"; break;

		case C_IF: return "C_IF"; break;
		case C_ELSE: return "C_ELSE"; break;
		case C_WHILE: return "C_WHILE"; break;

		default: return "--UNKNOWN--"; break;
	}
}


void pushLexem(Array array, int type){
	printf("L %s\n", getLexName(type));
	Lexem lexem;
	lexem.type = type;
	ArrayPush(array, &lexem);
}
void pushLexem1f(Array array, int type, float value){
	printf("L %s %f\n", getLexName(type), value);
	Lexem lexem;
	lexem.type = type;
	lexem.f_value = value; 
	ArrayPush(array, &lexem);
}
void pushLexem1s(Array array, int type, char* value){
	printf("L %s %s\n", getLexName(type), value);
	Lexem lexem;
	lexem.type = type;
	lexem.s_value = value;
	ArrayPush(array, &lexem);
}
void pushLexem1c(Array array, int type, char value){
	printf("L %s %c\n", getLexName(type), value);
	Lexem lexem;
	lexem.type = type;
	lexem.c_value = value;
	ArrayPush(array, &lexem);
}



int Lexems2Code(Array out, Array lexems){
	int i;
	Stack _if = newStack(10);
	Lexem* lex; 
	Lexem* next_lex;
	Lexem* next_lex2;
	Lexem* r=0;

	Stack store_commands = newStack(256);
	Stack store_functions = newStack(256);
	Stack store_goto = newStack(256);

	for(i=0; i<lexems->length; i++){
		lex = ArrayGet(lexems, i);

		switch(lex->type){
			case VAR:
				next_lex = ArrayGet(lexems,i+1);

				switch( next_lex->type ){
					case OPERATOR_ADD:
					case OPERATOR_SUB:
					case OPERATOR_MUL:
					case OPERATOR_DIV:
					//case OPERATOR_CP:
					case CONST_NUMBER:
					case OPERATOR_GT:
					case OPERATOR_LT:
						pushCmdAlias(out, VPUSH, lex->s_value);
						break;
					case OPERATOR_CP:
						// for skip
					break;
					case SEPARATOR:
						if(next_lex->c_value!='('){
							pushCmdAlias(out, VPUSH, lex->s_value);
						}
					break;
					default:
						printf("ERROR: error lexem %s\n", getLexName(next_lex->type));
						return 1;
					break;
				}

				switch( next_lex->type ){
					case OPERATOR_CP:
						r = lex;
						i++;
					break;
					case SEPARATOR:
						if(next_lex->c_value=='('){
							stackPush1p(store_functions, lex);
							i++;
						}
					break;
				}
				
			break;
			case CONST_NUMBER:
				pushCmd1f(out, PUSH, lex->f_value);
			break;
			case CONST_STRING:
				pushCmd1s(out, PUSH_S, lex->s_value);
			break;
			case OPERATOR_SUB:
			case OPERATOR_ADD:
				next_lex = ArrayGet(lexems,i+1);
				next_lex2 = ArrayGet(lexems,i+2);

				if(
					next_lex2->type==OPERATOR_MUL ||
					next_lex2->type==OPERATOR_DIV
				){
					stackPush1i(store_commands, lex->type==OPERATOR_ADD?SADDS:SSUBS);
				}else{
					if(next_lex->type==CONST_NUMBER){
						pushCmd1f(out, lex->type==OPERATOR_ADD?SADD:SSUB, next_lex->f_value);
						i++;
					}
				}
			break;
			case OPERATOR_MUL:
			case OPERATOR_DIV:
				next_lex = ArrayGet(lexems,i+1);
				if(next_lex->type==CONST_NUMBER){
					pushCmd1f(out, lex->type==OPERATOR_DIV?SDIV:SMUL, next_lex->f_value);
					i++;
				}
				if(next_lex->type==VAR){
					pushCmdAlias(out, VPUSH, next_lex->s_value);
					pushCmd(out, next_lex->type==OPERATOR_DIV?SDIVS:SMULS);
					i++;
				}

				while(!stackIsEmpty(store_commands)){
					pushCmd(out, stackPop1i(store_commands));
				}

			break;
			case SEPARATOR:

				switch(lex->c_value){
					case ')':
						if(!stackIsEmpty(store_functions)){
							pushCmdAlias(out, CALL, ((Lexem*)stackPop1p(store_functions))->s_value);
						}
					break;
					case ';':
						if(r){
							pushCmdAlias(out, VPOP, r->s_value);
							r = 0;
						}
					break;
					case '{':
						if(stackHead1i(_if)==C_IF){
							pushCmd(out, CMP_TRUE);
						}
					break;
					case '}':
						if(stackHead1i(_if)==C_IF){
							next_lex = ArrayGet(lexems,i+1);
							if(next_lex->type==C_ELSE){

							}else{
								if(!stackIsEmpty(store_goto)){
									pushCmd1f(out, GOTO, stackPop1i(store_goto));
								}

								pushCmd(out, CMP_FALSE);
								pushCmd(out, CMP_END);
								stackPop1i(_if);
							}
						}
						if(stackHead1i(_if)==C_ELSE){
							pushCmd(out, CMP_END);
						}
					break;
				}

			break;

			case C_IF:
				stackPush1i(_if, C_IF);
			break;
			case C_ELSE:
				pushCmd(out, CMP_FALSE);
				stackPush1i(_if, C_ELSE);
			break;

			case C_WHILE:
				stackPush1i(store_goto, out->length);
				stackPush1i(_if, C_IF);
			break;

			case OPERATOR_EQ:
				next_lex = ArrayGet(lexems,i+1);
				if(next_lex->type==CONST_NUMBER){
					pushCmd1f(out, EQ, next_lex->f_value);
					i++;
				}
			break;
			case OPERATOR_GT:
				next_lex = ArrayGet(lexems,i+1);
				if(next_lex->type==CONST_NUMBER){
					pushCmd1f(out, GT, next_lex->f_value);
					i++;
				}
			break;
			case OPERATOR_LT:
				next_lex = ArrayGet(lexems,i+1);
				if(next_lex->type==CONST_NUMBER){
					pushCmd1f(out, LT, next_lex->f_value);
					i++;
				}
			break;

		}

	}

	return 1;
}


#endif