#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_NPL(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);
	//SrcImg = src.clone();


	int(*ImgData)[7] = NPL_DATA;

	//Mat NewImg = src(Rect(340, 50, 280, 360));
	Mat NewImg = SrcImg(Rect(ImgData[0][1], ImgData[0][2], ImgData[0][3], ImgData[0][4]));
	//imshow("NewImg1", NewImg);

	Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, ImgData[0][6], ImgData[0][0], ImgData[0][5]);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -20, 1);

	//imshow("ResultImage1", ResultImage1);
	int check = CheckValid(ResultImage1, 35000, 8000);

	if (check == 1)
		return 1;


	//NewImg = src(Rect(150, 30, 300, 250));
	NewImg = SrcImg(Rect(ImgData[1][1], ImgData[1][2], ImgData[1][3], ImgData[1][4]));
	//imshow("NewImg3", NewImg);
	ResultImage1 = Pattern_UV_PROCESS2(NewImg, ImgData[1][6], ImgData[1][0], ImgData[1][5]);

	//imshow("ResultImage3", ResultImage1);
	check = CheckValid(ResultImage1, 35000, 8000);

	if (check == 1)
		return 1;
	else
		return 2;

}