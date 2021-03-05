#include "ContentCheck.h"

int ContentCheck::SourceSeperate_New(char* WIImageLoc, char* NIRImageLoc, char* UVImageLoc, char* MRZ, char* CodePath, int(&RCode)[14])
{
	if (NIRImageLoc == NULL)
		return NULL;
	if (UVImageLoc == NULL)
		return 0;
	if (WIImageLoc == NULL)
		return 0;

	if ((sizeof(RCode) / sizeof(RCode[0])) < 14)
		return 0;

	Mat srcWI = imread(WIImageLoc, 1);
	Mat srcUV = imread(UVImageLoc, 1);
	Mat srcIR = imread(NIRImageLoc, 1);

	if ((!srcWI.data) || (!srcUV.data) || (!srcIR.data))
	{
		return 0;
	}


	Mat WI;
	Mat UV;
	Mat IR;
	resize(srcWI, WI, Size(1250, 900), 0, 0, INTER_LINEAR);
	resize(srcUV, UV, Size(1250, 900), 0, 0, INTER_LINEAR);
	resize(srcIR, IR, Size(1250, 900), 0, 0, INTER_LINEAR);

	char nation[4];
	char type[3];

	int check = GetInfo(MRZ, nation, type);

	if (check != 1)
		return 0;

	check = 0;

	check = Image_Modification(WI, UV, IR);

	if (check != 1)
		return 0;
	//imshow("WI", WI);
	//imshow("UV", UV);
	//imshow("IR", IR);
	int state = 0;
	int NationCode = NationDetect(nation);
	//cout << NationCode << endl;
	int TypeCode = PassportType(type);
	//cout << TypeCode << endl;

	//strrchr(ImageLoc, '/');

	for (int mode = 1; mode < 15; mode++)
	{
		RCode[mode - 1] = 5;
	}


	check = 0;
	switch (NationCode)
	{
	case 9:return 5; break;
	case 12:return 5; break;
	case 16:
	{
		if (TypeCode == 15)
			return 5;
		break;
	}
	case 15:return 5; break;
	case 27:return 5; break;
	case 29:return 5; break;
	case 30:return 5; break;
	case 32:
	{
		check = 1;
		if (TypeCode == 7)	//单独国家护照
			if (!CHN_E(WI, UV, IR, MRZ, NationCode, TypeCode, RCode,CodePath))
				return 2;
		break;
	}
	case 33:return 5; break;
	case 37:return 5; break;
	case 43:return 5; break;
	case 102:return 5; break;
	case 108:return 5; break;
	case 136:return 5; break;
	case 147:return 5; break;
	case 148:return 5; break;
	case 167:return 5; break;
	case 168:return 5; break;
	case 176:return 5; break;
	case 177:return 5; break;
	case 178:return 5; break;
	case 179:return 5; break;


	}

	if (check == 0)
	{
		for (int mode = 1; mode < 15; mode++)
		{
			//std::cout << "HELLO";
			//RCode[mode - 1] = 0;
			switch (mode)
			{
			case 1:
			{
				state = Mode1_Data(UV, NationCode, TypeCode, NULL);
				break;
			}
			case 2:
			{
				state = Mode2_Data(UV, NationCode, TypeCode, CodePath);
				break;
			}
			case 3:
			{
				state = Mode3_Data(UV, NationCode, TypeCode, NULL);
				break;
			}
			case 4:
			{
				state = Mode4_Data(UV, NationCode, TypeCode, NULL);
				break;
			}
			case 5:
			{
				state = Mode_Mix_5_6(UV, NationCode, TypeCode, NULL);
				break;
			}
			case 6:
			{
				state = Mode7_Data(UV, NationCode, TypeCode, NULL);
				break;
			}
			case 7:
			{
				state = Mode_Mix_8_9_10_11(UV, NationCode, TypeCode, NULL);
				break;
			}

			case 8:
			{
				state = Mode_Mix_12_13(UV, NationCode, TypeCode, NULL);
				break;
			}

			case 9:
			{
				state = Mode14_Data(IR, NationCode, TypeCode, NULL);
				break;
			}
			case 10:
			{
				state = Mode15_Data(IR, NationCode, TypeCode, NULL);
				break;
			}
			case 11:
			{
				state = Mode16_Data(IR, NationCode, TypeCode, NULL);
				break;
			}
			case 12:
			{
				state = Mode17_Data(IR, NationCode, TypeCode, NULL);
				break;
			}
			case 13:
			{
				state = Mode18_Data(IR, NationCode, TypeCode, NULL);
				break;
			}
			case 14:
			{
				state = Mode19_Data(MRZ);
				break;
			}
			default:
			{
				return 0;
				break;
			}
			}
		}
	}

	for (int mode = 1; mode < 15; ++mode)
	{
		if ((RCode[mode - 1] != 1) && (RCode[mode - 1] != 5))
			RCode[mode - 1] = 2;
	}


	//if (((RCode[13] == 1) || (RCode[13] == 5)) && (RCode[8] == 1) || (RCode[8] == 5))
	//	RCode[0] = RCode[8];
	//else
	//	RCode[0] = 2;
	RCode[0] = RCode[13];

	return 1;
}


int ContentCheck::UVImageCheck(char* UVImageLoc)
{

	if (UVImageLoc == NULL)
		return 0;

	Mat src = imread(UVImageLoc, 1);

	Mat src1;
	resize(src, src1, Size(1250, 900), 0, 0, INTER_LINEAR);

	Mat SrcImage = src1.clone();

	int state = UV_Check(SrcImage);

	return state;
}

/*int ContentCheck::SourceSeperate(char* ImageLoc, char* MRZ, char* NoValid, int mode)
{
	if (ImageLoc == NULL)
		return 0;

	Mat src1 = imread(ImageLoc, 1);

	if (!src1.data)
	{
		return 0;
	}


	Mat src;
	resize(src1, src, Size(1250, 900), 0, 0, INTER_LINEAR);



	char nation[4];
	char type[3];

	int check = GetInfo(MRZ, nation, type);

	if (check != 1)
		return 0;

	int state = 0;
	int NationCode = NationDetect(nation);
	//cout << NationCode << endl;
	int TypeCode = PassportType(type);
	//cout << TypeCode << endl;

	//strrchr(ImageLoc, '/');


	switch (NationCode)
	{
	case 9:return 5; break;
	case 12:return 5; break;
	case 16:
	{
		if (TypeCode == 15)
			return 5;
		break;
	}
	case 15:return 5; break;
	case 27:return 5; break;
	case 29:return 5; break;
	case 30:return 5; break;
	case 33:return 5; break;
	case 37:return 5; break;
	case 43:return 5; break;
	case 102:return 5; break;
	case 108:return 5; break;
	case 136:return 5; break;
	case 147:return 5; break;
	case 148:return 5; break;
	case 167:return 5; break;
	case 168:return 5; break;
	case 176:return 5; break;
	case 177:return 5; break;
	case 178:return 5; break;
	case 179:return 5; break;


	}

	switch (mode)
	{
	case 1:
	{
		state = Mode1_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 2:
	{
		state = Mode2_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 3:
	{
		state = Mode3_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 4:
	{
		state = Mode4_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 5:
	{
		state = Mode_Mix_5_6(src, NationCode, TypeCode, NULL);
		break;
	}
	case 6:
	{
		state = Mode7_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 7:
	{
		state = Mode_Mix_8_9_10_11(src, NationCode, TypeCode, NULL);
		break;
	}

	case 8:
	{
		state = Mode_Mix_12_13(src, NationCode, TypeCode, NULL);
		break;
	}

	case 9:
	{
		state = Mode14_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 10:
	{
		state = Mode15_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 11:
	{
		state = Mode16_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 12:
	{
		state = Mode17_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 13:
	{
		state = Mode18_Data(src, NationCode, TypeCode, NULL);
		break;
	}
	case 14:
	{
		state = Mode19_Data(MRZ);
		break;
	}
	default:
	{
		src.release();
		return 0;
		break;
	}
	}


	src.release();
	return state;
}*/

