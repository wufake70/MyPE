#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
int a=0;
int main(){
	while(1) printf("%d\n",a++),system("pause > nul");
	MessageBox(0,0,0,0);
	return 0;
}