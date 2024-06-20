#ifndef BOG_STRING_H
#define BOG_STRING_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

static bool BOGIsAlphaChar(char c) {
	return (((c | 32) - 'a') < 26U);
}

static bool BOGIsAlphaString(char *s) {
	char *temp = s;
	while (BOGIsAlphaChar(*temp)) temp++;
	return !*temp;
}

static bool BOGIsDigit(char c) {
	return (c >= '0' && c <= '9');
}

static bool BOGIsNumber(char *s) {
	char *temp = s;
	if (temp[0] == '-' && temp[1]) temp++;
	while (*temp && BOGIsDigit(*temp)) temp++;
	return !*temp;
}

static char BOGToLowerChar(char c) {
	return BOGIsAlphaChar(c) ? (c | ('a' - 'A')) : c;
}

static char BOGToUpperChar(char c) {
	return BOGIsAlphaChar(c) ? (c & ~('a' - 'A')) : c;
}

static char * BOGToLowerString(char *s) {
	char *temp = s;
	while (*temp = BOGToLowerChar(*temp++));
	
	return s;
}

static char * BOGToUpperString(char *s) {
	char *temp = s;
	while (*temp = BOGToUpperChar(*temp++));
	
	return s;
}

static size_t BOGStringLength(char *s) {
	char *p = s;
	while (*s) s++;
	
	return (s - p);
}

static char * BOGStringCat(char *dest, char *src) {
	char *temp = dest;
	while (*dest) dest++;
	while (*dest++ = *src++);
	
	return temp;
}

static int32_t BOGStringCompare(char *s1, char *s2) {
	while(*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	
	return (*s1 - *s2);
}

static char * BOGStringCopy(char *dest, char* src) {
	char *temp = dest;
	while (*dest++ = *src++);
	
	return temp;
}

static char * BOGStringCatToChar(char *dest, char *src, char c) {
	char *temp = dest;
	while (*dest) dest++;
	while ((*src != c) && (*dest++ = *src++));
	
	return temp;
}

static char * BOGStringCatToIndex(char *dest, char *src, int32_t index) {
	char *temp = dest;
	while (*dest) dest++;
	while (index-- && (*dest++ = *src++));
	
	return temp;
}

static char * BOGStringCatFromChar(char *dest, char *src, char c) {
	while (*src && (*src != c)) *src++;
	
	return BOGStringCat(dest, src);
}

static char * BOGStringCatFromIndex(char *dest, char *src, int32_t index) {
	while (*src && index--) *src++;
	
	return BOGStringCat(dest, src);
}

static int32_t BOGStringCompareToChar(char *s1, char *s2, char c) {
	while (*s1 && (*s1 == *s2) && (*s1 != c)) {
		s1++;
		s2++;
	}
	
	return *s1 - *s2;
}

static int32_t BOGStringCompareToIndex(char *s1, char *s2, int32_t index) {
	while (*s1 && (*s1 == *s2) && index--) {
		s1++;
		s2++;
	}
	
	return *s1 - *s2;
}

static int32_t BOGStringCompareFromChar(char *s1, char *s2, char c) {
	while (*s1 && (*s1 != c)) s1++;
	while (*s2 && (*s2 != c)) s2++;
	
	return BOGStringCompare(s1, s2);
}

static int32_t BOGStringCompareFromIndex(char *s1, char *s2, int32_t index) {
	while ((*s1 && *s2) && index--) {
		s1++;
		s2++;
	}
	
	return BOGStringCompare(s1, s2);
}

static char * BOGStringCopyToChar(char *dest, char* src, char c) {
	char *temp = dest;
	while ((*src != c) && (*dest++ = *src++));
	
	return temp;
}

static char * BOGStringCopyToIndex(char *dest, char *src, int32_t index) {
	char *temp = dest;
	while (index-- && (*dest++ = *src++));
	
	return temp;
}

static char * BOGStringCopyFromChar(char *dest, char *src, char c) {
	while (*src && *src != c) src++;
	
	return BOGStringCopy(dest, src);
}

static char * BOGStringCopyFromIndex(char *dest, char *src, int32_t index) {
	while (*src && index--) src++;
	
	return BOGStringCopy(dest, src);
}

static size_t BOGStringLengthToChar(char *s, char c) {
	char *p = s;
	while (*s && (*s != c)) s++;
	
	return (s - p);
}

static size_t BOGStringLengthToIndex(char *s, int32_t index) {
	char *p = s;
	while (*s && index--) s++;
	
	return (s - p);
}

static size_t BOGStringLengthFromChar(char *s, char c) {
	while (*s && (*s != c)) s++;
	
	return BOGStringLength(s);
}

static size_t BOGStringLengthFromIndex(char *s, int32_t index) {
	while (*s && index--) s++;
	
	return BOGStringLength(s);
}

int32_t BOGStringToInt(char *s) {
	char *temp = s;
	int32_t result = 0;
	int8_t sign = 1;
	
	if (*s == '-') {
		sign = -1;
		*s++;
	}
	
	while (*temp && BOGIsNumber(s)) result = 10 * result + (*temp++ - '0');
	
	return sign * result;
}

static char * BOGStringReverse(char *s) {
	char *temp = s;
	
	size_t len = BOGStringLength(temp);
	for (size_t i = 0; i < len / 2; ++i) {
		char start = temp[i];
		char end = temp[(len - 1) - i];
		temp[(len - 1) - i] = start;
		temp[i] = end;
	}
	
	return s;
}

static char * BOGIntToString(int32_t num, char *s, int8_t base) {
	char *temp = s;
	bool isNegative = false;
	
	if (num < 0) {
		isNegative = true;
		num = -num;
	}
	
	do {
		int8_t digit = num % base;
		*temp++ = (digit > 9) ? (digit - 10) + 'a' : digit + '0';
		num /= base;
	} while (num);
	
	if (isNegative) *temp++ = '-';
	*temp = '\0';
	
	BOGStringReverse(s);
	
	return s;
}

static char * BOGSubstring(char *s, int32_t start, int32_t end) {
	int32_t length = (int32_t) BOGStringLength(s);
	
	if (start < 0) start = (start + length < 0) ? 0 : start + length;
	if (end < 0) end = (end + length < 0) ? 0 : end + length;
	if (end < start) end = start;
	
	char *sub = &s[start];
	s[end] = '\0';
	
	return sub;
}

static char ** BOGStringSplit(char *s, char *delims) {
	char *temp1 = s;
	char *temp2 = delims;
	
	size_t length = 0;
	while (*temp1) {
		if (*temp1 == *temp2) {
			do {
				temp1++;
				temp2++;
			} while (*temp1 && (*temp1 == *temp2));
			
			if (!*temp2) length++;
			temp2 = delims;
		} else {
			*temp1++;
		}
		
		if (!*temp1) length++;
	}
	
	char **result = calloc(length + 1, sizeof(char *));
	char **temp3 = result;
	
	temp1 = s;
	size_t count = 0;
	while (*temp1) {
		if (*temp1 == *temp2) {
			char *address = &(*temp1);
			
			do {
				count++;
				temp1++;
				temp2++;
			} while (*temp1 && (*temp1 == *temp2));
			
			if (!*temp2) {
				count -= (temp1 - address);
				*temp3++ = malloc(sizeof(char) * (count + 1));
				count = 0;
			}
			
			temp2 = delims;
		} else {
			count++;
			*temp1++;
		}
		
		if (!*temp1) *temp3 = malloc(sizeof(char) * (count + 1));
	}
	
	temp1 = s;
	char *str = result[0];
	for (size_t i = 0; i < length && *temp1;) {
		if (*temp1 == *temp2 && result[i + 1]) {
			do {
				temp1++;
				temp2++;
			} while (*temp1 && (*temp1 == *temp2));
			
			if (*temp2) {
				temp1 -= BOGStringLength(temp2);
				*str++ = *temp1++;
			} else {
				*str = '\0';
				str = result[++i];
			}
			
			temp2 = delims;
		} else {
			*str++ = *temp1++;
		}
	}
	
	return result;
}

static char * BOGStringJoin(char **ss, char *delims) {
	char **temp1 = ss;
	
	size_t length1 = 0;
	size_t length2 = 0;
	while (*temp1) {
		length1 += BOGStringLength(*temp1++);
		length2++;
	}
	
	char *s = malloc(sizeof(char) * (length1 + 1));
	char *temp2 = s;
	char *temp3 = delims;
	char *str = ss[0];
	for (size_t i = 0; i < length2;) {
		while (*str) *temp2++ = *str++;
		str = ss[++i];
		while (*temp3 && (i != length2)) *temp2++ = *temp3++;
		temp3 = delims;
	}
	*temp2 = '\0';
	
	return s;
}

#endif //BOG_STRING_H
