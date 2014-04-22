#ifndef _ARRAY_H
#define _ARRAY_H

#include <stdlib.h>
#include <mem.h>


#define ARRAY_PART_COUNT 100

typedef struct {
	int length;
	void* _data;
	int current;
	int _alloc;
	int _item_size;
} _Array;
typedef _Array *Array;

Array newArray(int item_size){
	Array array = malloc(sizeof(_Array));
	array->_item_size = item_size;
	array->_alloc = 0;
	array->length = 0;
	array->current = 0;
	return array;
}

int ArrayPush(Array array, void *data){
	if(array->length >= array->_alloc){
		array->_alloc += ARRAY_PART_COUNT;
		void* newMemory = malloc(array->_item_size*array->_alloc);
		if(array->length>0){
			memcpy(newMemory, array->_data, array->_item_size*array->length);
			free(array->_data);
		}
		array->_data = newMemory;
	}
	memcpy( array->_data +(array->_item_size*array->length++), data, array->_item_size);
	return array->length;
}

void ArrayReset(Array array){
	array->current = 0;
}
void* ArrayNext(Array array){
	if(array->current < array->length){
		return array->_data + (array->_item_size*array->current++);
	}else{
		return 0;
	}
}

void* ArrayGet(Array array, int index){
	return array->_data + (index*array->_item_size);
}


void ArrayFree(Array array){
	if(array->length>0){
		free(array->_data);
	}
	free(array);
}


unsigned int hashString(char* str){
	unsigned int res = 0;
	while(*str){
		res = (res<<5) + res + *str++;
	}
	return res;
}


#endif