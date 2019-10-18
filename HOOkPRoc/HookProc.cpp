#include "HookProc.h"
char* CCTOC(const char* str)
{
	const char* cpCh = "const char";
	int nLen = strlen(cpCh);
	char* pCh = new char[nLen + 1];
	strcpy_s(pCh, nLen + 1, cpCh);
	return pCh;
}
