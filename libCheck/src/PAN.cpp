#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_PAN(Mat& src)
{
	Mat SrcImg;
	resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);

	//int(*ImgData)[7] = PAN_DATA;

	//Mat NewImg = SrcImg(Rect(ImgData[0][1], ImgData[0][2], ImgData[0][3], ImgData[0][4]));

	//Mat NewImg = src(Rect(150, 10, 330, 260));
	Mat NewImg = src(Rect(740, 20, 500, 520));


	//Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, ImgData[0][6], ImgData[0][0], ImgData[0][5]);



	Mat ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -40, 1);
	//imshow("ResultImage1", ResultImage1);
	int count1 = countNonZero(ResultImage1);

	if (count1 > 5000)
	{
		NewImg = src(Rect(10, 20, 500, 520));

		ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -40, 1);
		//imshow("ResultImage2", ResultImage1);
		int count2 = countNonZero(ResultImage1);

		if (count1 - count2 < 3000)
			return 2;
		if ((count1 < 11000) | (count1 > 17000))
			return 2;

		if ((count2 < 5000) | (count2 > 11000))
			return 2;

		return 1;
	}
	else
	{
		NewImg = src(Rect(300, 20, 660, 520));

		ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -100, 2);
		//imshow("ResultImage1", ResultImage1);
		int check = CheckValid(ResultImage1, 12000, 5000);

		if (!check)
			return 2;

		NewImg = src(Rect(400, 240, 660, 640));
		ResultImage1 = Pattern_UV_PROCESS2(NewImg, 2, -100, 0);
		//imshow("ResultImage2", ResultImage1);
		check = CheckValid(ResultImage1, 17000, 11000);

		if (check)
			return 1;
		else
			return 2;
	}


}