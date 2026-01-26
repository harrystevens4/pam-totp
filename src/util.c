#include <stdlib.h>
#include <string.h>

//returns a pointer to string
char *str_remove_predicate(char *string, int (*predicate)(int)){
	for (char *index = string; index < strlen(string)+string;){
		if (predicate(*index)){
			//move the rest of the string one char back to replace ch
			memmove(index,index+1,strlen(index));
		}else {
			index++;
		}
	}
	return string;
}

