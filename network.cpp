#include "network.h"

bool operator < (SIN a, SIN b){
	if(a.sin_addr.s_addr == b.sin_addr.s_addr)
		return a.sin_port < b.sin_port;
	return a.sin_addr.s_addr < b.sin_addr.s_addr;
}

char getch(){
    int i;
    char ch;
    struct termios _old, _new;

    tcgetattr (0, &_old);
    memcpy (&_new, &_old, sizeof (struct termios));
    _new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr (0, TCSANOW, &_new);
    i = read (0, &ch, 1);
    tcsetattr (0, TCSANOW, &_old);
    if (i == 1)/* ch is the character to use */ 
        return ch;
    else/* there was some problem; complain, return error, whatever */ 
        printf("error!n");;
    return 0;
}