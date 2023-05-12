#include "mySignal.h"

void
ms_setTimer (struct itimerval *nval, struct itimerval *oval)
{
  nval->it_interval.tv_sec = 1;
  nval->it_interval.tv_usec = 0;
  nval->it_value.tv_sec = 1;
  nval->it_value.tv_usec = 0;
}

void
ms_setSignals ()
{
  signal (SIGALRM, ms_timerHandler);
  signal (SIGUSR1, ms_userSignal);
}

void
ms_timerHandler (int sig)
{
  CU ();
  int value;
  sc_regGet (FLAG_T, &value);
  if ((instruction_counter >= 0 && instruction_counter <= 99) && !value)
    {
      if (instruction_counter != 99)
        {
          instruction_counter++;
        }
      else
        instruction_counter = 0;
    }
  mi_uiUpdate ();
  if (!value)
    {
      ualarm (100000, 0);
    }
  currMemCell = instruction_counter;
}

void
ms_userSignal (int sig)
{
  alarm (0);
  sc_regInit ();
  sc_regSet (3, 1);
  instruction_counter = 0;
  accumulator = 0;
  mi_displayInstructionCounter ();
}