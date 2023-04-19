#include "myInterface.h"

int
mi_displayMemoryValues ()
{
  for (int i = 0; i < 10; ++i)
    {
      for (int j = 0; j < 10; ++j)
        {
          mt_gotoXY (2 + (5 * j + j), 2 + i);
          int value;
          sc_memoryGet (i * 10 + j, &value);
          if ((i * 10 + j) == instruction_counter)
            {
              mt_setbgcolor (GREEN);
            }
          int command, operand;
          sc_commandDecode (value, &command, &operand);
          char buffer[7];
          int length
              = sprintf (buffer, "%c%02X%02X", (value >> 14) ? '-' : '+',
                         abs ((value >> 7) & 0x7F), abs (value & 0x7F));
          write (STDOUT_FILENO, buffer, length);
          mt_setdfcolor ();
        }
    }
  return 0;
}

int
mi_displayAccumulator ()
{
  char buffer[5] = "+0001";
  mt_gotoXY (70, 2);
  write (STDOUT_FILENO, buffer, sizeof (buffer));
  return (0);
}

int
mi_displayInstructionCounter ()
{
  char buffer[5] = "+0000";
  mt_gotoXY (70, 5);
  write (STDOUT_FILENO, buffer, sizeof (buffer));
  return (0);
}

int
mi_displayOperation ()
{
  char buffer[8] = "+00 : 00";
  mt_gotoXY (68, 8);
  write (STDOUT_FILENO, buffer, sizeof (buffer));
  return (0);
}

int
mi_displayFlags ()
{
  char flags[] = { 'P', '0', 'M', 'T', 'E' };
  for (int i = 0; i < REG_COUNT; i++)
    {
      int value;
      sc_regGet (i + 1, &value);
      if (value == 1)
        mt_setfgcolor (DEFAULT);
      mt_gotoXY (68 + (i * 2), 11);
      char buffer[2];
      int length = sprintf (buffer, "%c", flags[i]);
      write (STDOUT_FILENO, buffer, length);
      mt_setdfcolor ();
    }
  return (0);
}

int
mi_displayBoxes ()
{
  bc_box (MEMORY_X, MEMORY_Y, MEMORY_WIDTH, MEMORY_HEIGHT);
  bc_box (ACC_X, ACC_Y, ACC_WIDTH, ACC_HEIGHT);
  bc_box (COUNTER_X, COUNTER_Y, COUNTER_WIDTH, COUNTER_HEIGHT);
  bc_box (OP_X, OP_Y, OP_WIDTH, OP_HEIGHT);
  bc_box (FLAGS_X, FLAGS_Y, FLAGS_WIDTH, FLAGS_HEIGHT);
  bc_box (BIG_CHARS_X, BIG_CHARS_Y, BIG_CHARS_WIDTH, BIG_CHARS_HEIGHT);
  bc_box (KEYS_X, KEYS_Y, KEYS_WIDTH, KEYS_HEIGHT);
  return 0;
}

int
mi_displayTexts ()
{
  mt_gotoXY (27, 1);
  write (STDOUT_FILENO, " Memory ", strlen (" Memory "));
  // TEXT (27, 1, " Memory ");
  mt_gotoXY (66, 1);
  write (STDOUT_FILENO, " accumulator ", strlen (" accumulator "));
  // TEXT (66, 1, " accumulator ");
  mt_gotoXY (63, 4);
  write (STDOUT_FILENO, " instructionCounter ",
         strlen (" instructionCounter "));
  // TEXT (63, 4, " instructionCounter ");
  mt_gotoXY (67, 7);
  write (STDOUT_FILENO, " Operation ", strlen (" Operation "));
  // TEXT (67, 7, " Operation ");
  mt_gotoXY (68, 10);
  write (STDOUT_FILENO, " Flags ", strlen (" Flags "));
  // TEXT (68, 10, " Flags ");
  mt_gotoXY (54, 13);
  write (STDOUT_FILENO, " Keys: ", strlen (" Keys: "));
  // TEXT (54, 13, " Keys: ");
  char *hot_keys[7] = { (char *)" l  – load",
                        (char *)" s  – save",
                        (char *)" r  – run",
                        (char *)" t  – step",
                        (char *)" i  – reset",
                        (char *)" F5 – accumulator",
                        (char *)" F6 – instructionCounter" };

  for (int i = 0; i < 7; i++)
    {
      mt_gotoXY (54, i + 14);
      write (STDOUT_FILENO, hot_keys[i], strlen (hot_keys[i]));
      // HOTKEY (54, i + 14, hot_keys[i]);
    }
  mt_gotoXY (1, 24);
  write (STDOUT_FILENO, "Input/Output: ", strlen ("Input/Output: "));
  // TEXT (1, 24, "Input/Output: ");
  return 0;
}

int
mi_displayBigchars ()
{
  int value, command, operand;
  short ind;
  sc_memoryGet (instruction_counter, &value);
  if (sc_memoryGet (instruction_counter, &value) < 0)
    {
      return -1;
    }
  if (sc_commandDecode (value & 0x3FFF, &command, &operand) < 0)
    {
      return -2;
    }

  if (!(value & 0x4000))
    {
      ind = 16;
      bc_printbigchar (&font[ind * 2], 2, 14, GREEN, 0);
    }

  else
    {
      ind = 17;
      bc_printbigchar (&font[ind * 2], 2, 14, GREEN, 0);
    }

  int ch;
  for (int i = 0; i < 4; ++i)
    {
      switch (i)
        {
        case 0:
          ch = (command >> 4) & 0xF;
          break;
        case 1:
          ch = (command)&0xF;
          break;
        case 2:
          ch = (operand >> 4) & 0xF;
          break;
        case 3:
          ch = (operand)&0xF;
          break;
        }
      bc_printbigchar (&font[ch * 2], 2 + 8 * (i + 1) + 2 * (i + 1), 14, GREEN,
                       0);
    }

  return 0;
}

int
mi_uiInit (int counter)
{
  instruction_counter = counter;
  int count_rows, count_columns;
  mt_getscreensize (&count_rows, &count_columns);
  if (count_rows < 30 || count_columns < 30)
    {
      printf ("\nмаленький размер окна!!!");
      return -1;
    }
  mt_clrscr ();
  mi_displayBoxes ();
  mi_displayTexts ();
  mi_displayAccumulator ();
  mi_displayInstructionCounter ();
  mi_displayOperation ();
  mi_displayFlags ();
  mi_displayMemoryValues ();
  mi_displayBigchars ();
  mt_gotoXY (15, 24);
  getchar_unlocked ();
  return 0;
}