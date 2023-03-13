#include "myTerm.h"
#include <stdio.h>

int mt_clrscr() {
    printf("\033[2J");
    printf("\033[H");
    return 0;
}

int mt_gotoXY(int x, int y) {
    printf("\033[%d;%dH", x, y);
    return 0;
}

int mt_getscreensize(int* rows, int* cols) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0)
        return -1;
    *rows = w.ws_row;
    *cols = w.ws_col;
    return 0;
}

int mt_setfgcolor(enum colors color) {
    printf("\033[%dm", color);
    return 0;
}

int mt_setbgcolor(enum colors color) {
    printf("\033[%dm", color + 10);
    return 0;
}

int main() {
    mt_clrscr();
    int rows = 0, cols = 0;
    mt_getscreensize(&rows, &cols);
    mt_setbgcolor(BLACK);
    putchar(' ');
    mt_setbgcolor(PURPLE);
    putchar(' ');

    mt_setfgcolor(YELLOW);
    mt_gotoXY(1, 1);
    printf(" Memory     ");

    mt_setfgcolor(WHITE);
    mt_gotoXY(2, 2);
    printf(" Value       ");

    mt_setfgcolor(YELLOW);
    mt_gotoXY(3, 1);
    printf(" AX  ");
    mt_gotoXY(4, 1);
    printf(" BX  ");
    mt_gotoXY(5, 1);
    printf(" CX  ");
    mt_gotoXY(6, 1);
    printf(" DX  ");

    mt_gotoXY(3, 15);
    printf("0x00  ");
    mt_gotoXY(4, 15);
    printf("0x00  ");
    mt_gotoXY(5, 15);
    printf("0x00  ");
    mt_gotoXY(6, 15);
    printf("0x00  ");

    mt_gotoXY(8, 1);
    printf("Function keys:");

    mt_gotoXY(9, 1);
    printf("F1  ");
    mt_gotoXY(10, 1);
    printf("F2  ");
    mt_gotoXY(11, 1);
    printf("F3  ");
    mt_gotoXY(12, 1);
    printf("F4  ");
    mt_gotoXY(13, 1);
    printf("F5  ");

    mt_gotoXY(9, 15);
    printf("Load  ");
    mt_gotoXY(10, 15);
    printf("Save  ");
    mt_gotoXY(11, 15);
    printf("Run   ");
    mt_gotoXY(12, 15);
    printf("Step  ");
    mt_gotoXY(13, 15);
    printf("Reset ");

    return 0;
}