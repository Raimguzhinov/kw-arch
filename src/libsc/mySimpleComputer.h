// Copyright [2023] <Raimguzhinov>
#pragma once
#include <stdio.h>

#define CPU_CMDS_COUNT 38 // Количество команд
#define RAM_SIZE 100      // Размер оперативной памяти
#define REG_COUNT 5       // Количество регистров

#define FLAG_P 0x0001 // Флаг переполнения при выполнении операции
#define FLAG_0 0x0002 // Флаг ошибки деления на ноль
#define FLAG_M 0x0004 // Флаг ошибки выхода за границы памяти
#define FLAG_T 0x0008 // Флаг игнорирования тактовых импульсов
#define FLAG_E 0x0010 // Флаг неверной команды

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
#define AND 52
#define OR 53
#define XOR 54
#define JNS 55
#define JC 56
#define JNC 57
#define JP 58
#define JNP 59
#define CHL 60
#define SHR 61
#define RCL 62
#define RCR 63
#define NEG 64
#define ADDC 65
#define SUBC 66
#define LOGLC 67
#define LOGRC 68
#define RCCL 69
#define RCCR 70
#define MOVA 71
#define MOVR 72
#define MOVCA 73
#define MOVCR 74
#define pADDC 75
#define pSUBC 76

// Макрос для проверки существующей команды процессору
#define CHECK_CPU_CMD(x) ((x == READ) || (x == WRITE) || (x == LOAD) || (x == STORE) || (x == ADD) || (x == SUB) || (x == DIVIDE) || (x == MUL) || (x == JUMP) || (x == JNEG) || (x == JZ) || (x == HALT) || (x == NOT) || (x == AND) || (x == OR) || (x == XOR) || (x == JNS) || (x == JC) || (x == JNC) || (x == JP) || (x == JNP) || (x == CHL) || (x == SHR) || (x == RCL) || (x == RCR) || (x == NEG) || (x == ADDC) || (x == SUBC) || (x == LOGLC) || (x == LOGRC) || (x == RCCL) || (x == RCCR) || (x == MOVA) || (x == MOVR) || (x == MOVCA) || (x == MOVCR) || (x == pADDC) || (x == pSUBC)) ? 1 : 0

int sc_memoryInit ();
int sc_memorySet (int address, int value);
int sc_memoryGet (int address, int *value);
int sc_memorySave (char *filename);
int sc_memoryLoad(char *filename);
int sc_regInit (void);
int sc_regSet (int registr, int value);
int sc_regGet (int registr, int *value);
int sc_commandEncode(int command, int operand, int *value);
int sc_commandDecode(int value,int *command,int *operand);
