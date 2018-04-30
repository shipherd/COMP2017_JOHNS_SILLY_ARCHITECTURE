#include"Disas.h"

/* INTEL SYNTAX IN USE*/
/* ORIGIN: YYYYYYY|YY|XXXXXXX|XX|OPCODE */
/* PARSED: OPCODE|DESTINATION|SOURCE*/
/* codeMap is in ORIGIN format */

const Code codeMap[] = {
{ 0x00,"MOV", USED_X | USED_Y, REGISTER | STACK_ADDR | VALUE|POINTER_ADDR, REGISTER | STACK_ADDR },// X is the DEST
{ 0x01,"CALL", USED_X | USED_Y, VALUE, STACK_ADDR },
{ 0x02,"POP", USED_Y, STACK_ADDR, NOT_USED },
{ 0x03,"RET", NOT_USED, NOT_USED, NOT_USED },
{ 0x04,"ADD", USED_X | USED_Y, REGISTER, REGISTER },//Y is the DEST
{ 0x05,"AND", USED_X | USED_Y, REGISTER, REGISTER },//Y is the DEST
{ 0x06,"NOT", USED_Y, REGISTER, NOT_USED },//Y is the DEST
{ 0x07,"CMP", USED_Y, REGISTER, NOT_USED } //Y is the DEST, CMP COMPARES REG WITH 0 BY DEFAULT
};
void printByte(char * str, byte B) {
	if (str)
		printf("%s:0x%.2hhX\n", str, B);
}
byte getBetweenBits(byte from, int startSide, int start, int length) {
	byte ret = 0x00;
	if (startSide == FROM_LEFT) {
		ret = from << start;
		ret = ret >> (8 - length);
	}
	else {
		ret = from >> start;
		ret = ret << (8 - length);
		ret = ret >> (8 - length);
	}
	return ret;
}
byte popByte(DList * memList) {
	byte * tmp = (byte*)stkPop(memList);
	return *tmp;
}
byte peekByte(DList * memList) {
	byte * tmp = (byte*)stkPeek(memList);
	return *tmp;
}

byte getBits(DList * memList, int numBits) {
	if (memList->Size == 1 && numBits == 8)return popByte(memList);
	static int read = 0;
	if (read + numBits > 8) {
		int remain = 8 - read;
		byte lastByte = popByte(memList);
		byte nextByte = peekByte(memList);
		lastByte = getBetweenBits(lastByte, FROM_LEFT, 0, remain);

		int newLength = numBits - remain;
		nextByte = getBetweenBits(nextByte, FROM_RIGHT, 0, newLength);
		nextByte = nextByte << remain;
		read = newLength;
		return nextByte | lastByte;
	}
	byte pb = peekByte(memList);
	byte ret = getBetweenBits(pb, FROM_RIGHT, read, numBits);
	read = read + numBits;
	if (read == 8) {
		stkPop(memList);
		read = 0;
	}
	return ret;
}

const Code * findCode(byte opcode) {
	const Code * tmp;
	for (int i = 0; i < sizeof(codeMap) / sizeof(Code); i++) {
		tmp = &codeMap[i];
		if (tmp->hex == opcode) return tmp;
	}
	return (void*)0;
}

void parseInstruction(DList * memList, Function * f, byte numIns) {
	if (numIns == 0)return;

	DList * insList = f->instructions;
	byte opcode = getBits(memList, 3);
	const Code * opc = findCode(opcode);

	//printf("%s", opc->str);

	Instruction * ins = malloc(sizeof(Instruction));
	ins->opcode = opcode;

	switch (opc->opFieldUsed) {
	case NOT_USED:
		//printf("\n");
		ins->DEST = NOT_USED;
		ins->SRC = NOT_USED;
		ins->typeDEST = NOT_USED;
		ins->typeSRC = NOT_USED;
		stkPush(insList, ins);
		parseInstruction(memList, f, numIns - 1);
		break;
	case USED_X://FOR A SINGAL FIELD ONLY READ Y FIELD AT THE MOMENT, SEE CODEMAP
	case USED_Y: {//Y IS DEST, X IS NOT_USED
		ins->typeSRC = NOT_USED;
		ins->SRC = NOT_USED;

		byte value = '?';
		byte typeField = getBits(memList, 2);
		ins->typeDEST = typeField;
		if (typeField == 0) {
			//printf(" VALUE:"); 
			value = getBits(memList, 8);
			//printf("0x%.2hhx\n", value);
		}
		else if (typeField == 1){
			//printf(" REGISTER:");
			 value = getBits(memList, 3);
			//printf("0x%.2hhx\n", value);
		}
		else if (typeField == 2){
			//printf(" STACK@:"); 
			 value = getBits(memList, 7);
			//printf("0x%.2hhx\n", value);
		}
		else if (typeField == 3){
			//printf(" POINTER:"); 
			 value = getBits(memList, 7);
			//printf("0x%.2hhx\n", value);
		}
		ins->DEST = value;
		stkPush(insList, ins);
		parseInstruction(memList, f, numIns - 1);
	}
		break;
	case USED_X | USED_Y:
	{
		byte * pDest, *pSrc;
		byte * typeDest, * typeSrc;
		if (opcode == 0x00) {
			pDest = &(ins->SRC);
			pSrc = &(ins->DEST);
			typeDest = &(ins->typeSRC);
			typeSrc = &(ins->typeDEST);
		}else {
			pDest = &(ins->DEST);
			pSrc = &(ins->SRC);
			typeDest = &(ins->typeDEST);
			typeSrc = &(ins->typeSRC);
		}
		/* DEST IS Y BY DEFAULT, AND X AS SRC */
		/* THE ONLY EXCEPTION IS MOV */
		/* ITS DEST IS X, AND Y IS SRC */
		/* IT IS TRANSLATED TO BE THE SAME ORDER AS THE OTHERS */
		//-----------------------------------X FIELD
		byte value = 0xFF;
		byte typeField = getBits(memList, 2);
		*typeSrc = typeField;
		if (typeField == 0) {
			//printf(" VALUE:");
			value = getBits(memList, 8);
			//printf("0x%.2hhx|", value);
		}
		else if (typeField == 1) {
			//printf(" REGISTER:");
			 value = getBits(memList, 3);
			//printf("0x%.2hhx|", value);
		}
		else if (typeField == 2) {
			//printf(" STACK@:");
			 value = getBits(memList, 7);
			//printf("0x%.2hhx|", value);
		}
		else if (typeField == 3) {
			//printf(" POINTER:");
			 value = getBits(memList, 7);
			//printf("0x%.2hhx|", value);
		}
		*pSrc = value;
		//-----------------------------------Y FIELD
		typeField = getBits(memList, 2);
		*typeDest = typeField;
		if (typeField == 0) {
			//printf(" VALUE:");
			value = getBits(memList, 8);
			//printf("0x%.2hhx\n", value);
		}
		else if (typeField == 1) {
			//printf(" REGISTER:");
			value = getBits(memList, 3);
			//printf("0x%.2hhx\n", value);
		}
		else if (typeField == 2) {
			//printf(" STACK@:");
			value = getBits(memList, 7);
			//printf("0x%.2hhx\n", value);
		}
		else if (typeField == 3) {
			//printf(" POINTER:");
			value = getBits(memList, 7);
			//printf("0x%.2hhx\n", value);
		}
		*pDest = value;
		stkPush(insList, ins);
		parseInstruction(memList, f, numIns - 1);
	}
		break;
	default:
		break;
	}

}
void parseFunction(DList * memList, DList * funcList) {
	Function * f = malloc(sizeof(Function));
	f->instructions = newDList();
	byte * Lable = &(f->Lable);
	byte * numArgs = &(f->numArgs);
	byte * numIns = &(f->numIns);

	
	byte tmp = getBits(memList,8);
	
	if (memList->Size == 0) {
		//printByte("Padding Byte is in",tmp);
		freeDList(f->instructions);
		free(f);
		return;
	}

	*numIns = tmp;

	parseInstruction(memList, f, *numIns);

	*numArgs = getBits(memList,4);
	*Lable = getBits(memList, 4);
	//printf("Function:0x%.2hhx, Number Of Args:0x%.2hhx, Number Of Ins:0x%.2hhx\n\n", f->Lable, f->numArgs, f->numIns);
	stkPush(funcList, f);
	parseFunction(memList, funcList);

}
