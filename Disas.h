#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"DList.h"

#define byte unsigned char
#define sbyte signed char
#define bool int
#define true 1
#define false 0

#define NOT_USED 0xFF

#define VALUE 1
#define REGISTER 2
#define STACK_ADDR 4
#define POINTER_ADDR 8

#define TYPE_VALUE 0x00
#define TYPE_REGISTER 0x01
#define TYPE_STACK_ADDR 0x02
#define TYPE_POINTER_ADDR 0x03

#define USED_X 1
#define USED_Y 2

#define FROM_LEFT 0
#define FROM_RIGHT 1

typedef struct {
	byte hex;
	const char * str;
	byte opFieldUsed;
	byte Y;
	byte X;
}Code;

typedef struct {
	byte opcode;
	byte typeDEST;
	byte typeSRC;
	byte DEST;
	byte SRC;
}Instruction;

typedef struct {
	byte Lable;
	byte numArgs;
	DList * instructions;
	byte numIns;
}Function;


void parseFunction(DList* memList, DList * funcList);
byte getBits(DList * memList, int numBits);
const Code * findCode(byte opcode);