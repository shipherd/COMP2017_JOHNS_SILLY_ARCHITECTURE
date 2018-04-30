#include"Loader.h"
Function * findFunction(DList * funcList,byte label) {
	for (int i = 0; i < funcList->Size; i++) {
		Function * tmp = lstGet(funcList, i);
		if (tmp->Lable == label)return tmp;
	}
	return NULL;
}

void printDebugInfo(byte * reg, DList * ds, DList * cs) {
	printf("Regs|");
	for (int i = 0; i < 8; i++) {
		printf(" r%d:0x%.2hhX ",i,reg[i]);
	}
	printf("\n\n");

	byte * sb = (byte*)lstGet(ds, 0);
	byte * sp = (byte*)lstGet(ds, 1);
	byte * ip = (byte*)lstGet(ds, 2);

	printf("Stack Base:0x%0.2hhx\nStack Pointer:0x%0.2hhx\nInstruction Pointer:0x%0.2hhx\n\n", *sb, *sp, *ip);
	printIns(lstGet(cs, *ip - 1), *ip);
	printf("\n");
}
void execute(DList * funcList) {
	byte Reg[] = {0,0,0,0,0,0,0,0};
	DList * dataSeg = newDList();//FREE
	DList * codeSeg = newDList();//FREE

	Function * entry = findFunction(funcList, 0x00);
	/* dateSeg init */
	/* @:0, Stack Base */
	lstAdd(dataSeg, malloc(sizeof(byte)));//FREE
	byte * sb = (byte*)lstGet(dataSeg, 0);
	*sb = 0x00;

	/* @:1, Stack Pointer */
	lstAdd(dataSeg, malloc(sizeof(byte)));//FREE
	byte * sp = (byte*)lstGet(dataSeg,1);
	*sp = 0x03;

	/* @:2, Instruction Pointer*/
	lstAdd(dataSeg, malloc(sizeof(byte)));//FREE
	byte * ip = (byte*)lstGet(dataSeg, 2);
	*ip = 0x01;//Start from instruction 1

	/* --- dataSeg done --- */

	/* codeSeg init */

	//DList * lstTMP = newDList();//FREE
	for (int y = 0; y < funcList->Size; y++) {
		Function * f = lstGet(funcList, y);
		for (int x = 0; x < f->instructions->Size; x++) {
			Instruction * ins = lstGet(f->instructions,x);
			Instruction * newIns = malloc(sizeof(Instruction));//FREE
			memcpy(newIns, ins, sizeof(Instruction));
			lstAdd(codeSeg, newIns);
		}
	}

	for (int i = 0; i < codeSeg->Size; i++) { 
		Instruction * tmp = lstGet(codeSeg, i);
		if (tmp->opcode == 0x01) {
			byte flabel = tmp->DEST;
			int insIP = 0;
			for (int i = 0; i < funcList->Size; i++) {
				Function * ftmp = lstGet(funcList, i);
				if (ftmp->Lable == flabel) {
					insIP += 1;
					break;
				}
				else insIP += ftmp->numIns;
			}
			tmp->DEST = insIP;
		}
	
	}

	/* --- codeSeg done --- */

	/* Init RETURN */
	DList * retList = newDList();//FREE
	byte * ret = malloc(sizeof(byte));//FREE
	*ret = SYS_CALL_ADDR;
	stkPush(retList, ret);

	/* Init RETVALADDR */
	DList * retVal = newDList();//FREE


	while (retList->Size != 0) {
		printDebugInfo(Reg, dataSeg, codeSeg);
		Instruction * ins = lstGet(codeSeg, *ip - 1);
		switch (ins->opcode)
		{
		case 0x00://MOV = mov, and push
			if (ins->typeDEST == TYPE_POINTER_ADDR) {
				byte addr = *((byte*)lstGet(dataSeg, ins->DEST));
				byte * val = lstGet(dataSeg, addr);

				if (val == NULL) {

					if (dataSeg->Size == STACK_LIMIT) {
						printf("Stack Overflow!\n");
						goto END;
					}
					byte num = addr - (*sp);
					if (num == 0) {
						lstAdd(dataSeg, malloc(sizeof(byte)));//FREE

						/* ---- SP++ ---- */
						(*sp)++;
						/* ---- SP++ ---- */
					}
					else {
						for (int i = 0; i < num+1; i++) {
							lstAdd(dataSeg, malloc(sizeof(byte)));
							(*sp)++;
						}
					}

					val = lstGet(dataSeg, addr);

					
				}

				if (ins->typeSRC == TYPE_VALUE)*val = ins->SRC;
				if (ins->typeSRC == TYPE_REGISTER)*val = Reg[ins->SRC];
				
				if (ins->typeSRC == TYPE_POINTER_ADDR) {//v can no way be NULL, otherwise error occurs.
					byte a = *((byte*)lstGet(dataSeg, ins->SRC));
					byte * v = lstGet(dataSeg, a);
					*val = *v;
				}
				
				if (ins->typeSRC == TYPE_STACK_ADDR) {//v can no way be NULL, otherwise error occurs.
					byte * v = lstGet(dataSeg, ins->SRC);
					*val = *v;
				}
			}
			if (ins->typeDEST == TYPE_REGISTER) {
				byte * tgtReg = &(Reg[ins->DEST]);

				if (ins->typeSRC == TYPE_VALUE) {
					*tgtReg = ins->SRC;
				}
				if (ins->typeSRC == TYPE_POINTER_ADDR) {//v can no way be NULL, otherwise error occurs.
					byte a = *((byte*)lstGet(dataSeg, ins->SRC));
					byte * v = lstGet(dataSeg, a);
					*tgtReg = *v;
				}
				if (ins->typeSRC == TYPE_REGISTER) {
					*tgtReg = Reg[ins->SRC];
				}
				if (ins->typeSRC == TYPE_STACK_ADDR) {//v can no way be NULL, otherwise error occurs.
					byte * v = lstGet(dataSeg, ins->SRC);
					*tgtReg = *v;
				}
			}
			if (ins->typeDEST == TYPE_STACK_ADDR) {

				byte * val = lstGet(dataSeg, ins->DEST);

				if (val == NULL) {
					if (dataSeg->Size==STACK_LIMIT) {
						printf("Stack Overflow!\n");
						goto END;
					}
					lstAdd(dataSeg, malloc(sizeof(byte)));//FREE
					/* ---- SP++ ---- */
					(*sp)++;
					/* ---- SP++ ---- */

					val = lstGet(dataSeg, ins->DEST);
				}

				if (ins->typeSRC == TYPE_VALUE) {
					*val = ins->SRC;
				}
				if (ins->typeSRC == TYPE_POINTER_ADDR) {
					byte a = *((byte*)lstGet(dataSeg, ins->SRC));
					byte * v = lstGet(dataSeg, a);
					*val = *v;
				}
				if (ins->typeSRC == TYPE_REGISTER) {
					*val = Reg[ins->SRC];
				}
				if (ins->typeSRC == TYPE_STACK_ADDR) {
					byte * v = lstGet(dataSeg, ins->SRC);
					*val = *v;
				}
			}
			

			break;
		case 0x01: /*CALL*/{
			byte * newRet = malloc(sizeof(byte));//FREE
			*newRet = ++(*ip);
			stkPush(retList, newRet);
			/* IP CHANGED */
			*ip = ins->DEST;
			continue;
		}
			break;
		case 0x02: {//POP
			byte * val = lstGet(dataSeg, ins->DEST);
			stkPush(retVal, val);
			byte * ptr = malloc(sizeof(byte));//FREE
			*ptr = *val;
			lstAdd(dataSeg, ptr);
			/* (*SP)++ */
			(*sp)++;
		}

			break;
		case 0x03: {
			byte * ret = stkPop(retList);
			/* IP CHANGED */
			*ip = *ret;
			free(ret);
			continue;
		}
			
			break;
		case 0x04: {//ADD
			sbyte sum = (sbyte)Reg[ins->DEST] + (sbyte)Reg[ins->SRC];
			Reg[ins->DEST] = (byte)sum;
		}

			break;
		case 0x05: {//AND
			sbyte sum = (sbyte)Reg[ins->DEST] & (sbyte)Reg[ins->SRC];
			Reg[ins->DEST] = (byte)sum;
		}
			break;
		case 0x06: {//NOT
			Reg[ins->DEST] = ~Reg[ins->DEST];
		}

			break;
		case 0x07: {//CMP
			if (Reg[ins->DEST] == 0)Reg[ins->DEST] = 1;
			else Reg[ins->DEST] = 0;
		}

			break;
		default:
			printf("ERROR:UNKNOWN OPCODE\n");
			break;
		}

		/* IP CHANGDE*/
		(*ip)++;

	}

	printf("%d\n", *(byte*)stkPeek(retVal));

	END:
	for (int i = 0; i < dataSeg->Size; i++)free(lstGet(dataSeg, i));
	freeDList(dataSeg);
	for (int i = 0; i < codeSeg->Size; i++)free(lstGet(codeSeg, i));
	freeDList(codeSeg);

	lstFreeElement(retList);
	freeDList(retList);
	//for (int i = 0; i < retVal->Size; i++)free(lstGet(retVal, i));
	freeDList(retVal);

}

void printIns(Instruction * i, int  addr) {

	const char * ocStr = findCode(i->opcode)->str;
	sbyte tmp;
	printf("%.2hhX %s", addr,ocStr);
	switch (i->typeDEST)
	{
	case TYPE_VALUE:
		printf(" 0x%.2hhx", i->DEST);
		break;
	case TYPE_STACK_ADDR:
		printf(" BYTE PTR [0x%.2hhx]", i->DEST);
		break;
	case TYPE_REGISTER:
		printf(" R%d", i->DEST);
		break;

	case TYPE_POINTER_ADDR:
		printf(" BYTE PTR [[0x%.2hhx]]", i->DEST);
		break;
	default:
		break;
	}

	switch (i->typeSRC)
	{
	case TYPE_VALUE:
		tmp = (sbyte)i->SRC;
		printf(", %d", tmp);
		break;
	case TYPE_STACK_ADDR:
		printf(", BYTE PTR [0x%.2hhx]", i->SRC);
		break;
	case TYPE_REGISTER:
		printf(", R%d", i->SRC);
		break;

	case TYPE_POINTER_ADDR:
		printf(", BYTE PTR [[0x%.2hhx]]", i->SRC);
		break;
	default:
		break;
	}
	printf("\n");
}

void printDisas(DList * funcList) {
	for (int c = 0; c < funcList->Size; c++) {
		Function *f = lstGet(funcList, c);
		printf("Function:0x%.2hhx, Number Of Args:0x%.2hhx, Number Of Ins:0x%.2hhx\n", f->Lable, f->numArgs, f->numIns);
		for (int y = 0; y < f->instructions->Size; y++) {
			Instruction * i = lstGet(f->instructions, y);
			printIns(i,y+1);
		}
		printf("\n");
	}
}