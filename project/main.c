#include <mySimpleComputer.h>
#include <myTerm.h>

int
main ()
{
  int value = 0;
  sc_memoryInit ();
  printf ("\n>> инициализирована память\n");
  for (int i = 0; i < RAM_SIZE / 10; i++)
    sc_memorySet (i, i + 1);
  for (int i = 0; i < RAM_SIZE / 10; i++)
    {
      sc_memoryGet (i, &value);
      printf ("RAM[%d] = %d\n", i, value);
    }
  sc_memorySet (0, 99);
  sc_memorySave ("test.bin");
  printf ("\n>> сохранение файла\n");
  sc_memoryGet (0, &value);
  printf ("RAM[0] = %d\n", value);
  sc_memoryLoad ("test.bin");
  printf ("\n>> чтение файла\n");
  for (int i = 0; i < RAM_SIZE / 10; i++)
    {
      sc_memoryGet (i, &value);
      printf ("RAM[%d] = %d\n", i, value);
    }
  sc_regInit ();
  sc_regSet (FLAG_T, 1);
  printf ("\n>> поднят флаг для игнорирования тактовых импульсов\n");
  sc_regGet (FLAG_P, &value);
  printf ("переполнение при выполнении операции [FLAG_P] = %d\n", value);
  sc_regGet (FLAG_0, &value);
  printf ("ошибка деления на 0 [FLAG_0] = %d\n", value);
  sc_regGet (FLAG_M, &value);
  printf ("ишибка выхода за границы памяти [FLAG_M] = %d\n", value);
  sc_regGet (FLAG_T, &value);
  printf ("игнорирование тактовых импульсов [FLAG_T] = %d\n", value);
  sc_regGet (FLAG_E, &value);
  printf ("указана неверная команда [FLAG_E] = %d\n", value);
  printf ("\n>> кодирование\n");
  int encode, oper, com = 0;
  sc_commandEncode (0x33, 0x59, &encode);
  printf ("закодированная команда: %x\n", encode);
  sc_commandDecode (encode, &com, &oper);
  printf ("команда: %x\nоперанд: %x\n\n", com, oper);
  return 0;
}