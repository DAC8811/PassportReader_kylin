#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_IRL(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);
	//SrcImg = src.clone();

	Mat NewImg = SrcImg(Rect(290, 110, 240, 270));

	Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -40, 3);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -50, 3);
	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 4, ImgData[0][0], 1);
	//int CAN_DATA[1][7] = { {-40, 260, 50, 350, 350, 1, 4 } };
	//imshow("ResultImage1", ResultImage1);
	int check = CheckValid(ResultImage1, 15000, 2500);

	if (check)
		return 1;
	else
		return 2;
}