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
          if ((i * 10 + j) == currMemCell)
            mt_setbgcolor (GREEN);
          else
            mt_setdfcolor ();
          int command, operand;
          sc_commandDecode (value, &command, &operand);
          char buffer[7];
          int length
              = sprintf (buffer, "%c%02X%02X", (value >> 14) ? '-' : '+',
                         abs ((value >> 7) & 0x7F), abs (value & 0x7F));
          write (STDOUT_FILENO, buffer, length);
        }
    }
  mt_setdfcolor ();
  return 0;
}

int
mi_displayAccumulator ()
{
  char buf[6];
  mt_gotoXY (70, 2);
  if ((accumulator >> 14) & 0x1)
    sprintf (buf, "-%04X", accumulator & 0x3fff);
  else
    sprintf (buf, "+%04X", accumulator & 0x3fff);
  write (STDOUT_FILENO, buf, 5);
  // char buff[32];
  // accumulator < 0 ? sprintf(buff, "-%04d", accumulator * -1)
  //                 : sprintf(buff, "+%04d", accumulator);
  // write(STDOUT_FILENO, buff, 5);

  return 0;
}

int
mi_displayInstructionCounter ()
{
  char buffer[6];
  mt_gotoXY (70, 5);
  sprintf (buffer, "+%04d", instruction_counter);
  // write (STDOUT_FILENO, buff, 5);
  write (STDOUT_FILENO, buffer, sizeof (buffer));
  return (0);
}

int
mi_displayOperation ()
{
  int value = 0, command = 0, operand = 0;
  if (sc_memoryGet (instruction_counter, &value))
    return -1;
  if (sc_commandDecode (value, &command, &operand))
    {
      value = 0;
      command = 0;
      operand = 0;
    }
  mt_gotoXY (68, 8);
  char buf[32];
  char sign = (value != 0) ? '+' : '-';
  sprintf (buf, " %c%02X : %02X", sign, command, operand);
  write (STDOUT_FILENO, buf, strlen (buf));
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
        mt_setfgcolor (RED);
      else
        mt_setfgcolor (NONACTIVE);
      mt_gotoXY (68 + (i * 2), 11);
      char buffer[2];
      int length = sprintf (buffer, "%c", flags[i]);
      write (STDOUT_FILENO, buffer, length);
    }
  mt_setdfcolor ();
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
  mt_gotoXY (66, 1);
  write (STDOUT_FILENO, " accumulator ", strlen (" accumulator "));
  mt_gotoXY (63, 4);
  write (STDOUT_FILENO, " instructionCounter ",
         strlen (" instructionCounter "));
  mt_gotoXY (67, 7);
  write (STDOUT_FILENO, " Operation ", strlen (" Operation "));
  mt_gotoXY (68, 10);
  write (STDOUT_FILENO, " Flags ", strlen (" Flags "));
  mt_gotoXY (54, 13);
  write (STDOUT_FILENO, " Keys: ", strlen (" Keys: "));
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
    }
  mt_gotoXY (1, 24);
  return 0;
}

int
mi_displayBigchars ()
{
  int value, command, operand;
  short ind;
  if (sc_memoryGet (currMemCell, &value))
    return -1;

  if (!(value >> 14))
    {
      ind = 16;
      bc_printbigchar (&font[ind * 2], 2, 14, GREEN, 0);
    }
  else
    {
      ind = 17;
      bc_printbigchar (&font[ind * 2], 2, 14, GREEN, 0);
    }
  sc_commandDecode (value & 0x3FFF, &command, &operand);
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
mi_Counter ()
{
  char buffer[32];
  write (1, "\033[2K", 4);
  mt_gotoXY (1, 24);
  write (STDOUT_FILENO, "Enter i_c: ", strlen ("Enter i_c: "));
  fgets (buffer, 32, stdin);
  instruction_counter = atoi (buffer);
  if (instruction_counter < 0 || instruction_counter > 99)
    {
      instruction_counter = 0;
      mt_clrscr ();
      write (STDOUT_FILENO, "\033[38;5;196mError\n", 17);
      return 1;
    }
  mi_displayInstructionCounter ();
  return 0;
}

int
mi_uiInit ()
{
  currMemCell = 0;
  instruction_counter = 0;
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
  return 0;
}

int
mi_uiUpdate ()
{
  mi_displayTexts ();
  mi_displayAccumulator ();
  mi_displayInstructionCounter ();
  mi_displayOperation ();
  mi_displayFlags ();
  mi_displayMemoryValues ();
  mi_displayBigchars ();
  mt_gotoXY (1, 24);
  write (1, "\033[2K", 4);
  write (STDOUT_FILENO, "Input/Output: ", strlen ("Input/Output: "));

  return 0;
}

int
mi_uisetValue ()
{

  char buffer[10];
  fgets (buffer, 10, stdin);
  mt_gotoXY (8, 24);
  write (1, "\033[2K", 4);

  if (buffer[strlen (buffer) - 1] != '\n')
    mi_clearBuffIn ();

  if (!mi_checkCorrectInput (buffer))
    {
      mt_gotoXY (1, 24);
      mi_messageOutput ((char *)"Invalid input", ERROR);
      return -1;
    }

  long int number = 0;
  char *tmp;

  if (buffer[0] == '+')
    {
      number = strtol (&buffer[1], &tmp, 16);
      if (number > 0x3FFF)
        {
          mi_messageOutput (
              (char *)"The command value must not exceed 14 bits (0x3FFF)",
              RED);
          return -1;
        }
      sc_memorySet (currMemCell, number);
    }
  else if (buffer[0] != '-')
    {
      number = strtol (buffer, &tmp, 16);
      if (number > 0x3FFF)
        {
          mi_messageOutput (
              (char *)"The command value must not exceed 14 bits (0x3FFF)",
              RED);
          return -1;
        }
      sc_memorySet (currMemCell, number);
    }
  else if (buffer[0] == '-')
    {
      number = strtol (&buffer[1], &tmp, 16);
      if (number > 0x3FFF)
        {
          mi_messageOutput (
              (char *)"The command value must not exceed 14 bits (0x3FFF)",
              RED);
          return -1;
        }
      number = number | 0x4000;
      sc_memorySet (currMemCell, number);
    }

  return 0;
}

bool
mi_checkCorrectInput (const char buffer[10])
{
  int i;
  if (buffer[0] == '+' || buffer[0] == '-')
    {
      if (strlen (buffer) == 2 || strlen (buffer) > 6)
        return false;
      i = 1;
    }
  else
    {
      i = 0;
      if (strlen (buffer) == 1 || strlen (buffer) > 5)
        return false;
    }
  for (; i < strlen (buffer) - 1; ++i)
    if (!(isxdigit (buffer[i])))
      return false;

  return true;
}

int
mi_messageOutput (char *str, enum colors color)
{

  char buff[100] = { 0 };
  sprintf (buff, "\033[38;5;%dm%s\033[0m", color, str);
  write (STDOUT_FILENO, buff, 100);
  color == ERROR ? sleep (1) : sleep (2);
  write (1, "\033[2K", 4);
  return 0;
}

int
mi_clearBuffIn ()
{
  int c;
  do
    {
      c = getchar ();
    }
  while (c != '\n' && c != '\0');
  return 0;
}

int
mi_hidecursor ()
{
  write (1, "\E[?25l\E[?1c", 8);
  return 0;
}

int
mi_showcursor ()
{
  write (1, "\E[?25h\E?8c", 8);
  return 0;
}

int
currentToCounterSet ()
{
  char buf[6];
  int value = 0, op = 0, com = 0;
  sc_memoryGet (currMemCell, &value);
  mt_gotoXY (currMemCell / 10 + 2, currMemCell % 10 * 6 + 2);
  sc_commandDecode (value & 0x3fff, &com, &op);
  if ((value >> 14) & 1)
    sprintf (buf, "-%02X%02X", com, op);
  else
    sprintf (buf, "+%02X%02X", com, op);
  write (1, buf, 5);
  currMemCell = instruction_counter;
  sc_memoryGet (currMemCell, &value);
  // newNumPrint (0x8fff);
  return 0;
}
