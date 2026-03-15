#include <stdlib.h>
#include <memory.h>
#include <string.h>
typedef unsigned char byte;
#include "StringHash.h"

__forceinline byte CalcHash(char* s) {
	#if defined(_MSC_VER) && defined(_M_IX86)
	__asm {
		mov esi, s
		cld
		xor bl, bl
		xor eax, eax
		strlp1 : lodsb
		add bl, al
		or al, al
		jnz strlp1
		mov al, bl
	};
	#else
	byte hash = 0;
	const unsigned char* p = (const unsigned char*)s;
	while (*p) { hash += *p; p++; }
	return hash;
	#endif
};
StrHash::StrHash(){
	memset(SHI, 0, sizeof(SHI));
	LastIndex = 0;
};
void StrHash::Clear(){
	for(int i=0;i<256;i++){
		StrHashItem* lpSHI=SHI+i;
		for(int j=0;j<lpSHI->NStr;j++)free(lpSHI->Str[j]);
		if(lpSHI->Str){
			free(lpSHI->Str);
			free(lpSHI->Value);
		};
	};
	memset(SHI, 0, sizeof(SHI));
	LastIndex = 0;
};
StrHash::~StrHash(){
	Clear();
};
void StrHash::AddString(char* s){
	byte hv=CalcHash(s);
	StrHashItem* lpSHI=SHI+hv;
	int nv=lpSHI->NStr;
	if(lpSHI->NStr>=lpSHI->MaxStr){
		lpSHI->MaxStr+=16;
		lpSHI->Str=(char**)realloc(lpSHI->Str,lpSHI->MaxStr*sizeof(char*));
		lpSHI->Value=(int*)realloc(lpSHI->Value,lpSHI->MaxStr<<2);
	};
	lpSHI->Str[nv]=(char*)malloc(strlen(s)+1);
	strcpy(lpSHI->Str[nv],s);
	lpSHI->Value[nv]=LastIndex;
	LastIndex++;
	lpSHI->NStr++;
};
int StrHash::SearchString(char* s){
	byte hv=CalcHash(s);
	StrHashItem* lpSHI=SHI+hv;
	int nv=lpSHI->NStr;
	for(int i=0;i<nv;i++)if(!strcmp(lpSHI->Str[i],s)){
		return lpSHI->Value[i];
	};
	return -1;
};