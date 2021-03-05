#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_MUS(Mat& src)
{
	//Mat SrcImg = src.clone();


	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);

	//int(*ImgData)[7] = CAN_DATA;

	//Mat NewImg = SrcImg(Rect(ImgData[0][1], ImgData[0][2], ImgData[0][3], ImgData[0][4]));
	//Mat NewImg = SrcImg(Rect(290, 270, 140, 60));
	Mat NewImg = SrcImg(Rect(230, 220, 240, 160));

	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, ImgData[0][6], ImgData[0][0], ImgData[0][5]);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -50, 3);
	Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -40, 3);

	//imshow("ResultImage3", ResultImage1);
	int check = CheckValid(ResultImage1, 9000, 4000);
	if (!check)
		return 2;

	//NewImg = SrcImg(Rect(360, 110, 110, 70));
	NewImg = SrcImg(Rect(310, 60, 180, 170));

	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, ImgData[0][6], ImgData[0][0], ImgData[0][5]);
	//ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -70, 3);
	ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -60, 3);

	//imshow("ResultImage4", ResultImage1);
	check = CheckValid(ResultImage1, 4000, 700);

	if (!check)
		return 2;

	//NewImg = SrcImg(Rect(360, 110, 110, 70));
	NewImg = SrcImg(Rect(470, 30, 150, 260));

	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, ImgData[0][6], ImgData[0][0], ImgData[0][5]);
	//ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -70, 3);
	ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -60, 3);

	//imshow("ResultImage5", ResultImage1);
	check = CheckValid(ResultImage1, 11000, 4000);


	if (check)
		return 1;
	else
		return 2;
}