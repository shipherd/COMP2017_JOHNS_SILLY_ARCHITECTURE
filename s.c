#include"Disas.h"
#include"Loader.h"
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>  
#include <crtdbg.h>
#endif

long fileSize = -1;
byte * readFile(char * path) {
	struct stat s;
	byte * newMem = NULL;
	if (stat(path, &s)<0) {
		return NULL;
	}

	fileSize = s.st_size;
	FILE * f = fopen(path, "r");
	if (!f)return NULL;

	newMem = (byte*)malloc(s.st_size);
	if (!newMem)return NULL;

	int read = fread(newMem, 1, s.st_size, f);
	if (read != s.st_size) {
		free(newMem);
		return NULL;
	}
	return newMem;
}

int main(int num, char ** ary) {
	byte * mem = readFile("some test file here");
	if (mem == NULL) {
		printf("ERROR READ FILE!\n");
		goto END;
	}

	DList * lstFunc = newDList();
	DList * memList = newDList();

	for (int i = 0; i <fileSize; i++)stkPush(memList, &mem[i]);

	parseFunction(memList,lstFunc);

	printDisas(lstFunc);
	execute(lstFunc);

	

	while (lstFunc->Size != 0) {/*FREE THE STUFF*/
		Function * f = stkPop(lstFunc);
		while (f->instructions->Size != 0)free(stkPop(f->instructions));
		freeDList(f->instructions);
		free(f);
	}

	getchar();

	freeDList(lstFunc);
	freeDList(memList);
	free(mem);
	END:
	getchar();
#ifdef
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
