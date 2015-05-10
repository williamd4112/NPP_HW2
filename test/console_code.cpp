#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <cstring>

using namespace std;

void SetCursorPos(int XPos, int YPos)
{
 	printf("\033[%d;%dH", YPos+1, XPos+1);
}

int
main(int argc, char *argv[]){
	string s = "\x09";
	printf("0x%x\n",s[0]);
	//char s[] = "\x01";
	//printf("%d\n",s[0]);
	//system("clear");
	//SetCursorPos(10,5);
	//printf("%s\n",argv[0]);
	//putchar(0x07);
	//while(1);
	return 0;
}
