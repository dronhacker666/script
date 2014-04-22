#ifndef _VM_H
#define _VM_H

#include "Array.c"
#include "Stack.c"


typedef int bool;
#define true 1
#define false 0


typedef enum {
	PUSH,
	SADD,
	SMUL,
	SDIV,
	VPOP,
	VPUSH,
	GT,
	LT,
	EQ,
	OR,
	CMP_TRUE,
	CMP_FALSE,
	CMP_END,
	GOTO,
	CALL,
	PUSH_SCOPE,
	POP_SCOPE
} CmdType;


char* getCmdName(int type){
	switch(type){
		case PUSH: return "PUSH"; break;
		case SADD: return "SADD"; break;
		case SMUL: return "SMUL"; break;
		case SDIV: return "SDIV"; break;
		case VPOP: return "VPOP"; break;
		case VPUSH: return "VPUSH"; break;
		case GT: return "GT"; break;
		case LT: return "LT"; break;
		case EQ: return "EQ"; break;
		case OR: return "OR"; break;
		case CMP_TRUE: return "CMP_TRUE"; break;
		case CMP_FALSE: return "CMP_FALSE"; break;
		case CMP_END: return "CMP_END"; break;
		case GOTO: return "GOTO"; break;
		case CALL: return "CALL"; break;
		case PUSH_SCOPE: return "PUSH_SCOPE"; break;
		case POP_SCOPE: return "POP_SCOPE"; break;

		default: return "--UNKNOWN--"; break;
	}
}


typedef struct {
	CmdType type;
	union {
		float f_value;
		char* s_value;
		unsigned int hash;
	};
} Cmd;

typedef struct {
	union{
		float f_value;
		void* p_value;
	};
	enum{
		tNumber,
		tPointer
	} type;
} Value;


void pushCmd(Array array, CmdType type){
	printf("C %s\n", getCmdName(type));
	Cmd cmd;
	cmd.type = type;
	ArrayPush(array, &cmd);
}
void pushCmd1f(Array array, CmdType type, float value){
	printf("C %s %f\n", getCmdName(type), value);
	Cmd cmd;
	cmd.type = type;
	cmd.f_value = value; 
	ArrayPush(array, &cmd);
}
void pushCmdAlias(Array array, CmdType type, char* value){
	printf("C %s %s\n", getCmdName(type), value);
	Cmd cmd;
	cmd.type = type;
	cmd.hash = hashString(value);
	ArrayPush(array, &cmd);
}

typedef struct{
	unsigned int hash;
	Value value;
} Var;

void ValueCopy(Value* val_to, Value* val_from){
	val_to->type = val_from->type;
	switch(val_to->type){
		case tNumber:
			val_to->f_value = val_from->f_value;
		break;
	}
}

Var* searchVarByHash(Array array, unsigned int hash){
	Var* p_var;
	ArrayReset(array);
	while(p_var = ArrayNext(array)){
		if(hash==p_var->hash){
			return p_var;
		}
	}
	printf("No find var with hash %i\n", hash);
	return false;
}



void print(Stack stack){
	Value* arg1 = stackPop1p(stack);
	if(arg1->type==tNumber){
		printf("%f\n", arg1->f_value);
	}
}


void exec(Array array){
	int i, hash;
	int wait_for=-1;
	bool logic_trigger = false;
	Cmd* cmd;
	Var var;
	Var* p_var;
	Value* p_value;
	Stack stack = newStack(5);

	Stack scopes = newStack(1024);

	Array root_scope = newArray(sizeof(Var));
	Array current_scope = root_scope;

	var.hash = hashString("print");
	var.value.type = tPointer;
	var.value.p_value = &print;
	ArrayPush(root_scope, &var);


	for(i=0; i<array->length; i++){
		cmd = ArrayGet(array, i);

		if(wait_for!=-1){
			if(wait_for==cmd->type){
				wait_for = -1;
			}else{
				continue;
			}
		}

		switch(cmd->type){
			case GOTO:
				i = cmd->f_value-1;
			break;
			case CALL:
				p_var = searchVarByHash(current_scope, cmd->hash);
				if(p_var){
					if(p_var->value.type==tPointer){
						void (*f)(Stack) = p_var->value.p_value;
						f(stack);
						//stackPush1f(stack, 0); //res
					}
				}
			break;
			case LT:
				p_value = stackPop1p(stack);
				logic_trigger = p_value->f_value < cmd->f_value;
			break;
			case GT:
				p_value = stackPop1p(stack);
				logic_trigger = p_value->f_value > cmd->f_value;
			break;
			case EQ:
				p_value = stackPop1p(stack);
				logic_trigger = p_value->f_value == cmd->f_value;
			break;
			case OR:
				if(logic_trigger==true){
					wait_for = CMP_TRUE;
				}
			break;
			case CMP_TRUE:
				if(logic_trigger==false){
					wait_for = CMP_FALSE;
				}
				if(logic_trigger==-1){
					wait_for = CMP_END;
				}
			break;
			case CMP_FALSE:
				if(logic_trigger==true || logic_trigger==-1){
					wait_for = CMP_END;
				}
			break;
			case CMP_END:
				logic_trigger = -1;
				wait_for = -1;
			break;
			case PUSH:
				p_value = malloc(sizeof(Value));
				p_value->type = tNumber;
				p_value->f_value = cmd->f_value;
				stackPush1p(stack, p_value);
			break;
			case SADD:
				p_value = stackPop1p(stack);
				p_value->f_value += cmd->f_value;
				stackPush1p(stack, p_value);
			break;
			case SMUL:
				p_value = stackPop1p(stack);
				p_value->f_value *= cmd->f_value;
				stackPush1p(stack, p_value);
			break;
			case SDIV:
				p_value = stackPop1p(stack);
				p_value->f_value /= cmd->f_value;
				stackPush1p(stack, p_value);
			break;
			case VPOP:
				p_var = searchVarByHash(current_scope, cmd->hash);
				p_value = stackPop1p(stack);
				if(p_var){
					ValueCopy(&p_var->value, p_value);
				}else{
					var.hash = cmd->hash;
					ValueCopy(&var.value, p_value);
					ArrayPush(current_scope, &var);
				}
			break;
			case VPUSH:
				p_var = searchVarByHash(current_scope, cmd->hash);
				if(p_var){
					stackPush1p(stack, &p_var->value);
				}
			break;


			case PUSH_SCOPE:

			break;
			case POP_SCOPE:


			break;
		}


	}
}

#endif