#include "myBigChars.h"
#include "mySimpleComputer.h"
#include "myTerm.h"
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

extern int instruction_counter;

void
testFONT (int num, int x, int y)
{
  // mt_clrscr ();
  bc_printbigchar (big_chars[num], x, y, PURPLE, DEFAULT);
  mt_gotoXY (x + 1, y + 12);
}

int
main ()
{
  mt_clrscr ();
  testFONT (2, 4, 2);
  bc_printbigchar (big_chars[16], 14, 2, WHITE, DEFAULT);
  testFONT (2, 23, 2);
  bc_printbigchar (big_chars[17], 33, 0, WHITE, DEFAULT);
  bc_printbigchar (big_chars[17], 33, 4, WHITE, DEFAULT);
  testFONT (4, 42, 2);
  printf ("\tшрифт ↓\n");
  int fd = open ("font.txt", O_WRONLY | O_CREAT, 0600);
  if (fd < 0)
    {
      perror ("open");
      abort ();
    }
  for (int i = 0; i < 18; i++)
    {
      if (bc_bigcharwrite (fd, big_chars[i], 2) < 0)
        {
          perror ("bc_bigcharwrite");
          abort ();
        }
    }
  if (close (fd) < 0)
    {
      perror ("close");
      abort ();
    }
  fd = open ("font.txt", O_RDONLY);
  if (fd < 0)
    {
      perror ("open");
      abort ();
    }
  int count;
  int big[2];
  for (int i = 0; i < 18; i++)
    {
      if (bc_bigcharread (fd, big, 2, &count) < 0)
        {
          perror ("bc_bigcharread");
          abort ();
        }
      if (count != 2 || big[0] != big_chars[i][0] || big[1] != big_chars[i][1])
        {
          fprintf (stderr, "Error: read unexpected big char %d\n", i);
          abort ();
        }
      bc_printbigchar (big, 2 + i * 10, 15, WHITE, DEFAULT);
    }
  if (close (fd) < 0)
    {
      perror ("close");
      abort ();
    }
  printf ("\nвсе тесты прошли успешно!\n");
  return 0;
}
