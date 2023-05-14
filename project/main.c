#include <myInterface.h>
#include <myReadKey.h>
#include <mySignal.h>
#include <mySimpleComputer.h>
#include <myTerm.h>

bool on = 1;

int
main ()
{
  bc_initfont ("mybch.font");
  rk_myTermSave ();
  sc_memoryInit ();
  for (int i = 0; i < RAM_SIZE; i++)
    sc_memorySet (i, i + 1);
  mi_uiInit ();
  mi_uiUpdate ();

  struct itimerval nval, oval;
  ms_setTimer (&nval, &oval);
  ms_setSignals ();

  enum keys key;
  do
    {
      mi_uiUpdate ();
      mi_hidecursor ();

      rk_readKey (&key);
      switch (key)
        {
        case UP_KEY:
          (currMemCell <= 9) ? (currMemCell = 90 + currMemCell)
                             : (currMemCell -= 10);
          break;
        case RIGHT_KEY:
          (!((currMemCell + 1) % 10)) ? (currMemCell -= 9)
                                      : (currMemCell += 1);
          break;
        case DOWN_KEY:
          (currMemCell >= 90) ? (currMemCell = currMemCell - 90)
                              : (currMemCell += 10);
          break;
        case LEFT_KEY:
          (!(currMemCell % 10)) ? (currMemCell += 9) : (currMemCell -= 1);
          break;

        case L_KEY:
          sc_memoryLoad ("memory.bin");
          break;
        case S_KEY:
          sc_memorySave ("memory.bin");
          break;

        case R_KEY:
          sc_regInit ();
          mi_displayFlags ();
          setitimer (ITIMER_REAL, &nval, &oval);
          // raise (SIGALRM);
          break;
        case T_KEY:

          currMemCell = instruction_counter;
          mi_uiUpdate ();
          sc_regSet (FLAG_T, 0);
          int cu_result = CU ();
          int value;
          sc_regGet (FLAG_T, &value);
          if (!(cu_result == 40 || cu_result == 41 || cu_result == 42
                || cu_result == -2))
            {
              sc_regSet (FLAG_E, 0);
              if ((instruction_counter >= 0 && instruction_counter <= 99)
                  && !value)
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
          break;
        case I_KEY:
          raise (SIGUSR1);
          accumulator = 0;
          instruction_counter = 0;
          currMemCell = 0;
          mi_displayInstructionCounter ();
          sc_memoryInit ();
          sc_regSet (FLAG_P, 0);
          sc_regSet (FLAG_0, 0);
          sc_regSet (FLAG_M, 0);
          sc_regSet (FLAG_T, 1);
          sc_regSet (FLAG_E, 0);
          mi_uiUpdate ();
          break;

        case F5_KEY:
          break;

        case F6_KEY:
          mi_Counter ();
          break;

        case ENTER_KEY:
          mi_showcursor ();
          mi_uisetValue ();
          mi_uiUpdate ();
          break;

        case NOTHING_KEY:
        case INVALID_KEY:
          break;

        case ESC_KEY:
          on = 0;
          break;
        }
    }
  while (on);
  mi_showcursor ();
  return 0;
}