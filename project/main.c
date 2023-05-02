#include <myInterface.h>
#include <myReadKey.h>
#include <mySimpleComputer.h>
#include <myTerm.h>
bool on = 1;

int
main ()
{
  int counter = 0x35;
  bc_initfont ("mybch.font");
  rk_myTermSave ();
  sc_memoryInit ();
  for (int i = 0; i < RAM_SIZE; i++)
    sc_memorySet (i, i + 1);
  mi_uiInit (counter);
  mi_uiUpdate ();
  enum keys key;
  do
    {
      mi_uiUpdate ();
      rk_readKey (&key);
      switch (key)
        {
        case UP_KEY:
          (instruction_counter <= 9)
              ? (instruction_counter = 90 + instruction_counter)
              : (instruction_counter -= 10);
          break;
        case RIGHT_KEY:
          (!((instruction_counter + 1) % 10)) ? (instruction_counter -= 9)
                                              : (instruction_counter += 1);
          break;
        case DOWN_KEY:
          (instruction_counter >= 90)
              ? (instruction_counter = instruction_counter - 90)
              : (instruction_counter += 10);
          break;
        case LEFT_KEY:
          (!(instruction_counter % 10)) ? (instruction_counter += 9)
                                        : (instruction_counter -= 1);
          break;

        case L_KEY:
          sc_memoryLoad ("memory.bin");
          break;
        case S_KEY:
          sc_memorySave ("memory.bin");
          break;

        case R_KEY:
          break;
        case T_KEY:
          break;
        case I_KEY:
          break;
        case F5_KEY:
          break;

        case F6_KEY:
          mi_Counter ();
          break;

        case ENTER_KEY:
          mi_uisetValue ();
          mi_uiUpdate ();
          break;
        case ESC_KEY:
          on = 0;
          break;
        }
    }
  while (on);
  return 0;
}