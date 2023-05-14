#include <myInterface.h>
#include <myReadKey.h>
#include <mySignal.h>
#include <mySimpleComputer.h>
#include <myTerm.h>

bool on = 1;

int
main ()
{
  rk_myTermSave ();
  bc_fontInit ("mybch.font");
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
      mi_hideCursor ();
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
          mi_dirMenu ();
          break;
        case S_KEY:
          mi_memorySave ();
          break;

        case R_KEY:
          sc_regInit ();
          mi_displayFlags ();
          setitimer (ITIMER_REAL, &nval, &oval);
          break;
        case T_KEY:
          mc_oneTactPulse ();
          break;
        case I_KEY:
          sc_restart ();
          mi_displayInstructionCounter ();
          mi_uiUpdate ();
          break;

        case F5_KEY:
          mi_accum ();
          break;

        case F6_KEY:
          mi_counter ();
          break;

        case ENTER_KEY:
          mi_showCursor ();
          mi_uiSetValue ();
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
  mi_showCursor ();
  return 0;
}