#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define UL_CORNER 'l' // левый верхний угол
#define UR_CORNER 'k' // правый верхний угол
#define DR_CORNER 'j' // правый нижний угол
#define DL_CORNER 'm' // левый нижний угол
#define H_LINE 'q'    // горизонтальная линия
#define V_LINE 'x'    // вертикальная линия
#define FILL 'a'      // штриховка

enum colors
{
  WHITE = 15,
  BLACK,
  RED = 196,
  BLUE = 30,
  YELLOW,
  GREEN = 34,
  PURPLE,
  CYAN = 36
};

extern int instruction_counter;
int mt_clrscr (void);
int mt_gotoXY (int x, int y);
int mt_getscreensize (int *rows, int *cols);
int mt_setfgcolor (enum colors color);
int mt_setbgcolor (enum colors color);
int mt_setdfcolor (void);
int mt_printsymbol (char symbol);
