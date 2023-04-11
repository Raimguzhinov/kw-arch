#include "mySimpleComputer.h"

int memory[RAM_SIZE]; // Модель оперативной памяти
int sc_regFlags;      // Регистр флагов

int
sc_memoryInit ()
{
  for (int i = 0; i < RAM_SIZE; i++)
    {
      memory[i] = 0;
    }
  return 0;
}

int
sc_memorySet (int address, int value)
{
  if (address < 0 || address >= RAM_SIZE)
    {
      sc_regSet (FLAG_M, 1);
      return 1;
    }
  memory[address] = value;
  return 0;
}

int
sc_memoryGet (int address, int *value)
{
  if (address < 0 || address >= RAM_SIZE)
    {
      sc_regSet (FLAG_M, 1);
      return 1;
    }
  *value = memory[address];
  return 0;
}

int
sc_memorySave (char *filename)
{
  FILE *fp;
  fp = fopen (filename, "wb");
  if (fp == NULL)
    {
      return 1;
    }
  fwrite (memory, RAM_SIZE, sizeof (RAM_SIZE), fp);
  fclose (fp);
  return 0;
}

int
sc_memoryLoad (char *filename)
{
  FILE *fp;
  fp = fopen (filename, "rb");
  if (fp == NULL)
    {
      return 1;
    }
  fread (memory, RAM_SIZE, sizeof (RAM_SIZE), fp);
  fclose (fp);
  return 0;
}

int
sc_regInit (void)
{
  sc_regFlags = 0;
  return 0;
}

int
sc_regSet (int registr, int value)
{
  if (registr > REG_COUNT || registr <= 0)
    {
      return 1;
    }
  if (value == 1)
    {
      sc_regFlags |= (1 << (registr - 1));
    }
  else if (value == 0)
    {
      sc_regFlags &= ~(1 << (registr - 1));
    }
  else
    {
      return 1;
    }
  return 0;
}

int
sc_regGet (int registr, int *value)
{
  if (registr > REG_COUNT || registr <= 0)
    {
      return 1;
    }
  *value = (sc_regFlags >> (registr - 1)) & 0x1;
  return 0;
}

int
sc_commandEncode (int command, int operand, int *value)
{
  if (CHECK_CPU_CMD (command))
    {
      *value = (command << 7) | operand;
      return 0;
    }
  return 1;
}

int
sc_commandDecode (int value, int *command, int *operand)
{
  if (((value >> 14) & 0x1) == 0)
    {
      int temp_command, temp_operand;
      temp_command = (value >> 7) & 0x7F;
      temp_operand = value & 0x7F;
      if (CHECK_CPU_CMD (temp_command))
        {
          *command = temp_command;
          *operand = temp_operand;
        }
      else
        {
          sc_regSet (FLAG_E, 1);
          return -1;
        }
    }
  else
    {
      return -1;
    }
  return 0;
}
