#include <ctype.h>

int countStringLength(char *str){
  int c = 0;

  while(*str != '\0'){
    c += 1;
    str++;
  }
  return c;
}

size_t countStrLen(char *str){
  size_t c = 0;

  while(*str != '\0'){
    c += 1;
    str++;
  }
  return c;
}

void printData(char *str, size_t numBytes){
	for(int i = 0; i < numBytes; i++){
		printf("%c", str[i]);
	}
	printf("\n");
}

void convertToUpperCase(char *str, size_t numBytes){
	for(int i = 0; i < numBytes; i++){
		str[i] = toupper(str[i]);
	}
}
