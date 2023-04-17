// Copyright [2023] <Raimguzhinov>
#pragma once
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

extern int instruction_counter;

#define ACS_CKBOARD 'a'  // штриховка
#define ACS_ULCORNER 'l' // левый верхний угол
#define ACS_URCORNER 'k' // правый верхний угол
#define ACS_LRCORNER 'j' // правый нижний угол
#define ACS_LLCORNER 'm' // левый нижний угол
#define ACS_HLINE 'q'    // горизонтальная линия
#define ACS_VLINE 'x'    // вертикальная линия

enum colors
{
  DEFAULT,
  WHITE = 15,
  BLACK,
  RED = 196,
  BLUE = 30,
  YELLOW,
  GREEN = 34,
  PURPLE = 55,
  CYAN = 36
};

int mt_clrscr (void);
int mt_gotoXY (int x, int y);
int mt_getscreensize (int *rows, int *cols);
int mt_setfgcolor (enum colors color);
int mt_setbgcolor (enum colors color);
int mt_setdfcolor (void);
int mt_printsymbol (char symbol);
