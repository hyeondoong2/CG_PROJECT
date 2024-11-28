#include <stdlib.h>
#include <stdio.h>

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	if (fopen_s(&fptr, file, "rb") != 0) {  // "r" 대신 "rb" (바이너리 모드 사용)
		return NULL; // 파일 열기에 실패한 경우
	}(file, "rb");

	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}