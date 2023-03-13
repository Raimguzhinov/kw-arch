#include <mySimpleComputer.h>
#include <myTerm.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 24
#define COLUMNS 80

#define MEMORY_X 1
#define MEMORY_Y 1
#define MEMORY_WIDTH 61
#define MEMORY_HEIGHT 12

#define ACC_X 62
#define ACC_Y 1
#define ACC_WIDTH 22
#define ACC_HEIGHT 3

#define COUNTER_X 62
#define COUNTER_Y 4
#define COUNTER_WIDTH 22
#define COUNTER_HEIGHT 3

#define OP_X 62
#define OP_Y 7
#define OP_WIDTH 22
#define OP_HEIGHT 3

#define FLAGS_X 62
#define FLAGS_Y 10
#define FLAGS_WIDTH 22
#define FLAGS_HEIGHT 3

#define BIG_CHARS_X 1
#define BIG_CHARS_Y 13
#define BIG_CHARS_WIDTH 52
#define BIG_CHARS_HEIGHT 10

#define KEYS_X 53
#define KEYS_Y 13
#define KEYS_WIDTH 31
#define KEYS_HEIGHT 10

#define BOX(x, y, w, h)                                                       \
  do                                                                          \
    {                                                                         \
      if (print_box (x, y, w, h))                                             \
        return -1;                                                            \
    }                                                                         \
  while (0)

#define TEXT(x, y, str)                                                       \
  do                                                                          \
    {                                                                         \
      mt_gotoXY (x, y);                                                       \
      printf ("%s", str);                                                     \
    }                                                                         \
  while (0)

#define HOTKEY(x, y, k)                                                       \
  do                                                                          \
    {                                                                         \
      mt_gotoXY (x, y);                                                       \
      printf ("%s", k);                                                       \
    }                                                                         \
  while (0)

/*Если старший бит (>> 14) установлен, то это означает, что значение ячейки памяти отрицательное, и его нужно привести к дополнительному коду, обнулив старший бит и инвертировав все остальные биты. Если старший бит не установлен, то значение положительное и выводится с символом + перед ним.*/
#define MEMORY_VALUES                                                         \
  do                                                                          \
    {                                                                         \
      for (int i = 0; i < 10; ++i)                                            \
        {                                                                     \
          for (int j = 0; j < 10; ++j)                                        \
            {                                                                 \
              mt_gotoXY (2 + (5 * j + j), 2 + i);                             \
              int value;                                                      \
              sc_memoryGet (i * 10 + j, &value);                              \
              if ((i * 10 + j) == instruction_counter)                        \
                {                                                             \
                  mt_setbgcolor (GREEN);                                      \
                }                                                             \
              if ((value >> 14) & 1)                                          \
                {                                                             \
                  printf (" %04X", value &(~(1 << 14)));                      \
                }                                                             \
              else                                                            \
                {                                                             \
                  printf ("+%04X", value);                                    \
                }                                                             \
              mt_setdfcolor ();                                               \
            }                                                                 \
        }                                                                     \
    }                                                                         \
  while (0)

#define FLAGS                                                                 \
  do                                                                          \
    {                                                                         \
      char flags[] = { 'P', '0', 'M', 'T', 'E' };                             \
      for (int i = 0; i < REG_COUNT; i++)                                     \
        {                                                                     \
          int value;                                                          \
          sc_regGet (i, &value);                                              \
          mt_gotoXY (68 + (i * 2), 11);                                       \
          printf ("%c", flags[i]);                                            \
        }                                                                     \
      mt_gotoXY (68, 8);                                                      \
      printf ("+00 : 00");                                                    \
      mt_gotoXY (70, 5);                                                      \
      printf ("+0000");                                                       \
      mt_gotoXY (70, 2);                                                      \
      printf ("+0001");                                                       \
    }                                                                         \
  while (0)

int
print_box (int x, int y, int width, int height)
{
  int count_rows, count_columns;
  mt_getscreensize (&count_rows, &count_columns);
  if (count_rows < 20 || count_columns < 20)
    return -1;
  mt_gotoXY (x, y);
  mt_printsymbol (UL_CORNER);
  mt_gotoXY (x + width - 1, y);
  mt_printsymbol (UR_CORNER);
  mt_gotoXY (x + width - 1, y + height - 1);
  mt_printsymbol (DR_CORNER);
  mt_gotoXY (x, y + height - 1);
  mt_printsymbol (DL_CORNER);
  /* линии вверх-низ */
  for (int i = 1; i < width - 1; ++i)
    {
      /* верхние линии */
      mt_gotoXY (x + i, y);
      mt_printsymbol (H_LINE);
      /* нижние линии*/
      mt_gotoXY (x + i, y + height - 1);
      mt_printsymbol (H_LINE);
    }
  /* боковые линии */
  for (int i = 1; i < height - 1; ++i)
    {
      /* лево */
      mt_gotoXY (x, y + i);
      mt_printsymbol (V_LINE);
      /* право */
      mt_gotoXY (x + width - 1, y + i);
      mt_printsymbol (V_LINE);
    }
  return 0;
}

int
drawing_boxes ()
{
  BOX (MEMORY_X, MEMORY_Y, MEMORY_WIDTH, MEMORY_HEIGHT);
  BOX (ACC_X, ACC_Y, ACC_WIDTH, ACC_HEIGHT);
  BOX (COUNTER_X, COUNTER_Y, COUNTER_WIDTH, COUNTER_HEIGHT);
  BOX (OP_X, OP_Y, OP_WIDTH, OP_HEIGHT);
  BOX (FLAGS_X, FLAGS_Y, FLAGS_WIDTH, FLAGS_HEIGHT);
  BOX (BIG_CHARS_X, BIG_CHARS_Y, BIG_CHARS_WIDTH, BIG_CHARS_HEIGHT);
  BOX (KEYS_X, KEYS_Y, KEYS_WIDTH, KEYS_HEIGHT);
  return 0;
}

int
drawing_texts ()
{
  TEXT (27, 1, " Memory ");
  TEXT (66, 1, " accumulator ");
  TEXT (63, 4, " instructionCounter ");
  TEXT (67, 7, " Operation ");
  TEXT (68, 10, " Flags ");
  TEXT (54, 13, " Keys: ");
  char *hot_keys[7] = { (char *)" l  – load",
                        (char *)" s  – save",
                        (char *)" r  – run",
                        (char *)" t  – step",
                        (char *)" i  – reset",
                        (char *)" F5 – accumulator",
                        (char *)" F6 – instructionCounter" };
  for (int i = 0; i < 7; i++)
    {
      HOTKEY (54, i + 14, hot_keys[i]);
    }
  TEXT (1, 24, "Input/Output: ");
  return 0;
}

int
drawing_memory ()
{
  MEMORY_VALUES;
  return 0;
}

int
drawing_flags ()
{
  FLAGS;
  return 0;
}

int
ui_initialise (int counter)
{
  instruction_counter = counter;
  mt_clrscr ();
  drawing_boxes ();
  drawing_texts ();
  drawing_memory ();
  drawing_flags ();
  mt_gotoXY (15, 24);
  getchar_unlocked ();
  return 0;
}

int
main ()
{
  int counter = 0x35;
  sc_memoryInit ();
  sc_memorySet (counter, 0x9999);
  ui_initialise (counter);

  return 0;
}