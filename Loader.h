#pragma once
#include"DList.h"
#include"Disas.h"

#define STACK_LIMIT 128
#define SYS_CALL_ADDR 0xFF

void execute(DList * funcList);
void printDisas(DList * funcList);
void printIns(Instruction * i, int addr);