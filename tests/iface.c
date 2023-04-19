#include <myInterface.h>

int
main ()
{
  int counter = 0x35;
  bc_initfont ("mybch.font");
  sc_memoryInit ();
  sc_memorySet (0x34, 0x4a81);
  sc_memorySet (counter, 0x2be4);
  sc_memorySet (0x36, 0x4a81);
  mi_uiInit (0x35);
  printf ("\n");

  return 0;
}