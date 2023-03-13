#include "myTerm.h"
#include <stdio.h>

int instruction_counter;

int
mt_clrscr ()
{
  printf ("\E[H\E[2J");
  return 0;
}

int
mt_gotoXY (int x, int y)
{
  int count_rows, count_columns;

  if (mt_getscreensize (&count_rows, &count_columns) == -1)
    {
      return -1;
    }

  if ((x > count_columns) || (x <= 0) || (y > count_rows) || (y <= 0))
    {
      return -1;
    }

  printf ("\033[%d;%dH", y, x);

  return 0;
}

int
mt_getscreensize (int *rows, int *columns)
{
  struct winsize window_size;

  if (ioctl (1, TIOCGWINSZ, &window_size))
    {
      return -1;
    }

  *rows = window_size.ws_row;
  *columns = window_size.ws_col;

  return 0;
}

int
mt_setfgcolor (enum colors color)
{
  printf ("\033[38;5;%dm", color);
  return 0;
}

int
mt_setbgcolor (enum colors color)
{
  printf ("\033[48;5;%dm", color);
  return 0;
}

int
mt_setdfcolor ()
{
  printf ("\033[0m");
  return 0;
}

int
mt_printsymbol (char symbol)
{
  printf ("\033(0%c\033(B", symbol);
  return 0;
}