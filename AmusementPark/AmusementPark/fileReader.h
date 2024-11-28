#include <stdlib.h>
#include <stdio.h>

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	if (fopen_s(&fptr, file, "rb") != 0) {  // "r" ��� "rb" (���̳ʸ� ��� ���)
		return NULL; // ���� ���⿡ ������ ���
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