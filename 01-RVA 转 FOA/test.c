#include <stdio.h>
#include <stdlib.h>
int a=99;
int main() {
	system("chcp 65001 > nul");
    while(1){

		printf("current a: %d\t,",a);
		printf("current a address: %p\n",&a);
		system("pause > nul");
	}

    
    system("pause > nul");
    return 0;
}