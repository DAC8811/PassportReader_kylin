#include "ContentCheck.h"
#include "PatternData.h"

//int  ContentCheck::Pattern_Check_SAU(Mat& src)
//{
//	Mat SrcImg;
//	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);
//
//	int(*ImgData)[6] = CAN_DATA;
//
//	//Mat NewImg = SrcImg(Rect(ImgData[0][1], ImgData[0][2], ImgData[0][3], ImgData[0][4]));
//
//	Mat ResultImage1;
//	RGB_Black_Process(src, ResultImage1);
//	//RGB_Black_Process(NewImg, ResultImage1);
//
//	imshow("ResultImage1", ResultImage1);
//	int check = CheckValid(ResultImage1, 35000, 15000);
//
//	if (check == 1)
//		return 1;
//	else
//		return 2;
//}