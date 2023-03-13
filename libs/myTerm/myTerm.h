#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

enum colors {BLACK = 30, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE};

int mt_clrscr();
int mt_gotoXY(int x, int y);
int mt_getscreensize(int* rows, int* cols);
int mt_setfgcolor(enum colors color);
int mt_setbgcolor(enum colors color);