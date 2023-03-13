// Copyright [2023] <Raimguzhinov>
#pragma once
#include <stdio.h>

#define CPU_CMDS_COUNT 38 // Количество команд
#define RAM_SIZE 100      // Размер оперативной памяти
#define REG_COUNT 5       // Количество регистров

#define FLAG_P 1 // Флаг переполнения при выполнении операции
#define FLAG_0 2 // Флаг ошибки деления на ноль
#define FLAG_M 3 // Флаг ошибки выхода за границы памяти
#define FLAG_T 4 // Флаг игнорирования тактовых импульсов
#define FLAG_E 5 // Флаг неверной команды

#define READ 10
#define WRITE 11
#define LOAD 20
#define STORE 21
#define ADD 30
#define SUB 31
#define DIVIDE 32
#define MUL 33
#define JUMP 40
#define JNEG 41
#define JZ 42
#define HALT 43
#define NOT 51

// Макрос для проверки существующей команды процессору
#define CHECK_CPU_CMD(x)                                                      \
  ((x == READ) || (x == WRITE) || (x == LOAD) || (x == STORE) || (x == ADD)   \
   || (x == SUB) || (x == DIVIDE) || (x == MUL) || (x == JUMP) || (x == JNEG) \
   || (x == JZ) || (x == HALT) || (x == NOT)) ? 1 : 0

int sc_memoryInit ();
int sc_memorySet (int address, int value);
int sc_memoryGet (int address, int *value);
int sc_memorySave (char *filename);
int sc_memoryLoad (char *filename);
int sc_regInit (void);
int sc_regSet (int registr, int value);
int sc_regGet (int registr, int *value);
int sc_commandEncode (int command, int operand, int *value);
int sc_commandDecode (int value, int *command, int *operand);
