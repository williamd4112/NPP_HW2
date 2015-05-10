#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stack>

using namespace std;

int main(){
	fstream fs("io.txt", ios::in);
	string line;
	stack<string> st;
	while(getline(fs, line) != NULL){
		cout << line << endl;
		st.push(line);
	}
	fs.close();

	fs.open("io.txt",ios::out | ios::trunc);
	while(!st.empty()){
		fs << st.top() << endl;
		st.pop();
	}
	fs.close();

	return 0;
}