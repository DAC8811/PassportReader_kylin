#include "ContentCheck.h"

int ContentCheck::MRZParityCheck1(int* MRZ, int start, int end)
{
	int len = end - start + 1;

	if ((len < 0) | (start < 0) | (end < 0))
		return 0;
	if ((len > 89) | (start > 89) | (end >89))
		return 0;

	int Sum = 0;
	int ParityCode[3] = { 7, 3, 1 };

	for (int m = 0; m < len; m++)
	{
		Sum = MRZ[start + m] * ParityCode[m % 3] + Sum;
	}
	//cout << Sum <<endl;

	if ((Sum % 10) == MRZ[end + 1])
		return 1;
	else
		return 2;
}

int ContentCheck::MRZParityCheck2(int* MRZ)
{
	int start = 0;
	int Sum = 0;
	int ParityCode[3] = { 7, 3, 1 };

	start = 45;
	for (int m = 0; m < 10; m++)
	{
		Sum = MRZ[start + m] * ParityCode[m % 3] + Sum;
	}

	start = 58;
	for (int m = 0; m < 7; m++)
	{
		Sum = MRZ[start + m] * ParityCode[(m + 10) % 3] + Sum;
	}

	start = 66;
	for (int m = 0; m < 22; m++)
	{
		Sum = MRZ[start + m] * ParityCode[(m + 17) % 3] + Sum;
	}
	//cout << Sum << endl;

	if ((Sum % 10) == MRZ[88])
		return 1;
	else
		return 2;
}
