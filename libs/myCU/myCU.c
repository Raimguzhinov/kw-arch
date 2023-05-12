#include "myCU.h"

int
READ (int operand)
{
  rk_myTermRestore ();
  mi_showcursor ();
  char buf[10];
  bool corrent_input = true;
  mt_gotoXY (1, 25);
  write (1, "\r\E[K", 4);
  int n = sprintf (buf, "%X< ", operand);
  write (1, buf, n);
  n = read (0, buf, 10);
  buf[n - 1] = '\0';
  for (int i = 1; i < n - 1; ++i)
    {
      if ((buf[i] < '0' || buf[i] > '9') && (buf[i] < 'a' || buf[i] > 'f')
          && (buf[i] < 'A' || buf[i] > 'F'))
        {
          return -1;
        }
    }
  int number = 0;
  char *pEnd;
  if (buf[0] == '+' || buf[0] == '-')
    {
      number = strtol (&buf[1], &pEnd, 16);
      if (number > 0x3fff)
        corrent_input = false;
      if (buf[0] == '-')
        number |= (0x1 << 14);
      if (sc_memorySet (operand, number & 0x7fff))
        return -1;
      // printMemoryOne (operand);
      if (!corrent_input)
        return -1;
    }
  else
    return -1;
  rk_myTermRegime (0, 30, 0, 0, 0);
  return 0;
}

int
WRITE (int operand)
{
  char buf[6];
  int value;
  mt_gotoXY (1, 26);
  write (1, "\r\E[K", 4);
  int n = sprintf (buf, "%X> ", operand);
  write (1, buf, n);
  if (sc_memoryGet (operand, &value))
    return -1;
  n = value >> 14 ? sprintf (buf, "-%04X", value & 0x3FFF)
                  : sprintf (buf, "+%04X", value & 0x3FFF);
  write (1, buf, n);
  return 0;
}

int
LOAD (int operand)
{
  int value = 0;
  sc_memoryGet (operand, &value);
  accumulator = value;
  return 0;
}

int
STORE (int operand)
{
  sc_memorySet (operand, accumulator);
  return 0;
}

int
JUMP (int operand)
{
  instruction_counter = operand;
  CU ();
  return 0;
}

int
JNEG (int operand)
{
  if (accumulator < 0)
    {
      instruction_counter = operand;
      CU ();
    }
  return 0;
}

int
JZ (int operand)
{
  if (accumulator == 0)
    {
      instruction_counter = operand;
      CU ();
    }
  return 0;
}

int
MOVR (int operand)
{
  int value;
  sc_memoryGet (accumulator, &value);
  sc_memorySet (operand, value);
  return 0;
}

int
HALT ()
{
  sc_regSet (FLAG_T, 1);
  return 0;
}

int
JNS (int operand)
{
  if (accumulator > 0)
    {
      instruction_counter = operand;
    }
  return 0;
}

int
CU ()
{
  int value = 0;
  sc_memoryGet (instruction_counter, &value);
  int command, operand;
  if (sc_commandDecode (value & 0x3FFF, &command, &operand))
    {
      sc_regSet (FLAG_E, 1);
      return -1;
    }

  if (((command >= 0x30) && (command <= 0x33)) || (command == 0x52))
    {
      ALU (command, operand);
    }
  else
    {
      switch (command)
        {
        case 0x10:
          READ (operand);
          break;
        case 0x11:
          WRITE (operand);
          break;
        case 0x20:
          LOAD (operand);
          break;
        case 0x21:
          STORE (operand);
          break;
        case 0x40:
          JUMP (operand);
          break;
        case 0x41:
          JNEG (operand);
          break;
        case 0x42:
          JZ (operand);
          break;
        case 0x43:
          HALT ();
          break;
        case 0x72:
          MOVR (operand);
          break;
        case 0x55:
          JNS (operand);
          break;
        }
    }
  return 0;
}