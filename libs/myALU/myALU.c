#include "myALU.h"

int
ALU (int command, int operand)
{
  int tmp, num, res;
  mi_showcursor ();

  // int error = 0;
  sc_memoryGet (operand, &tmp);
  switch (command)
    {
    case 0x30: // сложение, результат в акк
      if (sc_memoryGet (operand, &num))
        return -1;
      if (accumulator >> 14 == num >> 14)
        {
          res = (num & 0x3fff) + (accumulator & 0x3fff);
          if (res >> 14)
            {
              sc_regSet (FLAG_P, 1);
              // error = -1;
            }
          if (accumulator >> 14)
            res |= 0x4000;
        }
      else
        {
          if ((accumulator & 0x3fff) >= (num & 0x3fff))
            {
              res = (accumulator & 0x3fff) - (num & 0x3fff);
              if ((accumulator >> 14) && res != 0)
                res |= 0x4000;
            }
          else
            {
              res = (num & 0x3fff) - (accumulator & 0x3fff);
              if (num >> 14)
                res |= 0x4000;
            }
        }
      accumulator = res & 0x7fff;
      break;
    case 0x31: // вычитание
      if (sc_memoryGet (operand, &num))
        return -1;
      if (accumulator >> 14 == num >> 14)
        {
          if ((accumulator & 0x3fff) >= (num & 0x3fff))
            {
              res = (accumulator & 0x3fff) - (num & 0x3fff);
              if ((accumulator >> 14) && res != 0)
                res |= 0x4000;
            }
          else
            {
              res = (num & 0x3fff) - (accumulator & 0x3fff);
              if ((num >> 14) == 0 && res != 0)
                res |= 0x4000;
            }
        }
      else
        {
          res = (num & 0x3fff) + (accumulator & 0x3fff);
          if (res >> 14)
            {
              sc_regSet (FLAG_P, 1);
              // error = -1;
            }
          if (accumulator >> 14)
            res |= 0x4000;
        }
      accumulator = res & 0x7fff;
      break;
    case 0x32: // деление
      if (sc_memoryGet (operand, &num))
        return -1;
      if ((num & 0x3fff) == 0)
        {
          sc_regSet (FLAG_0, 1);
          return -1;
        }
      res = (accumulator & 0x3fff) / (num & 0x3fff);
      if (accumulator >> 14 != num >> 14)
        res |= 0x4000;
      accumulator = res & 0x7fff;
      break;
    case 0x33: // произведение
      if (sc_memoryGet (operand, &num))
        return -1;
      res = (num & 0x3fff) * (accumulator & 0x3fff);
      if (res >> 14)
        {
          sc_regSet (FLAG_P, 1);
          // error = 1;
        }
      if (accumulator >> 14 != num >> 14)
        res |= 0x4000;
      accumulator = res & 0x7fff;
      break;
    case 0x52: // логич и
      accumulator &= tmp;
      break;
    }
  return 0;
}