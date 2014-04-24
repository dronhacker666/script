#include <stdio.h>
#include "src/vm.c"
#include "src/Array.c"
#include "src/Lexer.c"
#include "src/Parser.c"

int saveCode(Array array, char* filename){
	FILE *fp;
	if((fp=fopen(filename, "wb+"))==NULL) {
		printf("File create error\n");
		return 1;
	}
	fwrite(array->_data, array->length*array->_item_size, 1, fp);
	fclose(fp);
	return 0;
}

int loadCode(Array array, char* filename){
	FILE *fp;
	if((fp=fopen(filename, "rb+"))==NULL) {
		printf("File open error\n");
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	array->_data = malloc(sz);
	array->length = sz/array->_item_size;

	fread(array->_data, sz, 1, fp);
	fclose(fp);

	return 0;
}

int execFile(char* filename){
	FILE *fp;
	if((fp=fopen(filename, "rb+"))==NULL) {
		printf("File open error\n");
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* data = malloc(sz+1);
	fread(data, sz, 1, fp);
	data[sz] = 0;
	fclose(fp);

	Array arr = newArray(sizeof(Cmd));
	Array lexems = newArray(sizeof(Lexem));

	printf("\n----\nPARSE\n----\n");
	parse(lexems, data);

	printf("\n----\nLEXER\n----\n");
	Lexems2Code(arr, lexems);

	printf("\n----\nEXEC\n----\n");
	exec(arr);
}


int main(){

	execFile("test");

	return 0;
}