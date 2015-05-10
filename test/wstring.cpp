#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

int 
main(){
	string s = "雷亞遊戲";
	printf("%s: %d\n",s.c_str(), sizeof(s.c_str()));
	//cout << s << endl;
	return 0;
}