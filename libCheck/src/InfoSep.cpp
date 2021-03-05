#include "ContentCheck.h"

int ContentCheck::DateDetect(int* num, char* date)
{
	int count = 0;
	char* datenum[3];

	strncmp(datenum[0], date, 4);
	strncmp(datenum[1], date + 4, 2);
	strncmp(datenum[2], date + 6, 4);

	for (count = 0; count < 3; count++)
	{
		if (datenum[count] == NULL)
			continue;
		num[count] = atoi(datenum[count]);
	}

	return 1;

}



int ContentCheck::GetInfo(char* MRZ, char* nation, char* type)
{
	//todo
//	if (MRZ == NULL)
//		return 0;
//
//	int check = 0;
//
//	check = strncpy_s(nation, 4, MRZ + 2, 3);
//	if (check != 0)
//		return 0;
//
//	if (!strncmp(nation, "CHN", 3))
//	{
//		check = strncpy_s(type, 3, MRZ + 45, 1);
//	}
//	else
//	{
//		check = strncpy_s(type, 3, MRZ, 2);
//	}
//
//	if (check != 0)
//		return 0;
//	return 1;
	if (MRZ == NULL)
		return 0;


	strncpy(nation, MRZ + 2, 3);

	if (!strncmp(nation, "CHN", 3))
	{
		strncpy(type, MRZ + 45, 1);
	}
	else
	{
		strncpy(type, MRZ, 2);
	}

	return 1;
}