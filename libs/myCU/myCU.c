#include "myCU.h"

int
mc_read (int operand)
{
  rk_myTermRestore ();
  mi_showCursor ();
  char buf[10];
  bool corrent_input = true;
  mt_gotoXY (1, 24);
  write (STDOUT_FILENO, "\033[2K", 4);
  write (STDOUT_FILENO, "Input/Output: ", strlen ("Input/Output: "));
  mt_gotoXY (1, 25);
  int n = sprintf (buf, "%X< ", operand);
  write (STDOUT_FILENO, buf, n);
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
mc_write (int operand)
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
mc_load (int operand)
{
  int value = 0;
  if (sc_memoryGet (operand, &value))
    return -1;
  accumulator = value;
  return 0;
}

int
mc_store (int operand)
{
  if (sc_memorySet (operand, accumulator))
    return -1;
  return 0;
}

int
mc_jump (int operand)
{
  if (operand > 99)
    {
      sc_regSet (FLAG_M, 1);
      return -1;
    }
  instruction_counter = operand;
  CU ();
  return 0;
}

int
mc_jneg (int operand)
{
  if ((accumulator >> 14) == 0 || (accumulator & 0x3fff) == 0)
    {
      instruction_counter++;
      return 0;
    }
  if (operand > 99)
    {
      sc_regSet (FLAG_M, 1);
      return -1;
    }
  instruction_counter = operand;
  CU ();
  return 0;
}

int
mc_jz (int operand)
{
  if (accumulator & 0x3fff)
    {
      instruction_counter++;
      return 0;
    }
  if (operand > 99)
    {
      sc_regSet (FLAG_M, 1);
      return -1;
    }
  instruction_counter = operand;
  CU ();
  return 0;
}

int
mc_halt ()
{
  sc_regSet (FLAG_T, 1);
  return 0;
}

int
mc_loglc (int operand)
{
  int value = 0;
  if (sc_memoryGet (operand, &value))
    {
      return -1;
    }
  int shift = accumulator & 0x3fff;
  value <<= shift;
  if (sc_memorySet (operand, value))
    {
      return -1;
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
        case READ:
          mc_read (operand);
          return READ;
          break;
        case WRITE:
          mc_write (operand);
          return WRITE;
          break;
        case LOAD:
          mc_load (operand);
          return LOAD;
          break;
        case STORE:
          mc_store (operand);
          return STORE;
          break;
        case JUMP:
          mc_jump (operand);
          return JUMP;
          break;
        case JNEG:
          mc_jneg (operand);
          return JNEG;
          break;
        case JZ:
          mc_jz (operand);
          return JZ;
          break;
        case HALT:
          mc_halt ();
          sc_halt = true;
          return HALT;
          break;
        case LOGLC:
          mc_loglc (operand);
          return LOGLC;
          break;
        default:
          sc_regSet (FLAG_E, 1);
          return -2;
          break;
        }
    }
  return 0;
}

void
mc_oneTactPulse ()
{
  sc_halt = false;
  currMemCell = instruction_counter;
  mi_uiUpdate (sc_halt);
  sc_regSet (FLAG_T, 0);
  int cu_result = CU ();
  int value;
  sc_regGet (FLAG_T, &value);
  if (!(cu_result == 40 || cu_result == 41 || cu_result == 42
        || cu_result == -2))
    {
      sc_regSet (FLAG_E, 0);
      if ((instruction_counter >= 0 && instruction_counter <= 99) && !value)
        {
          if (instruction_counter != 99)
            {
              instruction_counter++;
            }
          else
            instruction_counter = 0;
        }
    }
  sc_regSet (FLAG_T, 1);
}