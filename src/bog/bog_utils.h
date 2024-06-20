#ifndef BOG_UTILS_H
#define BOG_UTILS_H

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#define BOG_ASSERT(expression) if (!(expression)) {*(int *) 0 = 0;}

#define BOG_ARR_LEN(x) ((sizeof(x) / sizeof(*x)))
#define BOG_SWAP(a, b) \
	if ((a) != (b)) { \
		*(a) ^= *(b); \
		*(b) ^= *(a); \
		*(a) ^= *(b); \
}
#define BOG_MIN(a, b) ((a) < (b)) ? (a) : (b)
#define BOG_MAX(a, b) ((a) > (b)) ? (a) : (b)

typedef struct {
	void *contents;
	size_t size;
} bog_file_res;

static bog_file_res BOGReadFile(char *fileName) {
	bog_file_res res = {0};
	
	HANDLE file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (file != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER fileSize;
		GetFileSizeEx(file, &fileSize);
		
		res.size = fileSize.QuadPart;
		
		HANDLE heap = GetProcessHeap();
		void *buffer = HeapAlloc(heap, 0,  fileSize.QuadPart);
		
		DWORD numBytesRead;
		ReadFile(file, buffer, (DWORD) fileSize.QuadPart, &numBytesRead, 0);
		
		res.contents = buffer;
	}
	
	CloseHandle(file);
	
	return res;
}

static void BOGWriteFile(char *fileName, void *buffer, uint32_t bufferSize) {
	HANDLE file = CreateFile(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	
	if (file != INVALID_HANDLE_VALUE) {
		LARGE_INTEGER fileSize;
		GetFileSizeEx(file, &fileSize);
		
		DWORD numBytesWritten;
		WriteFile(file, buffer, bufferSize, &numBytesWritten, 0);
	}
	
	CloseHandle(file);
}

static void BOGPrintF(char *format, ...) {
	va_list args1, args2;
	va_start(args1, format);
	va_copy(args2, args1);
	
	char *temp1 = format;
	size_t length = 0;
	while (*temp1) {
		while (*temp1 && (*temp1 != '%')) {
			*temp1++;
			length++;
		}
		
		if (*temp1) {
			switch (*(++temp1)) {
				case '%': {
					length++;
				} break;
				
				case 'c': {
					va_arg(args1, char);
					length++;
				} break;
				
				case 's': {
					char *s = va_arg(args1, char *);
					while (*s++) length++;
				} break;
				
				case 'p': {
					void *p = va_arg(args1, void *);
					length += (sizeof(p) * 2);
				} break;
				
				case 'i': {
					int64_t i = va_arg(args1, int64_t);
					
					size_t digits = 0;
					if (i <= 0) digits++;
					
					while (i) {
						i /= 10;
						digits++;
					}
					
					length += digits;
				} break;
				
				case 'u': {
					uint64_t u = va_arg(args1, uint64_t);
					
					size_t digits = 0;
					if (!u) digits++;
					
					while (u) {
						u /= 10;
						digits++;
					}
					
					length += digits;
				} break;
				
				case 'd': {
					double d = va_arg(args1, double);
					length += snprintf(0, 0, "%g", d);
				} break;
			}
			
			*temp1++;
		}
	}
	va_end(args1);
	
	char *result = malloc(length + 1);
	char *temp2 = result;
	
	for (char *at = format; *at; ++at) {
		if (*at != '%') {
			*temp2++ = *at;
		} else {
			switch (*(++at)) {
				case '%': {
					*temp2++ = *at;
				} break;
				
				case 'c': {
					char c = va_arg(args2, char);
					*temp2++ = c;
				} break;
				
				case 's': {
					char *s = va_arg(args2, char *);
					while (*s) *temp2++ = *s++;
				} break;
				
				case 'p': {
					void *p = va_arg(args2, void *);
					int8_t digits = sizeof(p) * 2;
					
					char *buffer = malloc(digits + 1);
					sprintf_s(buffer, digits + 1, "%p", p);
					
					char *temp3 = buffer;
					while (*temp3) *temp2++ = *temp3++;
					free(buffer);
				} break;
				
				case 'i': {
					int64_t i = va_arg(args2, int64_t);
					int64_t t = i;
					
					size_t digits = 0;
					if (t <= 0) digits++;
					
					while (t) {
						t /= 10;
						digits++;
					}
					
					char *buffer = malloc(digits + 1);
					sprintf_s(buffer, digits + 1, "%lli", i);
					
					char *temp3 = buffer;
					while (*temp3) *temp2++ = *temp3++;
					free(buffer);
				} break;
				
				case 'u': {
					uint64_t u = va_arg(args2, uint64_t);
					uint64_t t = u;
					
					size_t digits = 0;
					if (!t) digits++;
					
					while (t) {
						t /= 10;
						digits++;
					}
					
					char *buffer = malloc(digits + 1);
					sprintf_s(buffer, digits + 1, "%llu", u);
					
					char *temp3 = buffer;
					while (*temp3) *temp2++ = *temp3++;
					free(buffer);
				} break;
				
				case 'd': {
					double d = va_arg(args2, double);
					
					size_t digits = snprintf(0, 0, "%g", d);
					
					char *buffer = malloc(digits + 1);
					sprintf_s(buffer, digits + 1, "%g", d);
					
					char *temp3 = buffer;
					while (*temp3) *temp2++ = *temp3++;
					free(buffer);
				} break;
			}
		}
	}
	
	*temp2 = '\0';
	OutputDebugString(result);
	
	free(result);
	va_end(args2);
}

uint64_t BOGGetSystemTime() {
	uint64_t ticksPerSecond = 10000000;
	
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	
	ULARGE_INTEGER ui = {ft.dwLowDateTime, ft.dwHighDateTime};
	
	return (ui.QuadPart / ticksPerSecond);
}

uint64_t BOGRandom(uint64_t *seed) {
	uint64_t num = *seed;
	num ^= num << 13;
	num ^= num >> 7;
	num ^= num << 17;
	*seed = num;
	
	return num;
}

#endif //BOG_UTILS_H
