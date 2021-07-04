#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct _tagMagicShift {
	int magic;
	int shift;
} tagMagicShift;

tagMagicShift MagicShiftList[13] = {
	{1, 1},
	{1, 1},
	{1, 1},
	{0x55555556, 0},
	{0, 0},
	{0x66666667, 1},
	{0x2AAAAAAB, 0},
	{0x92492493, 2},
	{0, 0},
	{0x38E38E39, 1},
	{0x66666667, 2},
	{0x2E8BA2E9, 1},
	{0x2AAAAAAB, 1}
};

tagMagicShift result;

tagMagicShift* __fastcall CalcMagicNumber(int divisor) {
	if (divisor >= 3 && divisor < 13) {
		return &MagicShiftList[divisor];
	}

	unsigned int AdRemain, dQuotient, dRemain, Ad, d, AdQuotient;
	unsigned int shift = 31;
	if (divisor >= 0) {
		d = divisor;
		Ad = 0x7fff'ffff - 0x8000'0000 % d;
	}
	else {
		d = ~divisor + 1;
		Ad = 0x8000'0000 - 0x8000'0001 % d;
	}
	AdQuotient = 0x8000'0000 / Ad;
	AdRemain = 0x8000'0000 % Ad;
	dQuotient = 0x8000'0000 / d;
	dRemain = 0x8000'0000 % d;

	do {
		shift++;
		AdRemain = AdRemain * 2;
		AdQuotient = AdQuotient * 2;
		if (AdRemain >= Ad) {
			AdQuotient++;
			AdRemain = AdRemain - Ad;
		}
		dRemain = dRemain * 2;
		dQuotient = dQuotient * 2;
		if (dRemain >= d) {
			dQuotient++;
			dRemain = dRemain - d;
		}
	} while (AdQuotient < d - dRemain || (AdQuotient == d - dRemain && AdRemain == 0));
	
	result.magic = dQuotient + 1;
	if (divisor < 0) {
		result.magic = -result.magic;
	}

	result.shift = shift - 32;

	return &result;
}

int main() {
	DWORD BaseAddress = (DWORD)LoadLibraryA("C2.DLL");
	if (BaseAddress == 0) {
		printf("LoadLibraryA Failed!, %d", GetLastError());
		return -1;
	}
	tagMagicShift *(__fastcall * OrigFunc)(int) = (tagMagicShift *(__fastcall*)(int))(BaseAddress + 0x5EACE);
	printf("OrigFunc: %p, ReverseFunc: %p\n", OrigFunc, CalcMagicNumber);
	system("pause");
	
	long long correct = 0, total = 0;
	for (int divisor = -1000000; divisor <= 1000000; divisor++) {
		if (!divisor) {
			continue;
		}
		tagMagicShift *originResult = OrigFunc(divisor);
		tagMagicShift *reverseResult = CalcMagicNumber(divisor);
		printf("----------------------------\n");
		printf("origin:  %d: %08x, %2d\n", divisor, originResult->magic, originResult->shift);
		printf("reverse: %d: %08x, %2d\n", divisor, reverseResult->magic, reverseResult->shift);
		total++;
		if (originResult->magic == reverseResult->magic && originResult->shift == reverseResult->shift) {
			correct++;
		}
		else {
			printf("Wrong Answer!");
			system("pause");
			return -1;
		}
	}
	printf("Correct Rate: %lld/%lld\n", correct, total);
	system("pause");
	return 0;
}