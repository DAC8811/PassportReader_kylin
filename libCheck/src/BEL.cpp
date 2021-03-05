#include "ContentCheck.h"
#include "PatternData.h"

int  ContentCheck::Pattern_Check_BEL(Mat& src)
{
	Mat SrcImg = src.clone();
	//resize(src, SrcImg, Size(src.cols / 2, src.rows / 2), 0, 0, INTER_LINEAR);

	//Mat NewImg = SrcImg(Rect(310, 60, 150, 150));
	//Mat NewImg = SrcImg(Rect(620, 120, 300, 300));

	//imshow("NewImg", SrcImg);

	int check = Thread_UV(SrcImg, 3);

	//cout << "Check: " << check <<endl;

	if (check == 1)
		return 1;
	else
		return 2;
}