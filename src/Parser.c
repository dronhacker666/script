#ifndef _PARSER_H
#define _PARSER_H

#include "Array.c"
#include "Lexer.c"


int is_num(char ch){
	return ch>='0' && ch<='9';
}
int is_letter(char ch){
	return (ch>='a'&&ch<='z') || (ch>='A'&&ch<='Z') || ch=='_';
}
int is_separator(char ch){
	return ch=='{'||ch=='}'||ch==';'||ch==' '||ch=='\n'||ch=='('||ch==')';
}
int is_operator(char ch){
	return ch=='+'||ch=='-'||ch=='*'||ch=='/'||ch=='='||ch=='>'||ch=='<';
}

typedef enum{
	PS_wait,
	PS_read_key,
	PS_read_num,
	PS_read_string,
	PS_read_operator,
	PS_read_separator,
} ParserState;


int compareStr(char* s1, char* s2, unsigned int length){
	int i;
	for(i=0; i<length; i++){
		if(!*s1 || !*s2) return 0;
		if(*s1!=*s2) return 0;
		s1++;
		s2++;
	}
	return 1;
}


int parse(Array out, char* source){
	char *ch = source;
	char *pChar, *cur;

	float f_value;
	char trigger;

	ParserState state = PS_wait;
	while(*ch){

		if(state==PS_wait)
		{
			// set state
			if(*ch==' ')
			{
				ch++;
				continue;
			}
			else if(is_letter(*ch))
			{
				state = PS_read_key;
			}
			else if(is_num(*ch))
			{
				state = PS_read_num;
			}
			else if(is_operator(*ch))
			{
				state = PS_read_operator;
			}
			else if(is_separator(*ch))
			{
				state = PS_read_separator;
			}
			cur = ch;
			trigger = 0;
			f_value = 0;
		}

		// process state
		switch(state){
			case PS_read_key:

				if(cur==ch){ // on start lexem
					if(compareStr("if",ch,2))
					{
						pushLexem(out, C_IF);
						ch+=2;
						state=PS_wait;
						continue;
					}
					else if(compareStr("else",ch,4))
					{
						pushLexem(out, C_ELSE);
						ch+=4;
						state=PS_wait;
						continue;
					}
					else if(compareStr("while",ch,5))
					{
						pushLexem(out, C_WHILE);
						ch+=5;
						state=PS_wait;
						continue;
					}
				}

				if(!is_letter(*ch) && !is_num(*ch) )
				{
					pChar = malloc(sizeof(char)*(ch-cur));
					memcpy(pChar, cur, ch-cur);
					pChar[ch-cur] = 0;

					pushLexem1s(out, VAR, pChar);

					state=PS_wait;
					continue;
				}
			break;
			case PS_read_num:
				if(!is_num(*ch) && *ch!='.')
				{
					while(cur<ch){
						if(*cur=='.'){
							trigger = 1;
							cur++;
						}
						if(trigger==0)
						{ // before dot
							f_value = f_value*10 + (*cur-48);
						}
						else
						{// after dot
							f_value += (float)(*cur-48)/(10*trigger);
							trigger++;
						}
						cur++;
					}
					pushLexem1f(out, CONST, f_value);

					state=PS_wait;
					continue;
				}
			break;
			case PS_read_operator:
				if(!is_operator(*ch))
				{
					if(*cur=='=' && *(cur+1)=='=')
					{
						pushLexem(out, OPERATOR_EQ);
					}
					else if(*cur=='=')
					{
						pushLexem(out, OPERATOR_CP);
					}
					else if(*cur=='+')
					{
						pushLexem(out, OPERATOR_ADD);
					}
					else if(*cur=='-')
					{
						pushLexem(out, OPERATOR_SUB);
					}
					else if(*cur=='*')
					{
						pushLexem(out, OPERATOR_MUL);
					}
					else if(*cur=='/')
					{
						pushLexem(out, OPERATOR_DIV);
					}
					else if(*cur=='>')
					{
						pushLexem(out, OPERATOR_GT);
					}
					else if(*cur=='<')
					{
						pushLexem(out, OPERATOR_LT);
					}

					state=PS_wait;
					continue;
				}
			break;
			case PS_read_separator:
				pushLexem1c(out, SEPARATOR, *cur);
				state=PS_wait;
			break;
		}

		ch++;
	}
	return 0;
}

#endif